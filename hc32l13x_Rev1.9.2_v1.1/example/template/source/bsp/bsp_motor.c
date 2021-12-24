#include "bsp_motor.h"

// ��Ҫ��ͣ�ĵ��,������λ
volatile uint8_t     g_u8MotorMoveFlag;

// ���������־
uint8_t     g_u8MotorRunFlag;

// �����ת����
uint8_t     g_u8MotorDir[5];

//void Tim1_IRQHandler(void)
//{
//    
//    //Timer1 ģʽ23 �����ж�
//    if(TRUE == Bt_GetIntFlag(TIM1, BtUevIrq))
//    {
//        Bt_M23_ARRSet(TIM1, 12000, TRUE);
//        Bt_M23_CCR_Set(TIM1, BtCCR0A, 6000);
//        Bt_ClearIntFlag(TIM1,BtUevIrq);  //���жϱ�־
//    }
//}

void bsp_Motor1TimerInit (void) // TIM1_CHA
{
    stc_bt_mode23_cfg_t        stcBtBaseCfg;
    stc_bt_m23_compare_cfg_t   stcBtPortCmpCfg;
    
    //�ṹ���ʼ������
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE);   //Base Timer����ʱ��ʹ��
        
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //��ݲ�ģʽ
    stcBtBaseCfg.enCT          = BtTimer;                  //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //���ϼ����������ǲ�ģʽʱֻ��
    stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;         //�������PWM
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //����ȽϹ���
    stcBtBaseCfg.bOneShot      = FALSE;                    //ѭ������
    stcBtBaseCfg.bURSSel       = FALSE;                    //���������
    Bt_Mode23_Init(TIM1, &stcBtBaseCfg);                   //TIM0 ��ģʽ23���ܳ�ʼ��
    
    Bt_M23_ARRSet(TIM1, 12000, TRUE);                  //��������ֵ,��ʹ�ܻ��� 4KHz
    Bt_M23_CCR_Set(TIM1, BtCCR0A, 6000);         //���ñȽ�ֵA

    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA�������OCMA:PWMģʽ2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //�������
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //Aͨ���������
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //Aͨ���ȽϿ���:��
 
    Bt_M23_PortOutput_Cfg(TIM1, &stcBtPortCmpCfg);         //�Ƚ�����˿�����        
    Bt_M23_SetValidPeriod(TIM1, 9);             //����������ã�0��ʾ��ݲ�ÿ�����ڸ���һ�Σ�ÿ+1�����ӳ�1������
        
    Bt_M23_Cnt16Set(TIM1, 0);                    //���ü�����ֵ
    Bt_M23_Stop (TIM1);
    Bt_M23_EnPWM_Output (TIM1, TRUE, FALSE);
    
//    Bt_ClearAllIntFlag(TIM1);                              //���жϱ�־
//    EnableNvic(TIM1_IRQn, IrqLevel3, FALSE);                //TIM0�ж�ʹ��
//    Bt_Mode23_DisableIrq(TIM1,BtUevIrq);                    //ʹ��TIM0 UEV�����ж�

}

void bsp_Motor1Start (void)
{
    if (0 == (g_u8MotorRunFlag & MOTOR_FLAG_1))
    {
        MOTOR_1_BRAKE_DISABLE;
        MOTOR_1_PWR_ENABLE;
        
        delay1ms (2);
        
        Bt_M23_ARRSet(TIM1, 12000, TRUE);
        Bt_M23_CCR_Set(TIM1, BtCCR0A, 6000);
//        Bt_ClearAllIntFlag(TIM1); 
//        EnableNvic(TIM1_IRQn, IrqLevel3, TRUE);                //TIM0�ж�ʹ��
//        Bt_Mode23_EnableIrq(TIM1,BtUevIrq);
        Bt_M23_Run (TIM1);
        Gpio_SetAfMode (MOTOR_1_PULSE_PORT, MOTOR_1_PULSE_PIN, GpioAf5);
        g_u8MotorRunFlag |= MOTOR_FLAG_1;
    }
}


