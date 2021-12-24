#include "bsp_air.h"


void bsp_AirInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    stcGpioCfg.enDir = GpioDirOut;
    ///< �˿�����������->����
//    stcGpioCfg.enPu = GpioPuDisable;
//    stcGpioCfg.enPd = GpioPdDisable;
    
    ///< LED�ر�
//    Gpio_SetIO(UV_EN_PORT, UV_EN_PIN);
    AIR_11_DISABLE;
    AIR_12_DISABLE;
    AIR_21_DISABLE;
    AIR_22_DISABLE;
    AIR_31_DISABLE;
    AIR_32_DISABLE;
    Gpio_SetAfMode (AIR_32_PORT, AIR_32_PIN, GpioAf0);
    ///< GPIO IO LED�˿ڳ�ʼ��
    Gpio_Init(AIR_11_PORT, AIR_11_PIN, &stcGpioCfg);
    Gpio_Init(AIR_12_PORT, AIR_12_PIN, &stcGpioCfg);
    Gpio_Init(AIR_21_PORT, AIR_21_PIN, &stcGpioCfg);
    Gpio_Init(AIR_22_PORT, AIR_22_PIN, &stcGpioCfg);
    Gpio_Init(AIR_31_PORT, AIR_31_PIN, &stcGpioCfg);
    Gpio_Init(AIR_32_PORT, AIR_32_PIN, &stcGpioCfg);
}
