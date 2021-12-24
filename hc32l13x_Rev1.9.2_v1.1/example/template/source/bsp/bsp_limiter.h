#ifndef __BSP_LIMITER_H__
#define __BSP_LIMITER_H__

#include "bsp_debug.h"

typedef enum
{
    LIMITER_FLAG_NONE,
    LIMITER_FLAG_VERTICAL_UP = 0x01,
    LIMITER_FLAG_VERTICAL_DOWN = 0x02,
    LIMITER_FLAG_HORIZONTAL_OUT = 0x04,
    LIMITER_FLAG_HORIZONTAL_IN = 0x08,
    LIMITER_FLAG_SWEEP_LEFT_OUT = 0x10,
    LIMITER_FLAG_SWEEP_LEFT_IN = 0x20,
    LIMITER_FLAG_SWEEP_RIGHT_OUT = 0x40,
    LIMITER_FLAG_SWEEP_RIGHT_IN = 0x80,
} LIMITER_FLAG_E;

// 限位开关集合，低电平闭合
#define LIMITER_1_PORT      GpioPortD
#define LIMITER_1_PIN       GpioPin6
#define LIMITER_1_READ       (M0P_GPIO->PDIN & (1 << 6)) 

#define LIMITER_2_PORT      GpioPortD
#define LIMITER_2_PIN       GpioPin7
#define LIMITER_2_READ       (M0P_GPIO->PDIN & (1 << 7)) 

#define LIMITER_3_PORT      GpioPortA
#define LIMITER_3_PIN       GpioPin15
#define LIMITER_3_READ       (M0P_GPIO->PAIN & (1 << 15)) 

#define LIMITER_4_PORT      GpioPortC
#define LIMITER_4_PIN       GpioPin10
#define LIMITER_4_READ       (M0P_GPIO->PCIN & (1 << 10)) 

#define LIMITER_5_PORT      GpioPortC
#define LIMITER_5_PIN       GpioPin11
#define LIMITER_5_READ       (M0P_GPIO->PCIN & (1 << 11)) 

#define LIMITER_6_PORT      GpioPortC
#define LIMITER_6_PIN       GpioPin12
#define LIMITER_6_READ       (M0P_GPIO->PCIN & (1 << 12)) 

#define LIMITER_7_PORT      GpioPortD
#define LIMITER_7_PIN       GpioPin2
#define LIMITER_7_READ       (M0P_GPIO->PDIN & (1 << 2)) 

#define LIMITER_8_PORT      GpioPortB
#define LIMITER_8_PIN       GpioPin3
#define LIMITER_8_READ       (M0P_GPIO->PBIN & (1 << 3)) 

#define LIMITER_9_PORT      GpioPortB
#define LIMITER_9_PIN       GpioPin4
#define LIMITER_9_READ       (M0P_GPIO->PBIN & (1 << 4)) 

#define LIMITER_10_PORT      GpioPortB
#define LIMITER_10_PIN       GpioPin5
#define LIMITER_10_READ       (M0P_GPIO->PBIN & (1 << 5)) 

extern volatile uint8_t     g_u8LimiterFlag;

void bsp_LimiterInit (void);
void bsp_LimiterTest (void);
    
#endif