void bsp_Motor1Stop (void)
{
    if (g_u8MotorRunFlag & MOTOR_FLAG_1)
    {
        Bt_M23_Stop (TIM1);
    
//        EnableNvic(TIM1_IRQn, IrqLevel3, FALSE);                //TIM0�ж�ʹ��
//        Bt_Mode23_DisableIrq(TIM1,BtUevIrq); 
//        Bt_ClearAllIntFlag(TIM1);                              //���жϱ�־
        
        MOTOR_1_BRAKE_ENABLE;
        MOTOR_1_PWR_DISABLE;
        Gpio_SetAfMode (MOTOR_1_PULSE_PORT, MOTOR_1_PULSE_PIN, GpioAf0);
        MOTOR_1_PULSE_LOW;
        
        g_u8MotorDir[0] = MOTOR_DIR_NONE;
        g_u8MotorRunFlag &= ~MOTOR_FLAG_1;
    }
}

void bsp_Motor1DirSet (uint8_t _u8Dir)
{
    if (g_u8MotorDir[0] != _u8Dir)
    {
        bsp_Motor1Stop ();
        delay1ms (2);
        if (MOTOR_DIR_P == _u8Dir)
        {
            MOTOR_1_DIR_P;
        }
        else
        {
            MOTOR_1_DIR_N;
        }
        delay10us (5);
        bsp_Motor1Start ();
        g_u8MotorDir[0] = _u8Dir;
    }
}



void bsp_Motor2TimerInit (void) // TIM1_CHA
{
    en_adt_compare_t          enAdtCompareA;
//    en_adt_compare_t          enAdtCompareB;

    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIMACfg;
//    stc_adt_CHxX_port_cfg_t   stcAdtTIMBCfg;
    
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIMACfg);
//    DDL_ZERO_STRUCT(stcAdtTIMBCfg);
    

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT����ʱ��ʹ��
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //��ݲ�ģʽ
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    
    Adt_Init(M0P_ADTIM5, &stcAdtBaseCntCfg);                      //ADT�ز�������ģʽ��ʱ������
    
    Adt_SetPeriod(M0P_ADTIM5, 12000);                         //��������
    
    Adt_SetCompareValue(M0P_ADTIM5, AdtCompareA, 6000);  //ͨ�ñȽϻ�׼ֵ�Ĵ���A����
//    Adt_SetCompareValue(M0P_ADTIM5, AdtCompareB, u16CHB_PWMDuty);  //ͨ�ñȽϻ�׼ֵ�Ĵ���B����
    
    stcAdtTIMACfg.enCap = AdtCHxCompareOutput;            //�Ƚ����
    stcAdtTIMACfg.bOutEn = TRUE;                          //CHA���ʹ��
    stcAdtTIMACfg.enPerc = AdtCHxPeriodLow;               //����ֵ������ƥ��ʱCHA��ƽ���ֲ���
    stcAdtTIMACfg.enCmpc = AdtCHxCompareHigh;             //����ֵ��Ƚ�ֵAƥ��ʱ��CHA��ƽ��ת
    stcAdtTIMACfg.enStaStp = AdtCHxStateSelSS;            //CHA��ʼ������ƽ��STACA��STPCA����
    stcAdtTIMACfg.enStaOut = AdtCHxPortOutLow;            //CHA��ʼ��ƽΪ��
    stcAdtTIMACfg.enStpOut = AdtCHxPortOutLow;            //CHA������ƽΪ��
    Adt_CHxXPortCfg(M0P_ADTIM5, AdtCHxA, &stcAdtTIMACfg);   //�˿�CHA����
    
//    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
//    stcAdtTIMBCfg.bOutEn = TRUE;
//    stcAdtTIMBCfg.enPerc = AdtCHxPeriodInv;
//    stcAdtTIMBCfg.enCmpc = AdtCHxCompareInv;
//    stcAdtTIMBCfg.enStaStp = AdtCHxStateSelSS;
//    stcAdtTIMBCfg.enStaOut = AdtCHxPortOutLow;
//    stcAdtTIMBCfg.enStpOut = AdtCHxPortOutLow;
//    Adt_CHxXPortCfg(M0P_ADTIM5, AdtCHxB, &stcAdtTIMBCfg);    //�˿�CHB����
}

