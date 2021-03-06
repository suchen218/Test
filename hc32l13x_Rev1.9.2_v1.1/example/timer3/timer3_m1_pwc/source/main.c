/******************************************************************************
* Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
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
 **   - 2019-04-18  1.0  Husj First version for Timer3.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "timer3.h"
#include "flash.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint32_t u32PwcCapValue;
volatile uint16_t u16TIM3_CntValue;

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
 * TIM3??????????????????
 ******************************************************************************/
void Tim3_IRQHandler(void)
{
    static uint16_t u16TIM3_OverFlowCnt;
    
    static uint16_t u16TIM3_CapValue;
     
    //Timer3 ??????1 ??????????????????
    if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
    {
        u16TIM3_OverFlowCnt++;         //??????????????????????????????????????????????????????timer???????????????
        
        Tim3_ClearIntFlag(Tim3UevIrq); //??????????????????
    }

    //Timer3 ??????1 PWC??????????????????
    if(TRUE == Tim3_GetIntFlag(Tim3CA0Irq))
    {
        u16TIM3_CntValue = Tim3_M1_Cnt16Get();         //?????????????????????
        u16TIM3_CapValue = Tim3_M1_PWC_CapValueGet();  //?????????????????????
        
        u32PwcCapValue = u16TIM3_OverFlowCnt*0x10000+ u16TIM3_CapValue;
        
        u16TIM3_OverFlowCnt = 0;
        
        Tim3_ClearIntFlag(Tim3CA0Irq); //??????????????????
    }
}

//???????????????
void App_ClockCfg(void)
{
    en_flash_waitcycle_t         enFlashWait;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL ??????48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //?????????????????????RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);///< ????????????
}

//Timer3 Port????????????
void App_Timer3PortCfg(void)
{
    stc_gpio_cfg_t            stcTIM3Port;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcTIM3Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //????????????????????????
    
    stcTIM3Port.enDir  = GpioDirIn;
    //PA08?????????TIM3_CH0A
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);
    
    //PA07?????????TIM3_CH0B
    //Gpio_Init(GpioPortA, GpioPin7, &stcTIM3Port);
    //Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);
}

//Timer3 ??????
void App_Timer3Cfg(void)
{
    uint16_t                     u16CntValue;

    stc_tim3_mode1_cfg_t      stcTim3BaseCfg;
    stc_tim3_pwc_input_cfg_t  stcTim3PwcInCfg;

    //????????????????????????
    DDL_ZERO_STRUCT(stcTim3BaseCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Timer3 ??????????????????
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode1;                //???????????????
    stcTim3BaseCfg.enCT       = Tim3Timer;                    //???????????????????????????????????????PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv1;                 //PCLK
    stcTim3BaseCfg.enOneShot  = Tim3PwcCycleDetect;           //PWC????????????
    
    Tim3_Mode1_Init(&stcTim3BaseCfg);                         //TIM3 ?????????1???????????????
    
    stcTim3PwcInCfg.enTsSel  = Tim3Ts6IAFP;                   //PWC???????????? CHA
    stcTim3PwcInCfg.enIA0Sel = Tim3IA0Input;                  //CHA??????IA0
    stcTim3PwcInCfg.enFltIA0 = Tim3FltPCLKDiv16Cnt3;          //PCLK/16 3???????????????
    //stcTim3PwcInCfg.enIB0Sel = Tim3IB0Input;                //CHB??????IB0
    //stcTim3PwcInCfg.enFltIB0 = Tim3FltPCLKDiv16Cnt3;        //PCLK/16 3???????????????
    Tim3_M1_Input_Cfg(&stcTim3PwcInCfg);                   //PWC????????????
    
    Tim3_M1_PWC_Edge_Sel(Tim3PwcRiseToRise);                  //?????????????????????????????????
    
    u16CntValue = 0;
    Tim3_M1_Cnt16Set(u16CntValue);                            //??????????????????
    
    Tim3_ClearIntFlag(Tim3UevIrq);                            //???Uev????????????
    Tim3_ClearIntFlag(Tim3CA0Irq);                            //?????????????????????
    Tim3_Mode1_EnableIrq(Tim3UevIrq);                         //??????TIM3????????????
    Tim3_Mode1_EnableIrq(Tim3CA0Irq);                         //??????TIM3????????????
    EnableNvic(TIM3_IRQn, IrqLevel3, TRUE);                   //TIM3????????????
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
    App_ClockCfg();      //???????????????
    
    App_Timer3PortCfg(); //Timer3 Port????????????
    
    App_Timer3Cfg();     //Timer3 ??????;
        
    Tim3_M1_Run();       //TIM3 ?????????
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


