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
__IO uint8_t rx_buf[10], rx0_buf[10], rx_cnt, tx_cnt;
uint8_t tx_buf[10]={1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
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
    tx_cnt = 0;
    rx_cnt = 0;
    tmp = 0;

    App_GpioInit();
    App_SPIInit();
    
    while(1 == Gpio_GetInputIO(GpioPortB,GpioPin12));//????????????????????????
    
    ///< ????????????????????????
    while(rx_cnt<10)                                 //????????????????????????????????????
    {        
        while(Spi_GetStatus(M0P_SPI1, SpiTxe) == FALSE);
        if (rx_cnt == 9)                            ///< ????????????????????????????????????
        {
            Spi_SendData(M0P_SPI1, rx_buf[0]);        
        }
            
        while(Spi_GetStatus(M0P_SPI1, SpiRxne) == FALSE);//???????????????????????????
        rx_buf[rx_cnt++] = Spi_ReceiveData(M0P_SPI1);  //????????????????????????      
    }    
    
    ///< ????????????????????????
    rx_cnt = 1;
    while(rx_cnt<10)
    {
        while(Spi_GetStatus(M0P_SPI1, SpiTxe) == FALSE)
        {}
        Spi_SendData(M0P_SPI1, rx_buf[rx_cnt++]);
    }
    
    
    for(tmp = 0; tmp<10;)                              //???????????????????????????????????????????????????
    {
        if(rx_buf[tmp] == tx_buf[tmp])             
            tmp++;
        else
            break;
    }
    if(tmp == 10)                                    //?????????????????????????????????????????????????????????????????????LED
        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);    
    
    while(1)
    {
    }
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
    
    //SPI1?????????????????????
    GpioInitStruct.enDir = GpioDirIn;
    Gpio_Init(GpioPortB, GpioPin12, &GpioInitStruct);
    Gpio_SfSsnCfg(GpioSpi1,GpioSfSsnExtClkPB12);      //????????????PB12??????SPI1_CS
    
    Gpio_Init(GpioPortB, GpioPin13,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortB, GpioPin13,GpioAf1);        //????????????PB13??????SPI1_SCK
    
    Gpio_Init(GpioPortB, GpioPin15,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortB, GpioPin15,GpioAf1);        //????????????PB15??????SPI1_MOSI    
    
    GpioInitStruct.enDir = GpioDirOut;
    Gpio_Init(GpioPortB, GpioPin14,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortB, GpioPin14,GpioAf1);        //????????????PB14??????SPI1_MISO    

    //PD14:????????????LED
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);     //??????????????????LED        
}

/**
 ******************************************************************************
 ** \brief  ??????SPI
 **
 ** \return ???
 ******************************************************************************/
static void App_SPIInit(void)
{
    stc_spi_cfg_t  SpiInitStruct;   

    Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi1, TRUE);
    
    //SPI1?????????????????????
    SpiInitStruct.enSpiMode = SpiMskSlave;   //?????????????????????
    SpiInitStruct.enPclkDiv = SpiClkMskDiv128;  //????????????fsys/128
    SpiInitStruct.enCPHA    = SpiMskCphafirst;//??????????????????
    SpiInitStruct.enCPOL    = SpiMskcpollow;  //????????????
    Spi_Init(M0P_SPI1, &SpiInitStruct);    
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


