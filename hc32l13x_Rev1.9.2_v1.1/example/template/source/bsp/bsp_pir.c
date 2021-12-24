#include "bsp_pir.h"

void bsp_PirInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
    stcGpioCfg.enDir = GpioDirIn;
    ///< 端口上下拉配置->下拉

    ///< GPIO IO LED端口初始化
    Gpio_Init(PIR_LIQUID_PORT, PIR_LIQUID_PIN, &stcGpioCfg);
    
    stcGpioCfg.enPd = GpioPdEnable;
    Gpio_Init(PIR_EN_PORT, PIR_EN_PIN, &stcGpioCfg);
}