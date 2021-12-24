#include "bsp_air.h"


void bsp_AirInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口上下拉配置->下拉
//    stcGpioCfg.enPu = GpioPuDisable;
//    stcGpioCfg.enPd = GpioPdDisable;
    
    ///< LED关闭
//    Gpio_SetIO(UV_EN_PORT, UV_EN_PIN);
    AIR_11_DISABLE;
    AIR_12_DISABLE;
    AIR_21_DISABLE;
    AIR_22_DISABLE;
    AIR_31_DISABLE;
    AIR_32_DISABLE;
    Gpio_SetAfMode (AIR_32_PORT, AIR_32_PIN, GpioAf0);
    ///< GPIO IO LED端口初始化
    Gpio_Init(AIR_11_PORT, AIR_11_PIN, &stcGpioCfg);
    Gpio_Init(AIR_12_PORT, AIR_12_PIN, &stcGpioCfg);
    Gpio_Init(AIR_21_PORT, AIR_21_PIN, &stcGpioCfg);
    Gpio_Init(AIR_22_PORT, AIR_22_PIN, &stcGpioCfg);
    Gpio_Init(AIR_31_PORT, AIR_31_PIN, &stcGpioCfg);
    Gpio_Init(AIR_32_PORT, AIR_32_PIN, &stcGpioCfg);
}
