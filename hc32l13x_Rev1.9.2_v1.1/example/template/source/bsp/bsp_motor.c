#include "bsp_motor.h"

// 需要暂停的电机,包括方位
volatile uint8_t     g_u8MotorMoveFlag;

// 电机启动标志
uint8_t     g_u8MotorRunFlag;

// 电机旋转方向
uint8_t     g_u8MotorDir[5];

//void Tim1_IRQHandler(void)
//{
//    
//    //Timer1 模式23 更新中断
//    if(TRUE == Bt_GetIntFlag(TIM1, BtUevIrq))
//    {
//        Bt_M23_ARRSet(TIM1, 12000, TRUE);
//        Bt_M23_CCR_Set(TIM1, BtCCR0A, 6000);
//        Bt_ClearIntFlag(TIM1,BtUevIrq);  //清中断标志
//    }
//}

void bsp_Motor1TimerInit (void) // TIM1_CHA
{
    stc_bt_mode23_cfg_t        stcBtBaseCfg;
    stc_bt_m23_compare_cfg_t   stcBtPortCmpCfg;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE);   //Base Timer外设时钟使能
        
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //锯齿波模式
    stcBtBaseCfg.enCT          = BtTimer;                  //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //向上计数，在三角波模式时只读
    stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;         //独立输出PWM
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //单点比较功能
    stcBtBaseCfg.bOneShot      = FALSE;                    //循环计数
    stcBtBaseCfg.bURSSel       = FALSE;                    //上下溢更新
    Bt_Mode23_Init(TIM1, &stcBtBaseCfg);                   //TIM0 的模式23功能初始化
    
    Bt_M23_ARRSet(TIM1, 12000, TRUE);                  //设置重载值,并使能缓存 4KHz
    Bt_M23_CCR_Set(TIM1, BtCCR0A, 6000);         //设置比较值A

    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA输出控制OCMA:PWM模式2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //正常输出
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A通道缓存控制
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A通道比较控制:无
 
    Bt_M23_PortOutput_Cfg(TIM1, &stcBtPortCmpCfg);         //比较输出端口配置        
    Bt_M23_SetValidPeriod(TIM1, 9);             //间隔周期设置，0表示锯齿波每个周期更新一次，每+1代表延迟1个周期
        
    Bt_M23_Cnt16Set(TIM1, 0);                    //设置计数初值
    Bt_M23_Stop (TIM1);
    Bt_M23_EnPWM_Output (TIM1, TRUE, FALSE);
    
//    Bt_ClearAllIntFlag(TIM1);                              //清中断标志
//    EnableNvic(TIM1_IRQn, IrqLevel3, FALSE);                //TIM0中断使能
//    Bt_Mode23_DisableIrq(TIM1,BtUevIrq);                    //使能TIM0 UEV更新中断

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
//        EnableNvic(TIM1_IRQn, IrqLevel3, TRUE);                //TIM0中断使能
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
    
//        EnableNvic(TIM1_IRQn, IrqLevel3, FALSE);                //TIM0中断使能
//        Bt_Mode23_DisableIrq(TIM1,BtUevIrq); 
//        Bt_ClearAllIntFlag(TIM1);                              //清中断标志
        
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
    

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT外设时钟使能
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //锯齿波模式
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    
    Adt_Init(M0P_ADTIM5, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置
    
    Adt_SetPeriod(M0P_ADTIM5, 12000);                         //周期设置
    
    Adt_SetCompareValue(M0P_ADTIM5, AdtCompareA, 6000);  //通用比较基准值寄存器A设置
//    Adt_SetCompareValue(M0P_ADTIM5, AdtCompareB, u16CHB_PWMDuty);  //通用比较基准值寄存器B设置
    
    stcAdtTIMACfg.enCap = AdtCHxCompareOutput;            //比较输出
    stcAdtTIMACfg.bOutEn = TRUE;                          //CHA输出使能
    stcAdtTIMACfg.enPerc = AdtCHxPeriodLow;               //计数值与周期匹配时CHA电平保持不变
    stcAdtTIMACfg.enCmpc = AdtCHxCompareHigh;             //计数值与比较值A匹配时，CHA电平翻转
    stcAdtTIMACfg.enStaStp = AdtCHxStateSelSS;            //CHA起始结束电平由STACA与STPCA控制
    stcAdtTIMACfg.enStaOut = AdtCHxPortOutLow;            //CHA起始电平为低
    stcAdtTIMACfg.enStpOut = AdtCHxPortOutLow;            //CHA结束电平为低
    Adt_CHxXPortCfg(M0P_ADTIM5, AdtCHxA, &stcAdtTIMACfg);   //端口CHA配置
    
//    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
//    stcAdtTIMBCfg.bOutEn = TRUE;
//    stcAdtTIMBCfg.enPerc = AdtCHxPeriodInv;
//    stcAdtTIMBCfg.enCmpc = AdtCHxCompareInv;
//    stcAdtTIMBCfg.enStaStp = AdtCHxStateSelSS;
//    stcAdtTIMBCfg.enStaOut = AdtCHxPortOutLow;
//    stcAdtTIMBCfg.enStpOut = AdtCHxPortOutLow;
//    Adt_CHxXPortCfg(M0P_ADTIM5, AdtCHxB, &stcAdtTIMBCfg);    //端口CHB配置
}

