#ifndef __BSP_DEBUG_H__
#define __BSP_DEBUG_H__

#include <stdio.h>
#include "gpio.h"
#include "uart.h"
#include "sysctrl.h"
#include "adt.h"
#include "ddl.h"
#include "bt.h"
#include "wdt.h"
#include "flash.h"

#include "bsp_air.h"
#include "bsp_limiter.h"
#include "bsp_motor.h"
#include "bsp_msg.h"
#include "bsp_pir.h"
#include "bsp_timer.h"
#include "bsp_uv.h"
#include "bsp_water.h"
#include "bsp_wifi.h"
#include "bsp_ctrl.h"


#define SHORT_FILE 	strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : (__FILE__)

#ifdef DEBUG_PRINT

#define LOG_DRAW_Print     printf


#ifdef TICKS_PRINT
#define LOG_Print(N, M, ...)		do {	\
	printf ("[%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d][%s:%d] " M " \n\r", system_database.pInfo->utc.year, system_database.pInfo->utc.month, system_database.pInfo->utc.day,	\
								system_database.pInfo->utc.hour, system_database.pInfo->utc.minute, system_database.pInfo->utc.second, ((NRF_RTC1->COUNTER & 0x7FFF) * 1000) >> 15, __func__, __LINE__, ##__VA_ARGS__);	\
} while(0)

#else
#define LOG_Print(N, M, ...)		do {	\
	printf ("[%s:%d (%s) ] " M " \n\r", SHORT_FILE, __LINE__, __func__, ##__VA_ARGS__);	\
} while(0)
#endif

#elif defined RTT_PRINT

#include "SEGGER_RTT.h"

#define LOG_DRAW_Print(...)     SEGGER_RTT_printf(0, ##__VA_ARGS__)

#define LOG_Print(N, M, ...)		do {	\
	SEGGER_RTT_printf(0, "[%s:%d (%s) ] " M " \n\r", SHORT_FILE, __LINE__, __func__, ##__VA_ARGS__);	\
} while(0)

#else
#define LOG_Print(...)
#define LOG_DRAW_Print(...)
#endif


#ifdef DEBUG_PRINT
void bsp_DebugInit (void);
#endif


#endif