void bsp_Motor2Start (void)
{
    if (0 == (g_u8MotorRunFlag & MOTOR_FLAG_2))
    {
        MOTOR_2_BRAKE_DISABLE;
        MOTOR_2_PWR_ENABLE;
        delay1ms (2);
        
        Adt_SetPeriod(M0P_ADTIM5, 12000);                         //��������
        Adt_SetCompareValue(M0P_ADTIM5, AdtCompareA, 6000);
        
        Adt_StartCount(M0P_ADTIM5);
        Gpio_SetAfMode (MOTOR_2_PULSE_PORT, MOTOR_2_PULSE_PIN, GpioAf5);
        g_u8MotorRunFlag |= MOTOR_FLAG_2;
    }
}


void bsp_Motor2Stop (void)
{
    if (g_u8MotorRunFlag & MOTOR_FLAG_2)
    {
        Adt_StopCount(M0P_ADTIM5);
        
        MOTOR_2_BRAKE_ENABLE;
        MOTOR_2_PWR_DISABLE;
        Gpio_SetAfMode (MOTOR_2_PULSE_PORT, MOTOR_2_PULSE_PIN, GpioAf0);
        MOTOR_2_PULSE_LOW;
        
        g_u8MotorDir[1] = MOTOR_DIR_NONE;
        g_u8MotorRunFlag &= ~MOTOR_FLAG_2;
    }
}

void bsp_Motor2DirSet (uint8_t _u8Dir)
{
    if (g_u8MotorDir[1] != _u8Dir)
    {
        bsp_Motor2Stop ();
        delay1ms (2);
        if (MOTOR_DIR_P == _u8Dir)
        {
            MOTOR_2_DIR_P;
        }
        else
        {
            MOTOR_2_DIR_N;
        }
        delay10us (5);
        bsp_Motor2Start ();
        g_u8MotorDir[1] = _u8Dir;
    }
}

void bsp_Motor3_4TimerInit (void) // TIM1_CHA
{
    en_adt_compare_t          enAdtCompareA;
    en_adt_compare_t          enAdtCompareB;

    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIMACfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIMBCfg;
    
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIMACfg);
    DDL_ZERO_STRUCT(stcAdtTIMBCfg);
    

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT����ʱ��ʹ��
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //��ݲ�ģʽ
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    
    Adt_Init(M0P_ADTIM6, &stcAdtBaseCntCfg);                      //ADT�ز�������ģʽ��ʱ������
    
    Adt_SetPeriod(M0P_ADTIM6, 12000);                         //��������
    
    Adt_SetCompareValue(M0P_ADTIM6, AdtCompareA, 6000);  //ͨ�ñȽϻ�׼ֵ�Ĵ���A����
    Adt_SetCompareValue(M0P_ADTIM6, AdtCompareB, 6000);  //ͨ�ñȽϻ�׼ֵ�Ĵ���B����
    
    stcAdtTIMACfg.enCap = AdtCHxCompareOutput;            //�Ƚ����
    stcAdtTIMACfg.bOutEn = TRUE;                          //CHA���ʹ��
    stcAdtTIMACfg.enPerc = AdtCHxPeriodLow;               //����ֵ������ƥ��ʱCHA��ƽ���ֲ���
    stcAdtTIMACfg.enCmpc = AdtCHxCompareHigh;             //����ֵ��Ƚ�ֵAƥ��ʱ��CHA��ƽ��ת
    stcAdtTIMACfg.enStaStp = AdtCHxStateSelSS;            //CHA��ʼ������ƽ��STACA��STPCA����
    stcAdtTIMACfg.enStaOut = AdtCHxPortOutLow;            //CHA��ʼ��ƽΪ��
    stcAdtTIMACfg.enStpOut = AdtCHxPortOutLow;            //CHA������ƽΪ��
    Adt_CHxXPortCfg(M0P_ADTIM6, AdtCHxA, &stcAdtTIMACfg);   //�˿�CHA����
    
    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIMBCfg.bOutEn = TRUE;
    stcAdtTIMBCfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIMBCfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIMBCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIMBCfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIMBCfg.enStpOut = AdtCHxPortOutLow;
    Adt_CHxXPortCfg(M0P_ADTIM6, AdtCHxB, &stcAdtTIMBCfg);    //�˿�CHB����
}

