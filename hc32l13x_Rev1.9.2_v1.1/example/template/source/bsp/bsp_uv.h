#ifndef __BSP_UV_H__
#define __BSP_UV_H__

#include "bsp_debug.h"

#define UV_EN_PORT      GpioPortD
#define UV_EN_PIN       GpioPin4
#define UV_ENABLE       M0P_GPIO->PDBCLR |= (1 << 4) 
#define UV_DISABLE       M0P_GPIO->PDBSET |= (1 << 4) 

void bsp_UvInit (void);

#endif
