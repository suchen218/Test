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
 **   - 2017-05-28 LiuHL    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint32_t u32AdcRestult0;
volatile uint32_t u32AdcRestult2;
volatile uint32_t u32AdcRestult5;
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
void App_PortInit(void);
void App_AdcInit(void);
void App_AdcSqrCfg(void);

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
    
    ///< ???????????????
    App_PortInit();
    
    ///< ADC?????? ?????????
    App_AdcInit();
    
    ///< ADC ?????????????????? ??????
    App_AdcSqrCfg();
    
    while(1)
    {        
        ///< ????????????IO??????
        while(FALSE == Adc_GetIrqStatus(AdcMskIrqSqr));
        ///< ???????????????
        u32AdcRestult0   = Adc_GetSqrResult(AdcSQRCH0MUX);
        u32AdcRestult2   = Adc_GetSqrResult(AdcSQRCH1MUX);
        u32AdcRestult5   = Adc_GetSqrResult(AdcSQRCH2MUX);
    }
}

///< ???????????????
void App_PortInit(void)
{    
    stc_gpio_cfg_t          stcGpioCfg;
        
    ///< ??????GPIO????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00 (AIN0)
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02 (AIN2)
    Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05 (AIN5)
    
    ///< GPIO IO USER KEY?????????
    stcGpioCfg.enDir = GpioDirIn;                   ///< GPIO ?????? 
    stcGpioCfg.enDrv = GpioDrvL;                    ///< GPIO???????????????
    stcGpioCfg.enPu = GpioPuDisable;                ///< GPIO?????????
    stcGpioCfg.enPd = GpioPdDisable;                ///< GPIO?????????
    stcGpioCfg.enOD = GpioOdDisable;                ///< GPIO??????????????????
    stcGpioCfg.enCtrlMode = GpioAHB;                ///< AHB ??????????????????
    Gpio_Init(STK_USER_PORT, STK_USER_PIN, &stcGpioCfg);          ///< GPIO ?????????
    Gpio_EnableIrq(STK_USER_PORT, STK_USER_PIN, GpioIrqFalling);  ///< GPIO IO????????????
}

///< ADC?????? ?????????
void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;

    ///< ??????ADC/BGR????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    
    Bgr_BgrEnable();        ///< ??????BGR
    
    ///< ADC ???????????????
    stcAdcCfg.enAdcMode         = AdcScanMode;              ///<????????????-??????
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv1;            ///<????????????-1
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle8Clk;      ///<???????????????-8
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelAVDD;      ///<??????????????????-VCC
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;         ///<OP BUF??????-???
    stcAdcCfg.enInRef           = AdcMskInRefDisable;       ///<????????????????????????-???
    stcAdcCfg.enAdcAlign        = AdcAlignRight;            ///<????????????????????????-???
    Adc_Init(&stcAdcCfg);                                   ///<
}

///< ADC ?????????????????? ??????
void App_AdcSqrCfg(void)
{
    stc_adc_sqr_cfg_t          stcAdcSqrCfg;
    
    ///< ???????????????????????????????????????
    ///< ???????????????????????????????????????????????????n??????????????????????????????????????????[SQRCH(0)MUX,SQRCH(n-1)MUX]
    stcAdcSqrCfg.bSqrDmaTrig = FALSE;
    stcAdcSqrCfg.enResultAcc = AdcResultAccDisable;
    stcAdcSqrCfg.u8SqrCnt    = 3;
    Adc_SqrModeCfg(&stcAdcSqrCfg);
    
    ///< ??????????????????????????????
    Adc_CfgSqrChannel(AdcSQRCH0MUX, AdcExInputCH0);
    Adc_CfgSqrChannel(AdcSQRCH1MUX, AdcExInputCH2);
    Adc_CfgSqrChannel(AdcSQRCH2MUX, AdcExInputCH5);
    
    ///< ??????????????????????????????
    Adc_SqrExtTrigCfg(AdcMskTrigPA07, TRUE);

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


