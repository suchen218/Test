#include "bsp_pir.h"

void bsp_PirInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    stcGpioCfg.enDir = GpioDirIn;
    ///< �˿�����������->����

    ///< GPIO IO LED�˿ڳ�ʼ��
    Gpio_Init(PIR_LIQUID_PORT, PIR_LIQUID_PIN, &stcGpioCfg);
    
    stcGpioCfg.enPd = GpioPdEnable;
    Gpio_Init(PIR_EN_PORT, PIR_EN_PIN, &stcGpioCfg);
}