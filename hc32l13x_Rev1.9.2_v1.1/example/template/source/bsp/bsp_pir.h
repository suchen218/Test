#ifndef __BSP_PIR_H__
#define __BSP_PIR_H__

#include "bsp_debug.h"

// 检测液位，判断马桶是否堵塞，低电平有效
#define PIR_LIQUID_PORT      GpioPortB
#define PIR_LIQUID_PIN       GpioPin10
#define PIR_LIQUID_IS_FULL  (0 == (M0P_GPIO->PBIN & (1 << 10)))
//#define UV_ENABLE       M0P_GPIO->PDBCLR |= (1 << 4) 
//#define UV_DISABLE       M0P_GPIO->PDBSET |= (1 << 4) 


// 热释电感应，来人输出高
#define PIR_EN_PORT      GpioPortB
#define PIR_EN_PIN       GpioPin11
#define PIR_IS_HUMAN    (M0P_GPIO->PBIN & (1 << 11))
//#define PIR_ENABLE       M0P_GPIO->PDBCLR |= (1 << 4) 
//#define PIR_DISABLE       M0P_GPIO->PDBSET |= (1 << 4) 

void bsp_PirInit (void);

#endif
