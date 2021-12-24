#include "bsp_uv.h"


void bsp_UvInit (void)
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
    UV_DISABLE;
    
    ///< GPIO IO LED端口初始化
    Gpio_Init(UV_EN_PORT, UV_EN_PIN, &stcGpioCfg);
}