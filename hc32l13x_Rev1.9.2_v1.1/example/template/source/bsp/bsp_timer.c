#include "bsp_timer.h"

void bsp_SysClkInit (void)  // 4M -> 48M
{
    stc_sysctrl_clk_cfg_t stcCfg;
    stc_sysctrl_pll_cfg_t stcPLLCfg;
    
    ///< ����FLASH����ʱ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);
    
    ///<========================== ʱ�ӳ�ʼ������ ===================================
    ///< ��Ҫʹ�õ�ʱ��ԴHCLKС��24M���˴�����FLASH ���ȴ�����Ϊ0 cycle(Ĭ��ֵҲΪ0 cycle)
    Flash_WaitCycle(FlashWaitCycle1);
    
    ///< ʱ�ӳ�ʼ��ǰ����������Ҫʹ�õ�ʱ��Դ���˴�����RCHΪ4MHz
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;   //RCH 24MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL ���48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //����ʱ��Դѡ��RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;             //24MHz x 2 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    ///< ʱ���л�
    Sysctrl_SysClkSwitch(SysctrlClkPLL);
    
    ///< ѡ���ڲ�RCH��ΪHCLKʱ��Դ;
//    stcCfg.enClkSrc    = SysctrlClkPLL;
//    ///< HCLK SYSCLK/1
//    stcCfg.enHClkDiv   = SysctrlHclkDiv1;
//    ///< PCLK ΪHCLK/1
//    stcCfg.enPClkDiv   = SysctrlPclkDiv1;
//    ///< ϵͳʱ�ӳ�ʼ��
//    Sysctrl_ClkInit(&stcCfg);
}

uint8_t         g_u8TimTicks;

void Tim0_IRQHandler(void)
{
    
    //Timer1 ģʽ23 �����ж�
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        Bt_ClearIntFlag(TIM0,BtUevIrq);  //���жϱ�־
        
        bsp_CtrlSteps ();
        bsp_WifiRcvtimeout ();
        
        g_u8TimTicks++;
        if (g_u8TimTicks >= 50)
        {
            g_u8TimTicks = 0;
            bsp_MsgSend (MSG_TYPE_FRESH_HALF_SECOND);
        }
    }
}

void bsp_TimerInit (void) // TIM0
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_bt_mode0_cfg_t     stcBtBaseCfg;
    
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer����ʱ��ʹ��
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //��ʱ��ģʽ
    stcBtBaseCfg.enCT       = BtTimer;                      //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv256;                 //PCLK/256
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //�Զ�����16λ������/��ʱ��
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ��ģʽ0���ܳ�ʼ��
    
    u16ArrValue = 0x10000 - 3750;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //��������ֵ(ARR = 0x10000 - ����)
    
    u16CntValue = 0x10000 - 3750;
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //���ü�����ֵ
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //���жϱ�־   
    Bt_Mode0_EnableIrq(TIM0);                               //ʹ��TIM0�ж�(ģʽ0ʱֻ��һ���ж�)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE); 

    Bt_M0_Run (TIM0);
}


