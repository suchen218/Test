#include "bsp_timer.h"

void bsp_SysClkInit (void)  // 4M -> 48M
{
    stc_sysctrl_clk_cfg_t stcCfg;
    stc_sysctrl_pll_cfg_t stcPLLCfg;
    
    ///< 开启FLASH外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);
    
    ///<========================== 时钟初始化配置 ===================================
    ///< 因要使用的时钟源HCLK小于24M：此处设置FLASH 读等待周期为0 cycle(默认值也为0 cycle)
    Flash_WaitCycle(FlashWaitCycle1);
    
    ///< 时钟初始化前，优先设置要使用的时钟源：此处设置RCH为4MHz
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;   //RCH 24MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL 输出48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //输入时钟源选择RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;             //24MHz x 2 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    ///< 时钟切换
    Sysctrl_SysClkSwitch(SysctrlClkPLL);
    
    ///< 选择内部RCH作为HCLK时钟源;
//    stcCfg.enClkSrc    = SysctrlClkPLL;
//    ///< HCLK SYSCLK/1
//    stcCfg.enHClkDiv   = SysctrlHclkDiv1;
//    ///< PCLK 为HCLK/1
//    stcCfg.enPClkDiv   = SysctrlPclkDiv1;
//    ///< 系统时钟初始化
//    Sysctrl_ClkInit(&stcCfg);
}

uint8_t         g_u8TimTicks;

void Tim0_IRQHandler(void)
{
    
    //Timer1 模式23 更新中断
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        Bt_ClearIntFlag(TIM0,BtUevIrq);  //清中断标志
        
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
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //定时器模式
    stcBtBaseCfg.enCT       = BtTimer;                      //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv256;                 //PCLK/256
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //自动重载16位计数器/定时器
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 的模式0功能初始化
    
    u16ArrValue = 0x10000 - 3750;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //设置重载值(ARR = 0x10000 - 周期)
    
    u16CntValue = 0x10000 - 3750;
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //设置计数初值
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //清中断标志   
    Bt_Mode0_EnableIrq(TIM0);                               //使能TIM0中断(模式0时只有一个中断)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE); 

    Bt_M0_Run (TIM0);
}


