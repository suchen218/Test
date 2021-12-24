#ifndef __BSP_WATER_H__
#define __BSP_WATER_H__

#include "bsp_debug.h"

#define WATER_1_PORT      GpioPortA
#define WATER_1_PIN       GpioPin8
#define WATER_1_ENABLE       M0P_GPIO->PABCLR |= (1 << 8) 
#define WATER_1_DISABLE       M0P_GPIO->PABSET |= (1 << 8) 

#define WATER_2_PORT      GpioPortA
#define WATER_2_PIN       GpioPin9
#define WATER_2_ENABLE       M0P_GPIO->PABCLR |= (1 << 9) 
#define WATER_2_DISABLE       M0P_GPIO->PABSET |= (1 << 9) 

#define WATER_3_PORT      GpioPortA
#define WATER_3_PIN       GpioPin10
#define WATER_3_ENABLE       M0P_GPIO->PABCLR |= (1 << 10) 
#define WATER_3_DISABLE       M0P_GPIO->PABSET |= (1 << 10) 

#define WATER_4_PORT      GpioPortA
#define WATER_4_PIN       GpioPin11
#define WATER_4_ENABLE       M0P_GPIO->PABCLR |= (1 << 11) 
#define WATER_4_DISABLE       M0P_GPIO->PABSET |= (1 << 11)

#define WATER_5_PORT      GpioPortA
#define WATER_5_PIN       GpioPin12
#define WATER_5_ENABLE       M0P_GPIO->PABCLR |= (1 << 12) 
#define WATER_5_DISABLE       M0P_GPIO->PABSET |= (1 << 12) 

#define WATER_6_PORT      GpioPortB
#define WATER_6_PIN       GpioPin8
#define WATER_6_ENABLE       M0P_GPIO->PBBCLR |= (1 << 8) 
#define WATER_6_DISABLE       M0P_GPIO->PBBSET |= (1 << 8) 

#define WATER_7_PORT      GpioPortB
#define WATER_7_PIN       GpioPin9
#define WATER_7_ENABLE       M0P_GPIO->PBBCLR |= (1 << 9) 
#define WATER_7_DISABLE       M0P_GPIO->PBBSET |= (1 << 9) 

void bsp_WaterInit (void);

#endif
