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
 **   - 2019-04-23  1.0   First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "spi.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
__IO uint8_t tx_cnt, rx_cnt;
uint8_t tx_buf[10]={1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, rx_buf[10];
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_GpioInit(void);
static void App_SPIInit(void);



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
    uint16_t tmp;
    volatile uint8_t tmp1;
    tx_cnt = 0;
    tmp = 0;

    ///< ???????????????
    App_GpioInit();
    ///< SPI?????????
    App_SPIInit();

    ///< USER ??????????????????
    while(TRUE == Gpio_GetInputIO(STK_USER_PORT, STK_USER_PIN));

    ///< ?????????????????????
    Spi_SetCS(M0P_SPI0, FALSE);

    ///< ???????????????????????????
    while(tx_cnt<10)                                        //???????????????????????????
    {
        while(Spi_GetStatus(M0P_SPI0, SpiTxe) == FALSE);    //????????????????????????
        Spi_SendData(M0P_SPI0, tx_buf[tx_cnt++]);
    }

    ///< ????????????????????????
    while(rx_cnt<10)                                        //?????????????????????
    {
        while(Spi_GetStatus(M0P_SPI0, SpiTxe) == FALSE);    //????????????????????????
        Spi_SendData(M0P_SPI0, 0);                          //???????????????
        delay10us(20);
        while(Spi_GetStatus(M0P_SPI0, SpiRxne) == FALSE);
        rx_buf[rx_cnt++] = Spi_ReceiveData(M0P_SPI0);
    }

    ///< ????????????
    Spi_SetCS(M0P_SPI0, TRUE);

    ///< ???????????????????????????????????????????????????
    for(tmp = 0; tmp<10; )
    {
        if(rx_buf[tmp] == tx_buf[tmp])
            tmp++;
        else
            break;
    }
    if(tmp == 10)                                    //???????????????????????????????????????????????????????????????LED
        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);

    while(1);
}


/**
 ******************************************************************************
 ** \brief  ???????????????GPIO??????
 **
 ** \return ???
 ******************************************************************************/
static void App_GpioInit(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);

    //SPI0????????????:??????
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;

    Gpio_Init(GpioPortA, GpioPin7,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin7,GpioAf1);        //????????????PA7??????SPI0_MOSI

    Gpio_Init(GpioPortA, GpioPin4,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin4,GpioAf1);         //????????????PA04??????SPI0_CS

    Gpio_Init(GpioPortA, GpioPin5,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin5,GpioAf1);         //????????????PA05??????SPI0_SCK

    GpioInitStruct.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin6,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin6,GpioAf1);         //????????????PA06??????SPI0_MISO


    ///< ??????????????????->??????
    GpioInitStruct.enDir = GpioDirIn;
    ///< ????????????????????????->???????????????
    GpioInitStruct.enDrv = GpioDrvL;
    ///< ?????????????????????->???
    GpioInitStruct.enPu = GpioPuDisable;
    GpioInitStruct.enPd = GpioPdDisable;
    ///< ????????????????????????->??????????????????
    GpioInitStruct.enOD = GpioOdDisable;
    ///< ????????????/??????????????????????????????????????????->AHB
    GpioInitStruct.enCtrlMode = GpioAHB;
    ///< GPIO IO USER KEY?????????
    Gpio_Init(STK_USER_PORT, STK_USER_PIN, &GpioInitStruct);


    //PD14:??????LED
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);     //??????????????????LED
}

/**
 ******************************************************************************
 ** \brief  ?????????SPI
 **
 ** \return ???
 ******************************************************************************/
static void App_SPIInit(void)
{
    stc_spi_cfg_t  SpiInitStruct;

    Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE);

    //SPI0?????????????????????
    SpiInitStruct.enSpiMode = SpiMskMaster;   //?????????????????????
    SpiInitStruct.enPclkDiv = SpiClkMskDiv128;  //????????????fsys/128
    SpiInitStruct.enCPHA    = SpiMskCphafirst;//??????????????????
    SpiInitStruct.enCPOL    = SpiMskcpollow;  //????????????
    Spi_Init(M0P_SPI0, &SpiInitStruct);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


