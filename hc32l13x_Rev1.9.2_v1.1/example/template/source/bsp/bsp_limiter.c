#include "bsp_limiter.h"

#if 1//CONFIG_RTT_SPI_ENABLED
#define LIMITER_Print(M, ...)	LOG_Print("LIMITER", M, ##__VA_ARGS__)
#else
#define LIMITER_Print(M, ...)
#endif

// ��λ������λ��־
volatile uint8_t     g_u8LimiterFlag;

///< PortA�жϷ�����
void PortA_IRQHandler(void)
{
    if (M0P_GPIO->PA_STAT & (1 << 15))
    {
        M0P_GPIO->PA_ICLR &= ~(1 << 15);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_SWEEP_RIGHT_IN)
        {
            g_u8LimiterFlag |= LIMITER_FLAG_SWEEP_RIGHT_IN;
        }
    }
}

void PortB_IRQHandler(void)
{
    if (M0P_GPIO->PB_STAT & (1 << 3))
    {
        M0P_GPIO->PB_ICLR &= ~(1 << 3);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_VERTICAL_UP)
        {
            g_u8LimiterFlag |= LIMITER_FLAG_VERTICAL_UP;
        }
    }
}

void PortC_IRQHandler(void)
{
    if (M0P_GPIO->PC_STAT & (1 << 10))
    {
        M0P_GPIO->PC_ICLR &= ~(1 << 10);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_SWEEP_RIGHT_OUT)
        {
            g_u8LimiterFlag |= LIMITER_FLAG_SWEEP_RIGHT_OUT;
        }
    }
    
    if (M0P_GPIO->PC_STAT & (1 << 11))
    {
        M0P_GPIO->PC_ICLR &= ~(1 << 11);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_HORIZONTAL_IN)
        {
            g_u8LimiterFlag |= LIMITER_FLAG_HORIZONTAL_IN;
        }
    }
    
    if (M0P_GPIO->PC_STAT & (1 << 12))
    {
        M0P_GPIO->PC_ICLR &= ~(1 << 12);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_HORIZONTAL_OUT)
        {
            g_u8LimiterFlag |= LIMITER_FLAG_HORIZONTAL_OUT;
        }
    }
}

void PortD_IRQHandler(void)
{
    if (M0P_GPIO->PD_STAT & (1 << 2))
    {
        M0P_GPIO->PD_ICLR &= ~(1 << 2);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_VERTICAL_DOWN)
        {
            g_u8LimiterFlag |= LIMITER_FLAG_VERTICAL_DOWN;
        }
    }
    
    if (M0P_GPIO->PD_STAT & (1 << 6))
    {
        M0P_GPIO->PD_ICLR &= ~(1 << 6);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_SWEEP_LEFT_IN)
        {
            g_u8LimiterFlag |= LIMITER_FLAG_SWEEP_LEFT_IN;
        }
    }
    
    if (M0P_GPIO->PD_STAT & (1 << 7))
    {
        M0P_GPIO->PD_ICLR &= ~(1 << 7);
        
        if (g_u8MotorMoveFlag & LIMITER_FLAG_SWEEP_LEFT_OUT)
        {
            CTRL_MOTOR_SWEEP1_STOP;
            g_u8LimiterFlag |= LIMITER_FLAG_SWEEP_LEFT_OUT;
        }
    }
}

