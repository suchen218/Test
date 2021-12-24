#include "bsp_water.h"

void bsp_WaterInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    stcGpioCfg.enDir = GpioDirOut;
    ///< �˿�����������->����

    
    ///< LED�ر�
//    Gpio_SetIO(UV_EN_PORT, UV_EN_PIN);
    WATER_1_DISABLE;
    WATER_2_DISABLE;
    WATER_3_DISABLE;
    WATER_4_DISABLE;
    WATER_5_DISABLE;
    WATER_6_DISABLE;
    WATER_7_DISABLE;
    
//    Gpio_SetAfMode (AIR_32_PORT, AIR_32_PIN, GpioAf0);
    ///< GPIO IO LED�˿ڳ�ʼ��
    Gpio_Init(WATER_1_PORT, WATER_1_PIN, &stcGpioCfg);
    Gpio_Init(WATER_2_PORT, WATER_2_PIN, &stcGpioCfg);
    Gpio_Init(WATER_3_PORT, WATER_3_PIN, &stcGpioCfg);
    Gpio_Init(WATER_4_PORT, WATER_4_PIN, &stcGpioCfg);
    Gpio_Init(WATER_5_PORT, WATER_5_PIN, &stcGpioCfg);
    Gpio_Init(WATER_6_PORT, WATER_6_PIN, &stcGpioCfg);
    Gpio_Init(WATER_7_PORT, WATER_7_PIN, &stcGpioCfg);
}
