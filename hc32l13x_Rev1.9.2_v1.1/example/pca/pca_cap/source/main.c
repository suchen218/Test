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
#include "pca.h"
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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32PcaTestFlag = 0;
static volatile uint16_t u16CcapData[8] = {0};

__IO uint8_t rising_flag;
__IO uint8_t falling_flag;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void App_GpioInit(void); 
static void App_PcaInit(void);

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/


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
    App_GpioInit();
    App_PcaInit();
    
    Pca_StartPca(TRUE);
    
    while (1)
    {
        
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
void Pca_IRQHandler(void)
{
    if(Pca_GetItStatus(PcaCcf1) != FALSE)
    {
        Pca_ClrItStatus(PcaCcf1);
        if(GetBit(M0P_GPIO->PAIN, GpioPin6) != FALSE)
        {
            rising_flag = 1;
            falling_flag = 0;
        }
        else
        {
            rising_flag = 0;
            falling_flag = 1;
        }
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

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    //PA07?????????PCA_CH1
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin7, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf2);

    //PA06?????????PCA_CH0
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin6, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin6, GpioAf2);
}

/**
 ******************************************************************************
 ** \brief  ??????PCA
 **
 ** \return ???
 ******************************************************************************/
static void App_PcaInit(void)
{
    stc_pcacfg_t  PcaInitStruct;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);    
    
    PcaInitStruct.pca_clksrc = PcaPclkdiv32;
    PcaInitStruct.pca_cidl   = FALSE;
    PcaInitStruct.pca_ecom   = PcaEcomDisable;       //?????????????????????
    PcaInitStruct.pca_capp   = PcaCappEnable;        //?????????????????????
    PcaInitStruct.pca_capn   = PcaCapnEnable;        //?????????????????????
    PcaInitStruct.pca_mat    = PcaMatDisable;        //??????????????????
    PcaInitStruct.pca_tog    = PcaTogDisable;        //????????????????????????
    PcaInitStruct.pca_pwm    = PcaPwm8bitDisable;    //??????PWM????????????
    PcaInitStruct.pca_epwm   = PcaEpwmDisable;       //??????16bitPWM??????
    Pca_M1Init(&PcaInitStruct);    
    
    PcaInitStruct.pca_cidl   = FALSE;
    PcaInitStruct.pca_ecom   = PcaEcomEnable;       //?????????????????????
    PcaInitStruct.pca_capp   = PcaCappDisable;      //?????????????????????
    PcaInitStruct.pca_capn   = PcaCapnDisable;      //?????????????????????
    PcaInitStruct.pca_pwm    = PcaPwm8bitEnable;    //??????PWM????????????
    PcaInitStruct.pca_ccapl  = 120;
    PcaInitStruct.pca_ccaph  = 120;
    Pca_M0Init(&PcaInitStruct);   

    Pca_ClrItStatus(PcaCcf1);
    Pca_ConfModulexIt(PcaModule1, TRUE);
    EnableNvic(PCA_IRQn, IrqLevel3, TRUE);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


