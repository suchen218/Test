#include "bsp_uv.h"


void bsp_UvInit (void)
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
    UV_DISABLE;
    
    ///< GPIO IO LED�˿ڳ�ʼ��
    Gpio_Init(UV_EN_PORT, UV_EN_PIN, &stcGpioCfg);
}