void bsp_Motor3_4Start (void)
{
    if (0 == (g_u8MotorRunFlag & (MOTOR_FLAG_3 | MOTOR_FLAG_4)))
    {
        MOTOR_3_BRAKE_DISABLE;
        MOTOR_4_BRAKE_DISABLE;
        MOTOR_3_PWR_ENABLE;
        MOTOR_4_PWR_ENABLE;
        delay1ms (2);
        
        Adt_SetPeriod(M0P_ADTIM6, 12000);                         //��������
        Adt_SetCompareValue(M0P_ADTIM6, AdtCompareA, 6000);
        
        Gpio_SetAfMode (MOTOR_3_PULSE_PORT, MOTOR_3_PULSE_PIN, GpioAf2);
        Gpio_SetAfMode (MOTOR_4_PULSE_PORT, MOTOR_4_PULSE_PIN, GpioAf7);
        
        MOTOR_3_PWM_ENABLE;
        MOTOR_4_PWM_ENABLE;
        Adt_StartCount(M0P_ADTIM6);
        g_u8MotorRunFlag |= (MOTOR_FLAG_3 | MOTOR_FLAG_4);
    }
}


void bsp_Motor3_4Stop (void)
{
    if (g_u8MotorRunFlag & (MOTOR_FLAG_3 | MOTOR_FLAG_4))
    {
        Adt_StopCount(M0P_ADTIM6);
        MOTOR_3_PWM_DISABLE;
        MOTOR_4_PWM_DISABLE;
        
        MOTOR_3_BRAKE_ENABLE;
        MOTOR_4_BRAKE_ENABLE;
        MOTOR_3_PWR_DISABLE;
        MOTOR_4_PWR_DISABLE;
        Gpio_SetAfMode (MOTOR_3_PULSE_PORT, MOTOR_3_PULSE_PIN, GpioAf0);
        Gpio_SetAfMode (MOTOR_4_PULSE_PORT, MOTOR_4_PULSE_PIN, GpioAf0);
        MOTOR_3_PULSE_LOW;
        MOTOR_4_PULSE_LOW;
        g_u8MotorDir[2] = MOTOR_DIR_NONE;
        g_u8MotorRunFlag &= ~(MOTOR_FLAG_3 | MOTOR_FLAG_4);
    }
}

void bsp_Motor3_4DirSet (uint8_t _u8Dir)
{
    if (g_u8MotorDir[2] != _u8Dir)
    {
        bsp_Motor3_4Stop ();
        delay1ms (2);
        if (MOTOR_DIR_P == _u8Dir)
        {
            MOTOR_3_DIR_P;
            MOTOR_4_DIR_P;
            g_u8MotorMoveFlag |= (LIMITER_FLAG_SWEEP_LEFT_OUT | LIMITER_FLAG_SWEEP_RIGHT_OUT);
        }
        else
        {
            MOTOR_3_DIR_N;
            MOTOR_4_DIR_N;
            g_u8MotorMoveFlag |= (LIMITER_FLAG_SWEEP_LEFT_IN | LIMITER_FLAG_SWEEP_RIGHT_IN);
        }
        delay10us (5);
        bsp_Motor3_4Start ();
        g_u8MotorDir[2] = _u8Dir;
    }
}

void bsp_Motor3_4_PwmStop (uint8_t _u8Type)
{
    if (g_u8MotorRunFlag & (MOTOR_FLAG_3 | MOTOR_FLAG_4))
    {
        if (MOTOR_3_PWM_STOP == _u8Type)
        {
            MOTOR_3_PWM_DISABLE;
            MOTOR_3_BRAKE_ENABLE;
            MOTOR_3_PWR_DISABLE;
            Gpio_SetAfMode (MOTOR_3_PULSE_PORT, MOTOR_3_PULSE_PIN, GpioAf0);
            MOTOR_3_PULSE_LOW;
            g_u8MotorRunFlag &= ~MOTOR_FLAG_3;
            g_u8MotorMoveFlag &= ~(LIMITER_FLAG_SWEEP_LEFT_OUT | LIMITER_FLAG_SWEEP_LEFT_IN);
        }
        else 
        {
            MOTOR_4_PWM_DISABLE;
            MOTOR_4_BRAKE_ENABLE;
            MOTOR_4_PWR_DISABLE;
            Gpio_SetAfMode (MOTOR_4_PULSE_PORT, MOTOR_4_PULSE_PIN, GpioAf0);
            MOTOR_4_PULSE_LOW;
            g_u8MotorRunFlag &= ~MOTOR_FLAG_4;
            g_u8MotorMoveFlag &= ~(LIMITER_FLAG_SWEEP_RIGHT_OUT | LIMITER_FLAG_SWEEP_RIGHT_IN);
        }
        
        if (0 == (g_u8MotorRunFlag & (MOTOR_FLAG_3 | MOTOR_FLAG_4)))
        {
            g_u8MotorDir[2] = MOTOR_DIR_NONE;
            Adt_StopCount(M0P_ADTIM6);
            
        }
    }
}