void bsp_Motor2Start (void)
{
    if (0 == (g_u8MotorRunFlag & MOTOR_FLAG_2))
    {
        MOTOR_2_BRAKE_DISABLE;
        MOTOR_2_PWR_ENABLE;
        delay1ms (2);
        
        Adt_SetPeriod(M0P_ADTIM5, 12000);                         //周期设置
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
    

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT外设时钟使能
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //锯齿波模式
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    
    Adt_Init(M0P_ADTIM6, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置
    
    Adt_SetPeriod(M0P_ADTIM6, 12000);                         //周期设置
    
    Adt_SetCompareValue(M0P_ADTIM6, AdtCompareA, 6000);  //通用比较基准值寄存器A设置
    Adt_SetCompareValue(M0P_ADTIM6, AdtCompareB, 6000);  //通用比较基准值寄存器B设置
    
    stcAdtTIMACfg.enCap = AdtCHxCompareOutput;            //比较输出
    stcAdtTIMACfg.bOutEn = TRUE;                          //CHA输出使能
    stcAdtTIMACfg.enPerc = AdtCHxPeriodLow;               //计数值与周期匹配时CHA电平保持不变
    stcAdtTIMACfg.enCmpc = AdtCHxCompareHigh;             //计数值与比较值A匹配时，CHA电平翻转
    stcAdtTIMACfg.enStaStp = AdtCHxStateSelSS;            //CHA起始结束电平由STACA与STPCA控制
    stcAdtTIMACfg.enStaOut = AdtCHxPortOutLow;            //CHA起始电平为低
    stcAdtTIMACfg.enStpOut = AdtCHxPortOutLow;            //CHA结束电平为低
    Adt_CHxXPortCfg(M0P_ADTIM6, AdtCHxA, &stcAdtTIMACfg);   //端口CHA配置
    
    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIMBCfg.bOutEn = TRUE;
    stcAdtTIMBCfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIMBCfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIMBCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIMBCfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIMBCfg.enStpOut = AdtCHxPortOutLow;
    Adt_CHxXPortCfg(M0P_ADTIM6, AdtCHxB, &stcAdtTIMBCfg);    //端口CHB配置
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
        
        Adt_SetPeriod(M0P_ADTIM6, 12000);                         //周期设置
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
    

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT外设时钟使能
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //锯齿波模式
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置
    
    Adt_SetPeriod(M0P_ADTIM4, 12000);                         //周期设置
    
    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareA, 6000);  //通用比较基准值寄存器A设置
//    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareB, u16CHB_PWMDuty);  //通用比较基准值寄存器B设置
    
    stcAdtTIMACfg.enCap = AdtCHxCompareOutput;            //比较输出
    stcAdtTIMACfg.bOutEn = TRUE;                          //CHA输出使能
    stcAdtTIMACfg.enPerc = AdtCHxPeriodLow;               //计数值与周期匹配时CHA电平保持不变
    stcAdtTIMACfg.enCmpc = AdtCHxCompareHigh;             //计数值与比较值A匹配时，CHA电平翻转
    stcAdtTIMACfg.enStaStp = AdtCHxStateSelSS;            //CHA起始结束电平由STACA与STPCA控制
    stcAdtTIMACfg.enStaOut = AdtCHxPortOutLow;            //CHA起始电平为低
    stcAdtTIMACfg.enStpOut = AdtCHxPortOutLow;            //CHA结束电平为低
    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxA, &stcAdtTIMACfg);   //端口CHA配置
    
//    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
//    stcAdtTIMBCfg.bOutEn = TRUE;
//    stcAdtTIMBCfg.enPerc = AdtCHxPeriodInv;
//    stcAdtTIMBCfg.enCmpc = AdtCHxCompareInv;
//    stcAdtTIMBCfg.enStaStp = AdtCHxStateSelSS;
//    stcAdtTIMBCfg.enStaOut = AdtCHxPortOutLow;
//    stcAdtTIMBCfg.enStpOut = AdtCHxPortOutLow;
//    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxB, &stcAdtTIMBCfg);    //端口CHB配置
}

void bsp_Motor5Start (void)
{
    if (0 == (g_u8MotorRunFlag & MOTOR_FLAG_5))
    {
        MOTOR_5_BRAKE_DISABLE;
        MOTOR_5_PWR_ENABLE;
        delay1ms (2);
        
        Adt_SetPeriod(M0P_ADTIM4, 12000);                         //周期设置
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
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口上下拉配置->下拉
    
    ///< LED关闭
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

    ///< GPIO IO LED端口初始化
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

