#ifndef __BSP_AIR_H__
#define __BSP_AIR_H__

#include "bsp_debug.h"

#define AIR_11_PORT      GpioPortC
#define AIR_11_PIN       GpioPin13
#define AIR_11_ENABLE       M0P_GPIO->PCBCLR |= (1 << 13) 
#define AIR_11_DISABLE       M0P_GPIO->PCBSET |= (1 << 13) 

#define AIR_12_PORT      GpioPortC
#define AIR_12_PIN       GpioPin14
#define AIR_12_ENABLE       M0P_GPIO->PCBCLR |= (1 << 14) 
#define AIR_12_DISABLE       M0P_GPIO->PCBSET |= (1 << 14)

#define AIR_21_PORT      GpioPortC
#define AIR_21_PIN       GpioPin15
#define AIR_21_ENABLE       M0P_GPIO->PCBCLR |= (1 << 15) 
#define AIR_21_DISABLE       M0P_GPIO->PCBSET |= (1 << 15)

#define AIR_22_PORT      GpioPortD
#define AIR_22_PIN       GpioPin10
#define AIR_22_ENABLE       M0P_GPIO->PDBCLR |= (1 << 0) 
#define AIR_22_DISABLE       M0P_GPIO->PDBSET |= (1 << 0)

#define AIR_31_PORT      GpioPortD
#define AIR_31_PIN       GpioPin1
#define AIR_31_ENABLE       M0P_GPIO->PDBCLR |= (1 << 1) 
#define AIR_31_DISABLE       M0P_GPIO->PDBSET |= (1 << 1)

// 共用下载口
#define AIR_32_PORT      GpioPortA
#define AIR_32_PIN       GpioPin13
#define AIR_32_ENABLE       M0P_GPIO->PABCLR |= (1 << 13) 
#define AIR_32_DISABLE       M0P_GPIO->PABSET |= (1 << 13)

void bsp_AirInit (void);

#endif
