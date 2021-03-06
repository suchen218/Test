/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
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
 **   - 2019-04-22  Husj    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adt.h"
#include "adc.h"
#include "gpio.h"
#include "flash.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
uint16_t u16AdcRestult0;
uint16_t u16AdcRestult2;
   
/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/



/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void Adc_IRQHandler(void)
{
    //ADC ????????????????????????
    if(TRUE == Adc_GetIrqStatus(AdcMskIrqSqr))
    {
        u16AdcRestult0 = (uint16_t)Adc_GetSqrResult(AdcSQRCH0MUX); //????????????0(PA00)???????????????
        u16AdcRestult2 = (uint16_t)Adc_GetSqrResult(AdcSQRCH1MUX); //????????????1(PA02)???????????????

        Adc_ClrIrqStatus(AdcMskIrqSqr);  //??????????????????????????????
    }
}

///< ???????????????
void App_ClockInit(void)
{
    en_flash_waitcycle_t      enFlashWait;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
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
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);   ///< ????????????
}

///< AdvTimer?????????
void App_AdvTimerInit(uint16_t u16Period)
{
    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
    stc_adt_irq_trig_cfg_t    stcAdtIrqTrigCfg;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtIrqTrigCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);  //ADT??????????????????
    
    //ADT Timer4??????ADC?????????Frequency: 1K
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;              //?????????
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;                //PCLK
    
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                   //ADT????????????????????????????????????
    
    Adt_SetPeriod(M0P_ADTIM4, u16Period);                      //????????????

    stcAdtIrqTrigCfg.bAdtOverFlowTrigEn = TRUE;
    Adt_IrqTrigCfg(M0P_ADTIM4, &stcAdtIrqTrigCfg);          //??????????????????ADC??????
}


///< ADC?????????
void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;
    stc_adc_sqr_cfg_t          stcAdcSqrCfg;
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcSqrCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);
    
    //ADC??????
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;  //BGR????????????
    M0P_BGR->CR_f.TS_EN  = 0x0u;
    delay100us(10);
    
    stcAdcCfg.enAdcMode         = AdcScanMode;          //??????????????????
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv2;        //Adc???????????? PCLK/2
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle8Clk;  //???????????? 8?????????
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelAVDD;  //??????AVDD
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;     //???????????????????????????
    stcAdcCfg.enInRef           = AdcMskInRefDisable;   //??????????????????Disable
    
    Adc_Init(&stcAdcCfg);                               //Adc?????????
    
    Adc_CfgSqrChannel(AdcSQRCH0MUX, AdcExInputCH0);     //??????????????????????????????
    Adc_CfgSqrChannel(AdcSQRCH1MUX, AdcExInputCH2);     //????????????CH1 --> CH0
    
    stcAdcSqrCfg.bSqrDmaTrig = FALSE;
    stcAdcSqrCfg.enResultAcc = AdcResultAccDisable;
    stcAdcSqrCfg.u8SqrCnt = 2;                       //??????????????????(2-1????????????????????????)
    Adc_SqrModeCfg(&stcAdcSqrCfg);                   //??????????????????????????????
    
    Adc_SqrExtTrigCfg(AdcMskTrigTimer4, TRUE);          //Timer4????????????????????????
    
    Adc_ClrIrqStatus(AdcMskIrqSqr);
    Adc_EnableIrq();                                    //??????Adc??????
    EnableNvic(ADC_IRQn, IrqLevel1, TRUE);          //Adc?????????
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
    App_ClockInit();          //???????????????
    

    App_AdvTimerInit(48000);  //AdvTimer?????????, ??????1K: 48M * 1000us = 48000
    
    App_AdcInit();            //ADC?????????
    
    Adt_StartCount(M0P_ADTIM4);   //timer4??????

    while(1)
    {
        //????????????????????????PWM???????????? GCMCR?????????
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