void bsp_LimiterInit (void)
{
    stc_gpio_cfg_t stcGpioCfg = {0};
    
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    stcGpioCfg.enDir = GpioDirIn;
    ///< �˿�����������->����
//    stcGpioCfg.enPu = GpioPuEnable;
    
    ///< GPIO IO LED�˿ڳ�ʼ��   �ڵ�Ϊ�͵�ƽ
    Gpio_Init(LIMITER_1_PORT, LIMITER_1_PIN, &stcGpioCfg);  // �������    �������⿴
    Gpio_Init(LIMITER_2_PORT, LIMITER_2_PIN, &stcGpioCfg);  // �������
    Gpio_Init(LIMITER_3_PORT, LIMITER_3_PIN, &stcGpioCfg);  // �Ҳ�����
    Gpio_Init(LIMITER_4_PORT, LIMITER_4_PIN, &stcGpioCfg);  // �Ҳ�����
    Gpio_Init(LIMITER_5_PORT, LIMITER_5_PIN, &stcGpioCfg);  // ˮƽ����
    Gpio_Init(LIMITER_6_PORT, LIMITER_6_PIN, &stcGpioCfg);  // ˮƽ����
    Gpio_Init(LIMITER_7_PORT, LIMITER_7_PIN, &stcGpioCfg);  // ��ֱ��
    Gpio_Init(LIMITER_8_PORT, LIMITER_8_PIN, &stcGpioCfg);  // ��ֱ��
    Gpio_Init(LIMITER_9_PORT, LIMITER_9_PIN, &stcGpioCfg);
    Gpio_Init(LIMITER_10_PORT, LIMITER_10_PIN, &stcGpioCfg);
    
    Gpio_EnableIrq(LIMITER_1_PORT, LIMITER_1_PIN, GpioIrqFalling);
    Gpio_EnableIrq(LIMITER_2_PORT, LIMITER_2_PIN, GpioIrqFalling);
    Gpio_EnableIrq(LIMITER_3_PORT, LIMITER_3_PIN, GpioIrqFalling);
    Gpio_EnableIrq(LIMITER_4_PORT, LIMITER_4_PIN, GpioIrqFalling);
    Gpio_EnableIrq(LIMITER_5_PORT, LIMITER_5_PIN, GpioIrqFalling);
    Gpio_EnableIrq(LIMITER_6_PORT, LIMITER_6_PIN, GpioIrqFalling);
    Gpio_EnableIrq(LIMITER_7_PORT, LIMITER_7_PIN, GpioIrqFalling);
    Gpio_EnableIrq(LIMITER_8_PORT, LIMITER_8_PIN, GpioIrqFalling);
    
    M0P_GPIO->PA_ICLR = 0;
    M0P_GPIO->PB_ICLR = 0;
    M0P_GPIO->PC_ICLR = 0;
    M0P_GPIO->PD_ICLR = 0;
    
    EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);
    EnableNvic(PORTB_IRQn, IrqLevel3, TRUE);
    EnableNvic(PORTC_IRQn, IrqLevel3, TRUE);
    EnableNvic(PORTD_IRQn, IrqLevel3, TRUE);
}

typedef struct
{
    en_gpio_port_t  enPort;
    en_gpio_pin_t   enPin;
} gpio_r_st;

gpio_r_st    g_stGpio_r[10] = {
    {LIMITER_1_PORT, LIMITER_1_PIN}  ,  // �������
    {LIMITER_2_PORT, LIMITER_2_PIN}  ,  // ������� 
    {LIMITER_3_PORT, LIMITER_3_PIN}  ,  // �Ҳ�����
    {LIMITER_4_PORT, LIMITER_4_PIN}  ,  // �Ҳ�����
    {LIMITER_5_PORT, LIMITER_5_PIN}  ,  // ˮƽ����
    {LIMITER_6_PORT, LIMITER_6_PIN}  ,  // ˮƽ����
    {LIMITER_7_PORT, LIMITER_7_PIN}  ,  // ��ֱ��
    {LIMITER_8_PORT, LIMITER_8_PIN}  ,  // ��ֱ��
    {LIMITER_9_PORT, LIMITER_9_PIN}  , 
    {LIMITER_10_PORT, LIMITER_10_PIN},
//    {PIR_LIQUID_PORT, PIR_LIQUID_PIN},
//        {PIR_EN_PORT, PIR_EN_PIN},
};

    
void bsp_LimiterTest (void)
{
//    uint8_t     i;
    
//    for (i = 0; i < 10; i++)
//    {
//        if (Gpio_GetInputIO (g_stGpio_r[i].enPort, g_stGpio_r[i].enPin))
//        {
//            LIMITER_Print ("L%d_1", i);
//        }
//        else
//        {
//           LIMITER_Print ("L%d_0", i); 
//        }
//    }
}

