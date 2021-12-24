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
#include "vc.h"
#include "gpio.h"
#include "adc.h"
#include "bgr.h"

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
static void App_GpioInit(void);
static void App_VC0Init(void);

/**
 ******************************************************************************
 ** \brief  初始化TIM2
 **
 ** \return 无
 ******************************************************************************/
int main(void)
{  
    ///< GPIO 初始化
    App_GpioInit();
    ///< VC0 初始化
    App_VC0Init();
    
    ///< VC0 使能
    Vc_Cmd(VcChannel0, TRUE);
    
    while (1)
    {
        ;
    }
}

 /**
 ******************************************************************************
 ** \brief  中断向量入口函数
 **
 ** \return 无
 ******************************************************************************/
 void Vc0_IRQHandler(void)
{
    Vc_ClearItStatus(Vc0_Intf);
    
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);
    delay1ms(1000);
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);
    delay1ms(1000);
}
/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return 无
 ******************************************************************************/
static void App_GpioInit(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);     //开GPIO时钟
    
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);              //PA06作为VC0_OUT

    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &GpioInitStruct);   //PD14配置成输出，控制板上蓝色LED
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);    
    
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);                  //VC0的P端输入
}

/**
 ******************************************************************************
 ** \brief  初始化VC0
 **
 ** \return 无
 ******************************************************************************/
static void App_VC0Init(void)
{
    stc_vc_channel_cfg_t VcInitStruct;
    DDL_ZERO_STRUCT(VcInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);    //开VC时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);   //开BGR时钟
    
    Bgr_BgrEnable();                                            //BGR必须使能
    
    VcInitStruct.enVcChannel = VcChannel0;
    VcInitStruct.enVcCmpDly  = VcDelay10mv;         //VC0迟滞电压约为10mV
    VcInitStruct.enVcBiasCurrent = VcBias10ua;      //VC0功耗为10uA
    VcInitStruct.enVcFilterTime  = VcFilter28us;    //VC输出滤波时间约为28us
    VcInitStruct.enVcInPin_P     = VcInPCh0;        //VC0_CH的P端连接PC00
    VcInitStruct.enVcInPin_N     = AiBg1p2;         //VC0_CH的N端连接内核1.2V
    VcInitStruct.enVcOutCfg      = VcOutDisable;    //不作为内部触发输出
    VcInitStruct.bFlten          = TRUE;            //使能滤波
    Vc_Init(&VcInitStruct);
    
    
    Vc_CfgItType(VcChannel0, VcIrqRise);            //配置VC0为上升沿中断
    Vc_ClearItStatus(Vc0_Intf);                     //清除中断标志位
    Vc_ItCfg(VcChannel0, TRUE);                     //使能中断
    EnableNvic(VC0_IRQn, IrqLevel3, TRUE);          //使能中断向量
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

