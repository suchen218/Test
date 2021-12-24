#include "bsp_msg.h"

msg_t		msg_info;

void bsp_MsgInit(void)
{
	msg_info.read_pos = 0;
	msg_info.write_pos = 0;
//	memset(msg_info.data, MSG_INFO_MAX, MSG_INFO_MAX);
}

// 发送消息，当消息已满，不再添加新的消息
uint8_t bsp_MsgSend(uint8_t type)
{
	uint8_t		ret = MSG_INFO_SIZE, pos;

	pos = msg_info.write_pos + 1;
	if (pos >= MSG_INFO_SIZE)
		pos = 0;

	if (pos != msg_info.read_pos)
	{
		
		msg_info.data[msg_info.write_pos] = type;
	
		ret = msg_info.write_pos;
		
		if (++msg_info.write_pos >= MSG_INFO_SIZE)
		{
			msg_info.write_pos = 0;
		}
	}
	
	return ret;
}

// 读消息，返回最靠前的消息
uint8_t bsp_MsgRead(void)
{
	uint8_t		type = MSG_TYPE_MAX;
	if (msg_info.read_pos != msg_info.write_pos)
	{
		type = msg_info.data[msg_info.read_pos++];
		if (msg_info.read_pos >= MSG_INFO_SIZE)
			msg_info.read_pos = 0;
	}

	return type;
}

