#ifndef __BSP_MSG_H__
#define __BSP_MSG_H__

#include <stdint.h>
#include <stdio.h>

#define MSG_INFO_SIZE	(40)

typedef void (*menuFunc)(uint8_t _u8Type);

typedef struct
{
	uint8_t		read_pos;
	uint8_t		write_pos;
	uint8_t		data[MSG_INFO_SIZE];
}msg_t;

//#define BUTTON_SHORT	(2)
//#define BUTTON_3S		(60)
//#define BUTTON_6S		(100)   // 5s

typedef enum
{
	MSG_TYPE_BUTTON_LEFT_SHORT_PRESS,	// 短按按下
	MSG_TYPE_BUTTON_LEFT_3S_PRESS,		// 2秒长按按下
	MSG_TYPE_BUTTON_LEFT_SHORT_RELEASE,	// 短按释放
	MSG_TYPE_BUTTON_LEFT_3S_RELEASE,	// 2秒长按释放

    MSG_TYPE_BUTTON_RIGHT_SHORT_PRESS,	// 短按按下
	MSG_TYPE_BUTTON_RIGHT_3S_PRESS,		// 2秒长按按下
	MSG_TYPE_BUTTON_RIGHT_SHORT_RELEASE,	// 短按释放
	MSG_TYPE_BUTTON_RIGHT_3S_RELEASE,	// 2秒长按释放
    
    MSG_TYPE_BUTTON_PAUSE_SHORT_PRESS,	// 短按按下
	MSG_TYPE_BUTTON_PAUSE_3S_PRESS,		// 2秒长按按下
	MSG_TYPE_BUTTON_PAUSE_SHORT_RELEASE,	// 短按释放
	MSG_TYPE_BUTTON_PAUSE_3S_RELEASE,	// 2秒长按释放
    
    
	MSG_TYPE_BUTTON_INVALID,		// 无效按键值


	MSG_TYPE_FRESH_ALL,		// 整屏全部刷一遍
//	MSG_TYPE_FRESH_FIRST,

//    MSG_TYPE_FRESH_100MS,
	MSG_TYPE_FRESH_HALF_SECOND,
    MSG_TYPE_FRESH_SECONDS, // 秒信号
    MSG_TYPE_FRESH_2SECONDS,    // 2秒
//    MSG_TYPE_FRESH_DAY,     // 一天的零点
//    MSG_TYPE_BAT_STATUS,	// 电池状态
//    MSG_TYPE_BAT_LEVEL,		// 电池电量
//    MSG_TYPE_BAT_ADC,

	MSG_TYPE_TIME_MINUTE,
	MSG_TYPE_TIME_HOUR,
	MSG_TYPE_TIME_DAY,

    MSG_TYPE_BATTERY_NONE,
    MSG_TYPE_BATTERY_CHARGE,
    MSG_TYPE_BATTERY_FULL,
    
    MSG_TYPE_WIFI_RX_FINISH,
	MSG_TYPE_WIFI_TX_READY,
//    MSG_TYPE_CO2_NEW_DATA,
    MSG_TYPE_TEMP_NEW_DATA,
    MSG_TYPE_HUMI_NEW_DATA,
    MSG_TYPE_LPTIMER_DISABLE,
    MSG_TYPE_USB_IN,
    
	MSG_TYPE_M_EVT,		// 电机工作事件
    MSG_TYPE_CLEAN,     // 清洗马桶
    MSG_TYPE_UV,        // 消毒
    
//    MSG_TYPE_LPTIMER_CLOSE_EXTI_OPEN,
    MSG_TYPE_LOW_POWER_OFF,
	MSG_TYPE_MAX = 0xFF,	/* 无效的消息 */
}msg_type_t;


void bsp_MsgInit(void);

uint8_t bsp_MsgSend(uint8_t type);

uint8_t bsp_MsgRead(void);

#endif

