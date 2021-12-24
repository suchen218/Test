#include "bsp_water.h"

void bsp_WaterInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口上下拉配置->下拉

    
    ///< LED关闭
//    Gpio_SetIO(UV_EN_PORT, UV_EN_PIN);
    WATER_1_DISABLE;
    WATER_2_DISABLE;
    WATER_3_DISABLE;
    WATER_4_DISABLE;
    WATER_5_DISABLE;
    WATER_6_DISABLE;
    WATER_7_DISABLE;
    
//    Gpio_SetAfMode (AIR_32_PORT, AIR_32_PIN, GpioAf0);
    ///< GPIO IO LED端口初始化
    Gpio_Init(WATER_1_PORT, WATER_1_PIN, &stcGpioCfg);
    Gpio_Init(WATER_2_PORT, WATER_2_PIN, &stcGpioCfg);
    Gpio_Init(WATER_3_PORT, WATER_3_PIN, &stcGpioCfg);
    Gpio_Init(WATER_4_PORT, WATER_4_PIN, &stcGpioCfg);
    Gpio_Init(WATER_5_PORT, WATER_5_PIN, &stcGpioCfg);
    Gpio_Init(WATER_6_PORT, WATER_6_PIN, &stcGpioCfg);
    Gpio_Init(WATER_7_PORT, WATER_7_PIN, &stcGpioCfg);
}
