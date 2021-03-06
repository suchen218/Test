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
#include "lcd.h"
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

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_PortCfg(void);
void App_LcdCfg(void);
/**
 ******************************************************************************
 ** \brief  ?????????
 ** 
 ** @param  ???
 ** \retval ???
 **
 ******************************************************************************/ 
 
 uint8_t    g_u8Minutes;
 uint8_t    g_u8Seconds;
 
 const uint8_t      g_u8caSegCode[] = {0xF5, 0x60, 0xB6, 0xF2, 0x63, 0xD3, 0xD7, 0x70, 0xF7, 0xF3};
 
void Display_Seg (void)
{
    uint8_t     _u8Tmp;
    uint32_t    _32Tmp;
    
    delay1ms (1000);
    
    _u8Tmp = g_u8Minutes / 10;
    _32Tmp = (g_u8caSegCode[_u8Tmp] << 4) | (g_u8caSegCode[_u8Tmp]);
    _u8Tmp = g_u8Minutes % 10;
    _32Tmp |= (g_u8caSegCode[_u8Tmp] << 20) | (g_u8caSegCode[_u8Tmp] << 16) | (1 << 19);
    
    Lcd_WriteRam(0, _32Tmp);
    
    _u8Tmp = g_u8Seconds / 10;
    _32Tmp = (g_u8caSegCode[_u8Tmp] << 4) | (g_u8caSegCode[_u8Tmp]);
    _u8Tmp = g_u8Seconds % 10;
    _32Tmp |= (g_u8caSegCode[_u8Tmp] << 20) | (g_u8caSegCode[_u8Tmp] << 16);
    
    Lcd_WriteRam(1, _32Tmp);
    
    g_u8Seconds++;
    if (g_u8Seconds > 59)
    {
        g_u8Seconds = 0;
        g_u8Minutes++;
        if (g_u8Minutes > 59)
            g_u8Minutes = 0;
    }
}
 
int32_t main(void)
{
    uint8_t     a = 0;
    Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);            ///< ??????RCL??????
    Sysctrl_SetRCLTrim(SysctrlRclFreq32768);                ///< ?????????????????????????????????32.768kHz

    Sysctrl_SetPeripheralGate(SysctrlPeripheralLcd,TRUE);   ///< ??????LCD??????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);  ///< ??????GPIO??????
    
    App_PortCfg();               ///< LCD????????????
    App_LcdCfg();                ///< LCD????????????

    Lcd_ClearDisp();             ///< ??????
    Lcd_WriteRam(0,0x0f0f0f0f);  ///< ???????????????LCDRAM0
    Lcd_WriteRam(1,0x0f0f0f0f);  ///< ???????????????LCDRAM1
    Lcd_WriteRam(2,0x0f0f0f0f);
    while(1)
    {
//        delay1ms (500);
//        M0P_LCD->CR0_f.CONTRAST = a++;
//        a &= 0x0F;
        
        Display_Seg ();
    }
}

/**
 ******************************************************************************
 ** \brief  ???????????????GPIO??????
 **
 ** \return ???
 ******************************************************************************/
void App_PortCfg(void)
{
    Gpio_SetAnalogMode(GpioPortA, GpioPin9);  //COM0
    Gpio_SetAnalogMode(GpioPortA, GpioPin10); //COM1
    Gpio_SetAnalogMode(GpioPortA, GpioPin11); //COM2
    Gpio_SetAnalogMode(GpioPortA, GpioPin12); //COM3   

    Gpio_SetAnalogMode(GpioPortA, GpioPin8);  //SEG0
    Gpio_SetAnalogMode(GpioPortC, GpioPin9);  //SEG1
    Gpio_SetAnalogMode(GpioPortC, GpioPin8);  //SEG2
    Gpio_SetAnalogMode(GpioPortC, GpioPin7);  //SEG3
    Gpio_SetAnalogMode(GpioPortC, GpioPin6);  //SEG4
    Gpio_SetAnalogMode(GpioPortB, GpioPin15); //SEG5
    Gpio_SetAnalogMode(GpioPortB, GpioPin14); //SEG6
    Gpio_SetAnalogMode(GpioPortB, GpioPin13); //SEG7
    Gpio_SetAnalogMode(GpioPortB, GpioPin3);  //VLCDH
    Gpio_SetAnalogMode(GpioPortB, GpioPin4);  //VLCD3
    Gpio_SetAnalogMode(GpioPortB, GpioPin5);  //VLCD2
    Gpio_SetAnalogMode(GpioPortB, GpioPin6);  //VLCD1
    
}

/**
 ******************************************************************************
 ** \brief  ??????LCD
 **
 ** \return ???
 ******************************************************************************/
void App_LcdCfg(void)
{
    stc_lcd_cfg_t LcdInitStruct;
    stc_lcd_segcom_t LcdSegCom;

    LcdSegCom.u32Seg0_31 = 0xffff000;                              ///< ??????LCD_POEN0????????? ??????SEG0~SEG7
    LcdSegCom.stc_seg32_51_com0_8_t.seg32_51_com0_8 = 0xffffffff;   ///< ?????????LCD_POEN1????????? ????????????????????????
    LcdSegCom.stc_seg32_51_com0_8_t.segcom_bit.Com0_3 = 0;          ///< ??????COM0~COM3
    LcdSegCom.stc_seg32_51_com0_8_t.segcom_bit.Mux = 0;             ///< Mux=0,Seg32_35=0,BSEL=1??????:???????????????????????????????????????????????????
    LcdSegCom.stc_seg32_51_com0_8_t.segcom_bit.Seg32_35 = 0;
    Lcd_SetSegCom(&LcdSegCom);                                      ///< LCD COMSEG????????????

    LcdInitStruct.LcdBiasSrc = LcdInResLowPower;                          ///< ?????????????????????????????????????????????
    LcdInitStruct.LcdDuty = LcdDuty4;                              ///< 1/4duty
    LcdInitStruct.LcdBias = LcdBias3;                              ///< 1/3 BIAS
    LcdInitStruct.LcdCpClk = LcdClk2k;                             ///< ???????????????????????????2kHz
    LcdInitStruct.LcdScanClk = LcdClk128hz;                        ///< LCD??????????????????128Hz
    LcdInitStruct.LcdMode = LcdMode0;                              ///< ????????????0
    LcdInitStruct.LcdClkSrc = LcdRCL;                              ///< LCD????????????RCL
    LcdInitStruct.LcdEn   = LcdEnable;                             ///< ??????LCD??????
    
    M0P_LCD->CR0_f.CONTRAST = 0x00;
    Lcd_Init(&LcdInitStruct);
    
    
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


