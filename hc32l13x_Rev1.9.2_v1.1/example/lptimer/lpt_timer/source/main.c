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
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "lptim.h"
#include "lpm.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

#define LptGate_PORT  GpioPortB
#define LptGate_PIN   GpioPin3

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
__IO uint8_t ItFlag;
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32LptTestFlag = 0;


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

static void App_SysClkInit(void);
static void App_GPIOInit(void);
static void App_LPTimerInit(void);
static void App_LowPowerModeGpioSet(void);


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
    ///< ?????????????????????
    App_SysClkInit();
    ///< GPIO?????????
    App_GPIOInit();
    ///< LPTimer?????????
    App_LPTimerInit();

    while (1 == Gpio_GetInputIO(STK_USER_PORT,STK_USER_PIN));

    Lptim_Cmd(M0P_LPTIMER, TRUE);    //LPT ??????

    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);     //??????????????????LED
    ///< ????????????GPIO??????
    App_LowPowerModeGpioSet();
    Lpm_GotoDeepSleep(TRUE);

    while (1)
    {
        if(ItFlag == 1)
        {
            ItFlag = 0;
        }
    }
}

/**
 ******************************************************************************
 ** \brief  LPTIMER??????????????????
 **
 ** \return ???
 ******************************************************************************/
void LpTim_IRQHandler(void)
{
    if (TRUE == Lptim_GetItStatus(M0P_LPTIMER))
    {
        ItFlag = 1;
        Lptim_ClrItStatus(M0P_LPTIMER);//??????LPTimer??????????????????

        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);     //??????????????????LED
        delay1ms(500);
        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);     //??????????????????LED
    }
}

static void App_SysClkInit(void)
{
    stc_sysctrl_clk_cfg_t  stcClkCfg;

    //CLK INIT
    stcClkCfg.enClkSrc  = SysctrlClkRCH;
    stcClkCfg.enHClkDiv = SysctrlHclkDiv1;
    stcClkCfg.enPClkDiv = SysctrlPclkDiv1;
    Sysctrl_ClkInit(&stcClkCfg);

    //??????RCL
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
}

/**
 ******************************************************************************
 ** \brief  ???????????????GPIO??????
 **
 ** \return ???
 ******************************************************************************/
static void App_GPIOInit(void)
{
    stc_gpio_cfg_t         GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);

    //??????GPIO????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

    //PB03?????????LP Timer GATE ??????
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirIn;
    Gpio_Init(LptGate_PORT, LptGate_PIN, &GpioInitStruct);
    Gpio_SetAfMode(LptGate_PORT,LptGate_PIN,GpioAf5);

    //LED
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);

    //KEY
    GpioInitStruct.enDir  = GpioDirIn;
    Gpio_Init(STK_USER_PORT, STK_USER_PIN, &GpioInitStruct);

    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);     //??????????????????LED
}


/**
 ******************************************************************************
 ** \brief  ?????????LPTIMER
 **
 ** \return ???
 ******************************************************************************/
static void App_LPTimerInit(void)
{
    stc_lptim_cfg_t    stcLptCfg;
    DDL_ZERO_STRUCT(stcLptCfg);

    ///< ??????LPTIM0 ????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim, TRUE);

    stcLptCfg.enGate   = LptimGateHigh;
    stcLptCfg.enGatep  = LptimGatePLow;
    stcLptCfg.enTcksel = LptimRcl;
    stcLptCfg.enTogen  = LptimTogEnLow;
    stcLptCfg.enCt     = LptimTimerFun;         //???????????????
    stcLptCfg.enMd     = LptimMode1;            //?????????????????????1?????????????????????16????????????/?????????
    stcLptCfg.u16Arr   = 0;                     //?????????????????????
    Lptim_Init(M0P_LPTIMER, &stcLptCfg);

    Lptim_ClrItStatus(M0P_LPTIMER);   //?????????????????????
    Lptim_ConfIt(M0P_LPTIMER, TRUE);  //??????LPTIMER??????
    EnableNvic(LPTIM_IRQn, IrqLevel3, TRUE);
}


static void App_LowPowerModeGpioSet(void)
{
    ///< ??????GPIO??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

    //swd as gpio
    Sysctrl_SetFunc(SysctrlSWDUseIOEn, TRUE);

    ///< ?????????????????????
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PCADS = 0;
    M0P_GPIO->PDADS = 0;

    ///< ???????????????????????????LED????????????
    M0P_GPIO->PADIR = 0XFFFF;
    M0P_GPIO->PBDIR = 0XFFFF;
    M0P_GPIO->PCDIR = 0XFFFF;
    M0P_GPIO->PDDIR = 0XFFDF;

    ///< ??????????????????KEY???????????????
    M0P_GPIO->PAPD = 0xFFFF;
    M0P_GPIO->PBPD = 0xFFFF;
    M0P_GPIO->PCPD = 0xFFFF;
    M0P_GPIO->PDPD = 0xFFEF;

}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