void bsp_Motor5TimerInit (void) // TIM1_CHA
{
    en_adt_compare_t          enAdtCompareA;
//    en_adt_compare_t          enAdtCompareB;

    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIMACfg;
//    stc_adt_CHxX_port_cfg_t   stcAdtTIMBCfg;
    
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIMACfg);
//    DDL_ZERO_STRUCT(stcAdtTIMBCfg);
    

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT����ʱ��ʹ��
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //��ݲ�ģʽ
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                      //ADT�ز�������ģʽ��ʱ������
    
    Adt_SetPeriod(M0P_ADTIM4, 12000);                         //��������
    
    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareA, 6000);  //ͨ�ñȽϻ�׼ֵ�Ĵ���A����
//    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareB, u16CHB_PWMDuty);  //ͨ�ñȽϻ�׼ֵ�Ĵ���B����
    
    stcAdtTIMACfg.enCap = AdtCHxCompareOutput;            //�Ƚ����
    stcAdtTIMACfg.bOutEn = TRUE;                          //CHA���ʹ��
    stcAdtTIMACfg.enPerc = AdtCHxPeriodLow;               //����ֵ������ƥ��ʱCHA��ƽ���ֲ���
    stcAdtTIMACfg.enCmpc = AdtCHxCompareHigh;             //����ֵ��Ƚ�ֵAƥ��ʱ��CHA��ƽ��ת
    stcAdtTIMACfg.enStaStp = AdtCHxStateSelSS;            //CHA��ʼ������ƽ��STACA��STPCA����
    stcAdtTIMACfg.enStaOut = AdtCHxPortOutLow;            //CHA��ʼ��ƽΪ��
    stcAdtTIMACfg.enStpOut = AdtCHxPortOutLow;            //CHA������ƽΪ��
    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxA, &stcAdtTIMACfg);   //�˿�CHA����
    
//    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
//    stcAdtTIMBCfg.bOutEn = TRUE;
//    stcAdtTIMBCfg.enPerc = AdtCHxPeriodInv;
//    stcAdtTIMBCfg.enCmpc = AdtCHxCompareInv;
//    stcAdtTIMBCfg.enStaStp = AdtCHxStateSelSS;
//    stcAdtTIMBCfg.enStaOut = AdtCHxPortOutLow;
//    stcAdtTIMBCfg.enStpOut = AdtCHxPortOutLow;
//    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxB, &stcAdtTIMBCfg);    //�˿�CHB����
}

void bsp_Motor5Start (void)
{
    if (0 == (g_u8MotorRunFlag & MOTOR_FLAG_5))
    {
        MOTOR_5_BRAKE_DISABLE;
        MOTOR_5_PWR_ENABLE;
        delay1ms (2);
        
        Adt_SetPeriod(M0P_ADTIM4, 12000);                         //��������
        Adt_SetCompareValue(M0P_ADTIM4, AdtCompareA, 6000);
        
        Adt_StartCount(M0P_ADTIM4);
        Gpio_SetAfMode (MOTOR_5_PULSE_PORT, MOTOR_5_PULSE_PIN, GpioAf2);
        g_u8MotorRunFlag |= MOTOR_FLAG_5;
    }
}


