#ifndef __BSP_WIFI_H__
#define __BSP_WIFI_H__

#include "bsp_debug.h"

typedef enum
{
    WIFI_UPDATE_NONE,
    WIFI_UPDATE_CLEAN_INFO,
    WIFI_UPDATE_HR = 0x02,
    WIFI_UPDATE_ACK = 0x04,
    WIFI_UPDATE_RTC = 0x08,
    WIFI_UPDATE_UNUSUAL = 0x10, // π ’œ
} WIFI_UPDATE_E;

void bsp_WifiInit (void);

void bsp_WifiRcvtimeout(void);

void bsp_WifiRxCmd (uint8_t _u8Type);
    

#endif
