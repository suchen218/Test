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
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "rtc.h"
#include "lpm.h"
#include "gpio.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
__IO uint8_t flag;
__IO uint8_t second, minute, hour, week, day, month, year;

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void App_GpioCfg(void);
void App_RtcCfg(void);

/**
******************************************************************************
    ** \brief  ?????????
    ** 
  ** @param  ???
    ** \retval ???
    **
******************************************************************************/ 
int32_t main(void)
{
    Sysctrl_ClkSourceEnable(SysctrlClkXTL,TRUE);     //??????XTL??????RTC??????
    
    App_GpioCfg();                                   //??????GPIO???????????????????????????????????????
    
    App_RtcCfg();                                    //??????RTC        
        
    //while(Gpio_GetInputIO(GpioPortA,GpioPin7) == 1); //??????????????????,?????????????????????????????????????????????????????????????????????????????????????????????
    
    Lpm_GotoDeepSleep(TRUE);                         //?????????????????????
    
    while (1)
    {
        ;
    }
}

 /**
******************************************************************************
    ** \brief  RTC??????????????????
    ** 
  ** @param  ???
    ** \retval ???
    **
******************************************************************************/  
void Rtc_IRQHandler(void)
{
    if(Rtc_GetPridItStatus() == TRUE)
    {
        flag = 1;
        Rtc_ClearPrdfItStatus();             //?????????????????????
        if(TRUE == Gpio_GetInputIO(STK_LED_PORT, STK_LED_PIN))
        {
            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);    //??????????????????LED
            Gpio_WriteOutputIO(GpioPortA, GpioPin5, FALSE);          //PA05?????????
        }            
        else
        {
            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);     //??????????????????LED
            Gpio_WriteOutputIO(GpioPortA, GpioPin5, TRUE);           //PA05?????????
        }        
    }
}

/**
 ******************************************************************************
 ** \brief  ???????????????GPIO??????
 **
 ** \return ???
 ******************************************************************************/
void App_GpioCfg(void)
{
    stc_gpio_cfg_t         GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);  //GPIO??????????????????
    
    //PD14 ??????LED?????????
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);   //??????LED?????????????????????
    //PA05 ?????????????????????????????????????????????LED??????????????????
    Gpio_Init(GpioPortA, GpioPin5, &GpioInitStruct);
    //PA07 ??????????????????   
    GpioInitStruct.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin7, &GpioInitStruct);   
    
    while(Gpio_GetInputIO(GpioPortA,GpioPin7) == 1); //??????????????????
    
    //??????LED ???PA05???????????????
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE); 
    Gpio_WriteOutputIO(GpioPortA, GpioPin5, TRUE);   
    
}

/**
******************************************************************************
    ** \brief  ??????RTC
    ** 
  ** @param  ???
    ** \retval ???
    **
******************************************************************************/ 
void App_RtcCfg(void)
{
    stc_rtc_initstruct_t RtcInitStruct;
    Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);   //RTC??????????????????
    RtcInitStruct.rtcAmpm = RtcPm;                          //12?????????
    RtcInitStruct.rtcClksrc = RtcClkXtl;                    //??????????????????
    RtcInitStruct.rtcPrdsel.rtcPrdsel = RtcPrdx;            //??????????????????PRDX
    RtcInitStruct.rtcPrdsel.rtcPrdx = 9;                    //???????????????????????? 5???
    RtcInitStruct.rtcTime.u8Second = 0x55;
    RtcInitStruct.rtcTime.u8Minute = 0x01;
    RtcInitStruct.rtcTime.u8Hour   = 0x10;
    RtcInitStruct.rtcTime.u8Day    = 0x17;
    RtcInitStruct.rtcTime.u8DayOfWeek = 0x04;
    RtcInitStruct.rtcTime.u8Month  = 0x04;
    RtcInitStruct.rtcTime.u8Year   = 0x19;
    RtcInitStruct.rtcCompen = RtcCompenEnable;
    RtcInitStruct.rtcCompValue = 0;                         //????????? ??????????????????????????????
    Rtc_Init(&RtcInitStruct);
    Rtc_AlmIeCmd(TRUE);                                     //??????????????????
    EnableNvic(RTC_IRQn, IrqLevel3, TRUE);                  //??????RTC????????????
    Rtc_Cmd(TRUE);                                          //??????RTC????????????
    Rtc_StartWait();                                        //??????RTC??????????????????????????????????????????????????????????????????    
}



/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