void bsp_Motor5Stop (void)
{
    if (g_u8MotorRunFlag & MOTOR_FLAG_5)
    {
        Adt_StopCount(M0P_ADTIM4);
        
        MOTOR_5_PWR_DISABLE;
        MOTOR_5_BRAKE_ENABLE;
        
        Gpio_SetAfMode (MOTOR_5_PULSE_PORT, MOTOR_5_PULSE_PIN, GpioAf0);
        MOTOR_5_PULSE_LOW;
        
        g_u8MotorDir[4] = MOTOR_DIR_NONE;
        g_u8MotorRunFlag &= ~MOTOR_FLAG_5;
    }
}

void bsp_Motor5DirSet (uint8_t _u8Dir)
{
    if (g_u8MotorDir[4] != _u8Dir)
    {
        bsp_Motor5Stop ();
        delay1ms (2);
        if (MOTOR_DIR_P == _u8Dir)
        {
            MOTOR_5_DIR_P;
        }
        else
        {
            MOTOR_5_DIR_N;
        }
        delay10us (5);
        bsp_Motor5Start ();
        g_u8MotorDir[4] = _u8Dir;
    }
}


void bsp_MotorGpioInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    stcGpioCfg.enDir = GpioDirOut;
    ///< �˿�����������->����
    
    ///< LED�ر�
//    Gpio_SetIO(UV_EN_PORT, UV_EN_PIN);
    MOTOR_1_PWR_DISABLE;
    MOTOR_2_PWR_DISABLE;
    MOTOR_3_PWR_DISABLE;
    MOTOR_4_PWR_DISABLE;
    MOTOR_5_PWR_DISABLE;
    
    MOTOR_1_PULSE_LOW;
    MOTOR_2_PULSE_LOW;
    MOTOR_3_PULSE_LOW;
    MOTOR_4_PULSE_LOW;
    MOTOR_5_PULSE_LOW;
    
    MOTOR_1_DIR_LOW;
    MOTOR_2_DIR_LOW;
    MOTOR_3_DIR_LOW;
    MOTOR_4_DIR_LOW;
    MOTOR_5_DIR_LOW;
    
    MOTOR_5_BRAKE_ENABLE;

    ///< GPIO IO LED�˿ڳ�ʼ��
    Gpio_Init(MOTOR_1_PWR_PORT, MOTOR_1_PWR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_2_PWR_PORT, MOTOR_2_PWR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_3_PWR_PORT, MOTOR_3_PWR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_4_PWR_PORT, MOTOR_4_PWR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_5_PWR_PORT, MOTOR_5_PWR_PIN, &stcGpioCfg);
    
    Gpio_Init(MOTOR_1_PULSE_PORT, MOTOR_1_PULSE_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_2_PULSE_PORT, MOTOR_2_PULSE_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_3_PULSE_PORT, MOTOR_3_PULSE_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_4_PULSE_PORT, MOTOR_4_PULSE_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_5_PULSE_PORT, MOTOR_5_PULSE_PIN, &stcGpioCfg);

    Gpio_Init(MOTOR_1_DIR_PORT, MOTOR_1_DIR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_2_DIR_PORT, MOTOR_2_DIR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_3_DIR_PORT, MOTOR_3_DIR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_4_DIR_PORT, MOTOR_4_DIR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_5_DIR_PORT, MOTOR_5_DIR_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_BRAKE_PORT, MOTOR_BRAKE_PIN, &stcGpioCfg);
    
    
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(MOTOR_1_NP_PORT, MOTOR_1_NP_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_2_NP_PORT, MOTOR_2_NP_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_3_NP_PORT, MOTOR_3_NP_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_4_NP_PORT, MOTOR_4_NP_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_5_NP_PORT, MOTOR_5_NP_PIN, &stcGpioCfg);
    
    Gpio_Init(MOTOR_1_NN_PORT, MOTOR_1_NN_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_2_NN_PORT, MOTOR_2_NN_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_3_NN_PORT, MOTOR_3_NN_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_4_NN_PORT, MOTOR_4_NN_PIN, &stcGpioCfg);
    Gpio_Init(MOTOR_5_NN_PORT, MOTOR_5_NN_PIN, &stcGpioCfg);
}

void bsp_MotorInit (void)
{
    bsp_MotorGpioInit ();
    bsp_Motor1TimerInit ();
    bsp_Motor2TimerInit ();
    bsp_Motor3_4TimerInit ();
    bsp_Motor5TimerInit ();
}

