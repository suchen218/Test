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
 **   - 2017-05-17  1.0  cj First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "uart.h"
#include "gpio.h"
#include "sysctrl.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define     T1_PORT                 (3)
#define     T1_PIN                  (3)

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
volatile static uint8_t u8RxData;
volatile static uint8_t u8TxCnt=0;
volatile static uint8_t u8RxCnt=0;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_UartCfg(void);
void App_PortInit(void);

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
    //??????????????????
    App_PortInit();

    //????????????
    App_UartCfg();

    while(1)
    {
        if(u8RxCnt>=1)
        {
            u8RxCnt = 0;
            Uart_SendDataIt(M0P_UART1, ~u8RxData); //??????UART1?????????????????????
        }

    }
}

//UART1????????????
void Uart1_IRQHandler(void)
{
    if(Uart_GetStatus(M0P_UART1, UartRC))         //UART1????????????
    {
        Uart_ClrStatus(M0P_UART1, UartRC);        //??????????????????
        u8RxData = Uart_ReceiveData(M0P_UART1);   //??????????????????
        u8RxCnt++;
    }

    if(Uart_GetStatus(M0P_UART1, UartTC))         //UART1????????????
    {
        Uart_ClrStatus(M0P_UART1, UartTC);        //??????????????????
        u8TxCnt++;
    }

}

//??????????????????
void App_PortInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;

    DDL_ZERO_STRUCT(stcGpioCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //??????GPIO????????????

    ///<TX
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin2, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortA, GpioPin2, GpioAf1);          //??????PA02 ?????????URART1_TX

    ///<RX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin3, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortA, GpioPin3, GpioAf1);          //??????PA03 ?????????URART1_RX
}

//????????????
void App_UartCfg(void)
{
    stc_uart_cfg_t    stcCfg;

    DDL_ZERO_STRUCT(stcCfg);

    ///< ??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);///<??????uart1????????????

    ///<UART Init
    stcCfg.enRunMode        = UartMskMode3;          ///<??????3
    stcCfg.enStopBit        = UartMsk1bit;           ///<1bit?????????
    stcCfg.enMmdorCk        = UartMskEven;           ///<?????????
    stcCfg.stcBaud.u32Baud  = 9600;                  ///<?????????9600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<????????????????????????
    stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<?????????????????????PCLK????????????
    Uart_Init(M0P_UART1, &stcCfg);                   ///<???????????????

    ///<UART????????????
    Uart_ClrStatus(M0P_UART1,UartRC);                ///<???????????????
    Uart_ClrStatus(M0P_UART1,UartTC);                ///<???????????????
    Uart_EnableIrq(M0P_UART1,UartRxIrq);             ///<????????????????????????
    Uart_EnableIrq(M0P_UART1,UartTxIrq);             ///<????????????????????????
    EnableNvic(UART1_IRQn, IrqLevel3, TRUE);       ///<??????????????????

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


