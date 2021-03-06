/******************************************************************************
* Copyright (C) 2019, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2019-04-17  1.0  Husj First version for General Timer012.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "bt.h"
#include "flash.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint32_t u32PwcCapValue;
volatile uint16_t u16TIM0_CntValue;

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/*******************************************************************************
 * TIM0??????????????????
 ******************************************************************************/
void Tim0_IRQHandler(void)
{
    static uint16_t u16TIM0_OverFlowCnt;
    
    static uint16_t u16TIM0_CapValue;
     
    //Timer0 ??????1 ??????????????????
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        u16TIM0_OverFlowCnt++;        //??????????????????????????????????????????????????????timer???????????????
        
        Bt_ClearIntFlag(TIM0,BtUevIrq); //??????????????????
    }

    //Timer0 ??????1 PWC??????????????????
    if(TRUE == Bt_GetIntFlag(TIM0, BtCA0Irq))
    {
        u16TIM0_CntValue = Bt_M1_Cnt16Get(TIM0);         //?????????????????????
        u16TIM0_CapValue = Bt_M1_PWC_CapValueGet(TIM0);  //?????????????????????
        
        u32PwcCapValue = u16TIM0_OverFlowCnt*0x10000 + u16TIM0_CapValue;
        
        u16TIM0_OverFlowCnt = 0;
        
        Bt_ClearIntFlag(TIM0, BtCA0Irq); //??????????????????
    }
}

//?????????????????????
void App_ClockCfg(void)
{
    en_flash_waitcycle_t      enWaitCycle;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enWaitCycle = FlashWaitCycle1;
    Flash_WaitCycle(enWaitCycle);
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL ??????48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //?????????????????????RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);  ///< ????????????
}

//?????????????????????
void App_Timer0PortCfg(void)
{
    stc_gpio_cfg_t          stcTIM0Port;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcTIM0Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ??????????????????
    
    //PA00?????????TIM0_CHA
    stcTIM0Port.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);
    
    //PA01?????????TIM0_CHB
    //Gpio_Init(GpioPortA, GpioPin1, &stcTIM0Port);
    //Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);
}

//Timer0 ??????
void App_Timer0Cfg(void)
{
    uint16_t                u16CntValue;
    stc_bt_mode1_cfg_t      stcBtBaseCfg;
    stc_bt_pwc_input_cfg_t  stcBtPwcInCfg;

    //????????????????????????
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPwcInCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer??????????????????
    
    stcBtBaseCfg.enWorkMode = BtWorkMode1;                  //???????????????
    stcBtBaseCfg.enCT       = BtTimer;                      //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv1;                   //PCLK
    stcBtBaseCfg.enOneShot  = BtPwcCycleDetect;             //PWC????????????
    Bt_Mode1_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ?????????1???????????????
    
    stcBtPwcInCfg.enTsSel  = BtTs6IAFP;                     //PWC???????????? CHA
    stcBtPwcInCfg.enIA0Sel = BtIA0Input;                    //CHA??????IA0
    stcBtPwcInCfg.enFltIA0 = BtFltPCLKDiv16Cnt3;            //PCLK/16 3???????????????
    //stcBtPwcInCfg.enIB0Sel = BtIB0Input;                  //CHB??????IB0
    //stcBtPwcInCfg.enFltIB0 = BtFltPCLKDiv16Cnt3;          //PCLK/16 3???????????????
    Bt_M1_Input_Cfg(TIM0, &stcBtPwcInCfg);                  //PWC????????????
    
    Bt_M1_PWC_Edge_Sel(TIM0, BtPwcRiseToRise);              //???????????????????????????
    
    u16CntValue = 0;
    Bt_M1_Cnt16Set(TIM0, u16CntValue);                      //??????????????????  
    
    Bt_ClearAllIntFlag(TIM0);                               //???????????????
    Bt_Mode1_EnableIrq(TIM0, BtUevIrq);                     //??????TIM0????????????
    Bt_Mode1_EnableIrq(TIM0, BtCA0Irq);                     //??????TIM0????????????
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0????????????
}

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

int32_t main(void)
{
    App_ClockCfg();         //?????????????????????
    
    App_Timer0PortCfg();    //?????????????????????
    
    App_Timer0Cfg();        //Timer0???????????????

    Bt_M1_Run(TIM0);        //TIM0 ??????
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


