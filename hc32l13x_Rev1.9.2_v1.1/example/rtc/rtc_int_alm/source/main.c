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
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
__IO uint8_t toggleCnt;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
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
    //??????GPIO
    App_GpioCfg();

    //??????RTC
    App_RtcCfg();

    while (1)
    {
        if(toggleCnt > 0)          // toggleCnt ?????????????????????
        {
            toggleCnt--;

            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);     //??????????????????LED
            delay1ms(500);

            Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);    //??????????????????LED
            delay1ms(500);
        }
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
    if (Rtc_GetAlmfItStatus() == TRUE) //????????????
    {
        toggleCnt = 10;                //??????mian????????????????????????LED 10???
        Rtc_ClearAlmfItStatus();       //??????????????????
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

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);   //GPIO??????????????????

    //LED???????????????
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);     //????????????????????????LED
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
    stc_rtc_alarmtime_t RtcAlmStruct;

    DDL_ZERO_STRUCT(RtcInitStruct);                      //?????????????????????
    DDL_ZERO_STRUCT(RtcAlmStruct);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);//RTC??????????????????

    Sysctrl_ClkSourceEnable(SysctrlClkXTL, TRUE);        //????????????XTL????????????RTC??????

    RtcInitStruct.rtcAmpm = RtcPm;                       //12?????????
    RtcInitStruct.rtcClksrc = RtcClkXtl;                 //??????????????????
    RtcInitStruct.rtcPrdsel.rtcPrdsel = RtcPrds;         //??????????????????PRDS
    RtcInitStruct.rtcPrdsel.rtcPrds = RtcNone;           //?????????????????????
    RtcInitStruct.rtcTime.u8Second = 0x55;               //??????RTC??????2019???4???17???10:01:55
    RtcInitStruct.rtcTime.u8Minute = 0x01;
    RtcInitStruct.rtcTime.u8Hour   = 0x10;
    RtcInitStruct.rtcTime.u8Day    = 0x17;
    RtcInitStruct.rtcTime.u8DayOfWeek = 0x04;
    RtcInitStruct.rtcTime.u8Month  = 0x04;
    RtcInitStruct.rtcTime.u8Year   = 0x19;
    RtcInitStruct.rtcCompen = RtcCompenEnable;           // ????????????????????????
    RtcInitStruct.rtcCompValue = 0;                      //?????????  ??????????????????????????????
    Rtc_Init(&RtcInitStruct);

    RtcAlmStruct.RtcAlarmMinute = 0x02;
    RtcAlmStruct.RtcAlarmHour = 0x10;
    RtcAlmStruct.RtcAlarmWeek = 0x7f;                    //???????????????????????????10:02:05??????????????????
    Rtc_SetAlarmTime(&RtcAlmStruct);                     //??????????????????
    Rtc_AlmIeCmd(TRUE);                                  //??????????????????

    EnableNvic(RTC_IRQn, IrqLevel3, TRUE);               //??????RTC????????????
    Rtc_Cmd(TRUE);                                       //??????RTC????????????
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


