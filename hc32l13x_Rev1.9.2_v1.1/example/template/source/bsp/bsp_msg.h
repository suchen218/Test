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
	MSG_TYPE_BUTTON_LEFT_SHORT_PRESS,	// �̰�����
	MSG_TYPE_BUTTON_LEFT_3S_PRESS,		// 2�볤������
	MSG_TYPE_BUTTON_LEFT_SHORT_RELEASE,	// �̰��ͷ�
	MSG_TYPE_BUTTON_LEFT_3S_RELEASE,	// 2�볤���ͷ�

    MSG_TYPE_BUTTON_RIGHT_SHORT_PRESS,	// �̰�����
	MSG_TYPE_BUTTON_RIGHT_3S_PRESS,		// 2�볤������
	MSG_TYPE_BUTTON_RIGHT_SHORT_RELEASE,	// �̰��ͷ�
	MSG_TYPE_BUTTON_RIGHT_3S_RELEASE,	// 2�볤���ͷ�
    
    MSG_TYPE_BUTTON_PAUSE_SHORT_PRESS,	// �̰�����
	MSG_TYPE_BUTTON_PAUSE_3S_PRESS,		// 2�볤������
	MSG_TYPE_BUTTON_PAUSE_SHORT_RELEASE,	// �̰��ͷ�
	MSG_TYPE_BUTTON_PAUSE_3S_RELEASE,	// 2�볤���ͷ�
    
    
	MSG_TYPE_BUTTON_INVALID,		// ��Ч����ֵ


	MSG_TYPE_FRESH_ALL,		// ����ȫ��ˢһ��
//	MSG_TYPE_FRESH_FIRST,

//    MSG_TYPE_FRESH_100MS,
	MSG_TYPE_FRESH_HALF_SECOND,
    MSG_TYPE_FRESH_SECONDS, // ���ź�
    MSG_TYPE_FRESH_2SECONDS,    // 2��
//    MSG_TYPE_FRESH_DAY,     // һ������
//    MSG_TYPE_BAT_STATUS,	// ���״̬
//    MSG_TYPE_BAT_LEVEL,		// ��ص���
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
    
	MSG_TYPE_M_EVT,		// ��������¼�
    MSG_TYPE_CLEAN,     // ��ϴ��Ͱ
    MSG_TYPE_UV,        // ����
    
//    MSG_TYPE_LPTIMER_CLOSE_EXTI_OPEN,
    MSG_TYPE_LOW_POWER_OFF,
	MSG_TYPE_MAX = 0xFF,	/* ��Ч����Ϣ */
}msg_type_t;


void bsp_MsgInit(void);

uint8_t bsp_MsgSend(uint8_t type);

uint8_t bsp_MsgRead(void);

#endif

