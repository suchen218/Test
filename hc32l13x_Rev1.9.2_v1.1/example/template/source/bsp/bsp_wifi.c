#include "bsp_wifi.h"
#include "bsp_ctrl.h"


#if 0//CONFIG_RTT_SPI_ENABLED
#define WIFI_Print(M, ...)	LOG_Print("WIFI", M, ##__VA_ARGS__)
#else
#define WIFI_Print(M, ...)
#endif

#define WIFI_RX_PORT         GpioPortB
#define WIFI_RX_PIN          GpioPin7
#define WIFI_RX_LOW       M0P_GPIO->PBBSET = (1 << 7)
#define WIFI_RX_HIGH      M0P_GPIO->PBBCLR = (1 << 7) 

#define WIFI_TX_PORT         GpioPortB
#define WIFI_TX_PIN          GpioPin6
#define WIFI_TX_LOW       M0P_GPIO->PBBSET = (1 << 6)
#define WIFI_TX_HIGH      M0P_GPIO->PBBCLR = (1 << 6) 

//#define WIFI_CH         UARTCH0
#define WIFI_UART    M0P_UART0
#define WIFI_UIRQ    UART0_IRQn
#define WIFI_AF      GpioAf2

#define WIFI_ERR_MAX	(6)



#define WIFI_CMD_HR                 "HI"
// 完整清洗
#define WIFI_CMD_COMPLETE_CLEANING  "complete_cleaning"
// 快速清洗
#define WIFI_CMD_QUICK_CLEANING     "quick_cleaning"
// 自清洗
#define WIFI_CMD_SELF_CLEANING      "self_cleaning"
// 消毒
#define WIFI_CMD_DISINFECT          "disinfect"
// 清洗仓轨道推出收回
#define WIFI_CMD_DOORROTATE       "doorrotate"
// 清洗仓轨道上下检测
#define WIFI_CMD_TRACK_MOVE         "trackmove"
// 毛刷旋转检测
#define WIFI_CMD_BRUSH_ROTATE       "brushrotate"
// 清洗仓喷水检测
#define WIFI_CMD_WATER_SPRAY        "waterspray"
// 清洗仓喷液检测
#define WIFI_CMD_LIQUID_SPRAY       "liquidspray"
// UV检测
#define WIFI_CMD_OPEN_LAMP          "openlamp"

// 清洗仓喷气检测
#define WIFI_CMD_JET                    "Jet"
// 地面清洗
#define WIFI_CMD_FLOOR_CLEANING          "floor_cleaning"
// 地面电机检测
#define WIFI_CMD_FLOOR_DOORROTATE          "floor_doorrotate"
// 地面气杆检测
#define WIFI_CMD_FLOOR_HYDRAULICROD          "floor_hydraulicrod"
// 地面喷水检测
#define WIFI_CMD_FLOOR_WATERSPRAY          "floor_waterspray"
// 地面喷液检测
#define WIFI_CMD_FLOOR_LIQUIDSPRAY          "floor_liquidspray"

#define IP_ADDR       "47.114.1.153"
//#define IP_ADDR         "192.168.1.7"//"47.114.1.153"
#define IP_PORT         "9527"
#define FROM_IP_PORT    IP_PORT ":"
#define WIFI_TCP_CMD    "AT+IPSTART=0,tcp," IP_ADDR "," IP_PORT "\r\n"
#define WIFI_SEND_CMD	"AT+IPSEND=0,"

//#define WIFI_SSID		"\"zngctywg\""//"\"Mi 10\""
//#define WIFI_PASSWD		"\"zngc1234\""

#define WIFI_SSID		"\"xiongsi209\""
#define WIFI_PASSWD		"\"Xiongsi123456\""
#define WIFI_CONN_CMD	"AT+CONN=" WIFI_SSID ",,2,"	WIFI_PASSWD "\r\n"
 

#define WIFI_ACK_STR    "{\"data\":\"true\"}"



//time_t      g_tSysTime = {21, 1, 1, 12, 0, 0};

//urinal_usage_info_t     urinal_usage_info;

//uint8_t					urinal_update_flag;
//urinal_usage_info_t		urinal_old_info;

char     	g_s8aWifiRxBuf[128 + 1];
char		g_s8aBakRxBuf[128 + 1];
uint8_t     g_u8WifiIndex;
uint8_t     g_u8WifiTicks;
uint16_t		g_u8WifiErrs;

/* WIFI已经断开 */
uint8_t     g_u8WifiIsConn;

uint8_t		g_u8AckTicks;

/* WIFI 自动上传标志 */
uint8_t     g_u8WifiUpdateFlag = WIFI_UPDATE_NONE;

/* WIFI 上传故障 */
//uint8_t     g_u8WifiUnusualType = WIFI_UNUSUAL_NONE;

char     	g_s8aWifiTxBuf[128];
char		g_s8aWifiMsg[128];


//UART0中断函数
void Uart0_IRQHandler(void)
{
    if(Uart_GetStatus(WIFI_UART, UartRC))         //UART1数据接收
    {
        Uart_ClrStatus(WIFI_UART, UartRC);        //清中断状态位
        g_s8aWifiRxBuf[g_u8WifiIndex++] = M0P_UART0->SBUF_f.DATA;   //接收数据字节
        g_u8WifiIndex &= 0x7F;
        g_u8WifiTicks = 1;
    }
}


uint8_t		g_u8Uid[10];
void bsp_GetUid (void)
{
	uint8_t		i;
	
	LOG_DRAW_Print ("Read Uid: ");
	for (i = 0; i < 10; i++)
	{
		g_u8Uid[i] = *(volatile uint8_t *)(0x00100E74 + i);
		
		LOG_DRAW_Print ("%.2X ", g_u8Uid[i]);
	}
	LOG_DRAW_Print ("\n");
}

void bsp_WifiSendAck (uint8_t _bl)
{
//	memcpy (g_s8aWifiMsg, WIFI_ACK_STR, sizeof(WIFI_ACK_STR));
	sprintf (g_s8aWifiMsg, "{\"data\":\"%s\"}", "true");

}

void bsp_WifiSendUnusual (void)
{
    uint8_t     _u8TypeNums = 0;
	sprintf (g_s8aWifiMsg, "{\"unusual\":");
#if 0    
    if (g_u8WifiUnusualType)
    {
        if (g_u8WifiUnusualType & WIFI_UNUSUAL_FLUME)
        {
            strcat (g_s8aWifiMsg, "\"flume\"");//
            _u8TypeNums++;
        }
        
        
        if (g_u8WifiUnusualType & WIFI_UNUSUAL_PIR)
        {
            if (_u8TypeNums)
            {
                strcat (g_s8aWifiMsg, ",");//
            }
            strcat (g_s8aWifiMsg, "\"pir\"");//
            _u8TypeNums++;
        }
        
        if (g_u8WifiUnusualType & WIFI_UNUSUAL_LIFT)
        {
            if (_u8TypeNums)
            {
                strcat (g_s8aWifiMsg, ",");//
            }
            strcat (g_s8aWifiMsg, "\"lift\"");//
            _u8TypeNums++;
        }
        
        if (g_u8WifiUnusualType & WIFI_UNUSUAL_BRUSH)
        {
            if (_u8TypeNums)
            {
                strcat (g_s8aWifiMsg, ",");//
            }
            strcat (g_s8aWifiMsg, "\"brush\"");//
            _u8TypeNums++;
        }
        
        if (g_u8WifiUnusualType & WIFI_UNUSUAL_DOOR)
        {
            if (_u8TypeNums)
            {
                strcat (g_s8aWifiMsg, ",");//
            }
            strcat (g_s8aWifiMsg, "\"door\"");//
            _u8TypeNums++;
        }
        
        strcat (g_s8aWifiMsg, "}");
    }
    else
    {
        sprintf (g_s8aWifiMsg, "%s\"none\"}", g_s8aWifiMsg);
    }
#endif
}

void bsp_WifiSendHr (void)
{
	if (g_u8AckTicks < 3)
	{
		g_u8AckTicks++;

		sprintf (g_s8aWifiMsg, "%s,%.2X%.2X%.2X%.2X%.2X%.2X", "HI", g_u8Uid[0], \
        g_u8Uid[1], g_u8Uid[2], g_u8Uid[3], g_u8Uid[4], g_u8Uid[5]);
	}
    else
	{
		sprintf (g_s8aWifiMsg, "%s", "HI");
	}
}





void bsp_WifiSendUsageInfo (void)
{
#if 0
    sprintf (g_s8aWifiMsg, "{\"usage_times\":\"%d\",\"cleaning_times\":\"%d\",\"self_times\":\"%d\"}", \
        urinal_old_info.u8UsageCnt, urinal_old_info.u8TotalCleanCnt, urinal_old_info.u8SelfCleanCnt);

	urinal_update_flag = 0;	
#endif
}

void bsp_WifiSendRtc (void)
{
    // {"DEVICE_RTC":"211210173950"}
#if 0
    sprintf (g_s8aWifiMsg, "{\"DEVICE_RTC\":\"%.2d%.2d%.2d%.2d%.2d%.2d\"}", g_tSysTime.u8Year, g_tSysTime.u8Month, \
        g_tSysTime.u8Day, g_tSysTime.u8Hour, g_tSysTime.u8Minute, g_tSysTime.u8Second);
#endif
}

void  bsp_WifiGetRtc (char *_u8pData)
{
#if 0
    uint8_t     _u8Tmp;
    time_t      _time;
    
    
    _u8Tmp = ((_u8pData[0] - '0') > 9) ? (0) : (_u8pData[0] - '0');
    _time.u8Hour = _u8Tmp * 10;
    _u8Tmp = ((_u8pData[1] - '0') > 9) ? (0) : (_u8pData[1] - '0');
    _time.u8Hour += _u8Tmp;
    
    _u8Tmp = ((_u8pData[3] - '0') > 9) ? (0) : (_u8pData[3] - '0');
    _time.u8Minute = _u8Tmp * 10;
    _u8Tmp = ((_u8pData[4] - '0') > 9) ? (0) : (_u8pData[4] - '0');
    _time.u8Minute += _u8Tmp;
    
    _u8Tmp = ((_u8pData[6] - '0') > 9) ? (0) : (_u8pData[6] - '0');
    _time.u8Second = _u8Tmp * 10;
    _u8Tmp = ((_u8pData[7] - '0') > 9) ? (0) : (_u8pData[7] - '0');
    _time.u8Second += _u8Tmp;
    
	if((_time.u8Hour > 23) || (_time.u8Minute > 59) || (_time.u8Second > 59))
	{
		// 无效的时间
//		bsp_WifiSendAck (FALSE);
//		g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
//        LOG_DRAW_Print ("GET_A: %.2d:%.2d:%.2d\n", _time.u8Hour, _time.u8Minute, _time.u8Second);
	}
	else
	{
		g_tSysTime = _time;
//		g_u8WifiUpdateFlag |= WIFI_UPDATE_RTC;
//	bsp_WifiSendRtc ();
        LOG_DRAW_Print ("GET_T: %.2d:%.2d:%.2d\n", g_tSysTime.u8Hour, g_tSysTime.u8Minute, g_tSysTime.u8Second);
	}
#endif    
}


void bsp_WifiInit (void)
{
    uint16_t u16Scnt = 0;
    stc_gpio_cfg_t stcGpioCfg;

    DDL_ZERO_STRUCT(stcGpioCfg); 
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    
//    SEGGER_RTT_printf (0, "%s", WIFI_TCP_CMD);
    
    /* 串口初始化 */
    stc_uart_cfg_t  stcUartCfg;
//	stc_uart_irq_cb_t stcUartIrqCb;
	stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcUartCfg);                        ///< 结构体变量初始值置零
//	DDL_ZERO_STRUCT(stcUartIrqCb);
	DDL_ZERO_STRUCT(stcMulti);
	DDL_ZERO_STRUCT(stcBaud);
    
    ///<外设模块时钟使能
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);
    
                               ///< 结构体变量初始值置零

    ///<TX
    stcGpioCfg.enDir =  GpioDirOut;
    Gpio_Init(WIFI_TX_PORT, WIFI_TX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(WIFI_TX_PORT, WIFI_TX_PIN, WIFI_AF); //配置PA00为LPUART1_TX

    //<RX
    stcGpioCfg.enDir =  GpioDirIn;
    Gpio_Init(WIFI_RX_PORT, WIFI_RX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(WIFI_RX_PORT, WIFI_RX_PIN, WIFI_AF); //配置PA01为LPUART1_RX
    

    ///<LPUART 初始化 (无校验：模式1，参考数据手册 24.6章节)
    
    //Mode0 8-Bit                     8-Bit Data
    //Mode1 10-Bit  1-Bit Start Bit + 8-Bit Data +            (1/1.5/2)-Bit Stop Bit
    //Mode2 11-Bit  1-Bit Start Bit + 8-Bit Data + 1-Bit B8 + (1/1.5/2)-Bit Stop Bit
    //Mode3 11-Bit  1-Bit Start Bit + 8-Bit Data + 1-Bit B8 + (1/1.5/2)-Bit Stop Bit
    
//	stcUartIrqCb.pfnRxIrqCb   = RxIntCallback;//中断入口地址
    

    ///<UART Init
    stcUartCfg.enRunMode        = UartMskMode1;          ///<模式3
    stcUartCfg.enStopBit        = UartMsk1bit;           ///<1bit停止位
    stcUartCfg.enMmdorCk        = UartMskEven;           ///<偶检验
    stcUartCfg.stcBaud.u32Baud  = 115200;                  ///<波特率9600
    stcUartCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<通道采样分频配置
    stcUartCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<获得外设时钟（PCLK）频率值
    Uart_Init(WIFI_UART, &stcUartCfg);                   ///<串口初始化

    ///<UART中断使能
    Uart_ClrStatus(WIFI_UART,UartRC);                ///<清接收请求
    Uart_ClrStatus(WIFI_UART,UartTC);                ///<清接收请求
    Uart_EnableIrq(WIFI_UART,UartRxIrq);             ///<使能串口接收中断
//    Uart_EnableIrq(WIFI_UART,UartTxIrq);             ///<使能串口接收中断
    EnableNvic(WIFI_UIRQ, IrqLevel3, TRUE);       ///<系统中断使能
	
	/* 读取芯片的唯一UID */
	bsp_GetUid ();
}

void bsp_WifiRcvtimeout(void)
{
    if (g_u8WifiTicks)
    {
        if (++g_u8WifiTicks >= 3)
        {
            g_u8WifiTicks = 0;
            bsp_MsgSend(MSG_TYPE_WIFI_RX_FINISH);
        }
    }
}


//void bsp_WifiProtocol (void)
//{
//    uint8_t     i;
//    if (g_u8WifiIndex)
//    {
//        for (i = 0; i < g_u8WifiIndex; i++)
//        {
//            LOG_DRAW_Print ("%c", g_s8aWifiRxBuf[i]);
//        }
//        
//        LOG_DRAW_Print ("\n");
//        
//        g_u8WifiIndex = 0;
//    }
//}

void bsp_WifiSendByte (char _s8Data)
{
	uint8_t		_u8Cnt = 0;
	
	WIFI_UART->ICR_f.TCCF = 0;
	WIFI_UART->SBUF_f.DATA = _s8Data;
	while (0 == WIFI_UART->ISR_f.TC)
	{
		delay10us (2);
		_u8Cnt++;
		if (_u8Cnt >= 250)
			break;
    }
	WIFI_UART->ICR_f.TCCF = 0;
    
//    Uart_SendData (WIFI_CH, _s8Data);
}


void bsp_WifiSendString(char *s8pStr)
{
    while (*s8pStr)
    {
		bsp_WifiSendByte (*s8pStr);
        
//        SEGGER_RTT_printf (0, "%c", *s8pStr);
        
        s8pStr++;
    }
	
//	g_u8WifiErrs = 0; 
}

void bsp_WifiSendManyByte(char *s8pStr, uint8_t _u8Len)
{
	uint8_t		_u8Cnt = 0;
	
    while (_u8Len--)
    {
        WIFI_UART->ICR_f.TCCF = 0;
		WIFI_UART->SBUF_f.DATA = *s8pStr++;
		_u8Cnt = 0;
		while (0 == WIFI_UART->ISR_f.TC)
		{
			delay10us (1);
			_u8Cnt++;
			if (_u8Cnt >= 250)
				break;
		}
		WIFI_UART->ICR_f.TCCF = 0;
    }
}

//int fputc(int ch, FILE *f)
//{

//    M0P_UART1->ICR_f.TCCF = 0;
//    M0P_UART1->SBUF_f.DATA = ch & 0xFF;
//    while (0 == M0P_UART1->ISR_f.TC)
//    {}
//    M0P_UART1->ICR_f.TCCF = 0;

//    return ch;
//}


void bsp_WifiRxCmd (uint8_t _u8Type)
{
	static uint8_t s_u8Index = 0;
	
	uint8_t		_u8Len = 0;
	uint8_t		i;
	char		*_s8pBuf = NULL;
	uint8_t		_u8Data;
	
	
	if (_u8Type == MSG_TYPE_WIFI_RX_FINISH)
	{
		_u8Len = g_u8WifiIndex;
		memcpy (g_s8aBakRxBuf, g_s8aWifiRxBuf, g_u8WifiIndex);
		g_u8WifiIndex = 0;
        
        LOG_DRAW_Print ("Get msg %d: ", s_u8Index);
		
		for (i = 0; i < _u8Len; i++)
        {
            LOG_DRAW_Print ("%c", g_s8aBakRxBuf[i]);
        }
        
        LOG_DRAW_Print ("\n");
	}
    else
    {
        LOG_DRAW_Print ("s_u8Index: %d\n", s_u8Index);
    }
	g_s8aBakRxBuf[_u8Len] = 0;
    
    if (strstr(g_s8aBakRxBuf, "CLOSED"))
    {
        // 已经断开，需要重新连接
        g_u8WifiIsConn = 0;
    }
	
    _s8pBuf = strstr (g_s8aBakRxBuf, "HI,");
                       
    if (_s8pBuf)
    {
        bsp_WifiGetRtc (_s8pBuf + 3);   // HI,23:59:30
    }

	
	switch (s_u8Index)
	{
        case 0: // 复位指令
            bsp_WifiSendString ("AT+RST=1000000\r\n");
//        g_u8WifiErrs++;
//        SEGGER_RTT_printf (0, "eee: %d\n", g_u8WifiErrs);
            s_u8Index++;
            break;
        
        case 1:
            if (strstr (g_s8aBakRxBuf, "OK"))
			{
				s_u8Index++;
			}
			else
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= WIFI_ERR_MAX)
				{
					g_u8WifiErrs = 0;
					s_u8Index = 0;
				}
			}
            break;
         
        case 2: // 稍等一会发送AT试探指令
//            SEGGER_RTT_printf (0, "g_u8WifiErrs: %d\n", g_u8WifiErrs);
            g_u8WifiErrs++;
            if (g_u8WifiErrs >= WIFI_ERR_MAX)
            {
                g_u8WifiErrs = 0;
                s_u8Index++;
                bsp_WifiSendString ("AT\r\n"); // OK 或 ERROR   
            }
            break;
            
        case 3:
            if (strstr (g_s8aBakRxBuf, "OK"))
			{
                // 设置STA模式，连接WIFI热点
				bsp_WifiSendString ("AT+STARTSTA\r\n"); // OK 或 ERROR
				s_u8Index++;
			}
			else
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= WIFI_ERR_MAX)
				{
					g_u8WifiErrs = 0;
					s_u8Index = 0;
				}
			}
            break;
            
        case 4:
            if (strstr (g_s8aBakRxBuf, "OK"))
			{
                // 连接WIFI热点
				bsp_WifiSendString (WIFI_CONN_CMD); // OK 或 ERROR
//                bsp_WifiSendString ("AT+SCAN\r\n");
				s_u8Index++;
			}
			else
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= WIFI_ERR_MAX)
				{
					g_u8WifiErrs = 0;
					s_u8Index = 0;
				}
			}
            break;
            
        case 5:
            /*
                CONNECTED       ->  连上热点
                DISCONNECTED    ->  密码错误等原因，断开连接 
                NOT FIND        ->  WIFI热点不存在
            */
            if (strstr (g_s8aBakRxBuf, ":CONNECTED"))
			{
                // 启动DHCP
				bsp_WifiSendString ("AT+DHCP=wlan0,1\r\n"); // OK 或 ERROR
				s_u8Index++;
			}
            else if (strstr (g_s8aBakRxBuf, "DISCONNECTED"))
            {
                g_u8WifiErrs = 0;
				s_u8Index = 0;
            }
			else //if (strstr (g_s8aBakRxBuf, "DISCONNECTED"))  // 
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= (WIFI_ERR_MAX << 3))
				{
					g_u8WifiErrs = 0;
					s_u8Index = 0;
				}
			}
            break;
            
        case 6:
            if (strstr (g_s8aBakRxBuf, "OK"))
			{
                // 连接TCP IP
				
				s_u8Index = 201;
                g_u8WifiErrs = 0;
			}
			else
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= WIFI_ERR_MAX)
				{
					g_u8WifiErrs = 0;
					s_u8Index = 0;
				}
			}
            break;
            
        case 201:
            g_u8WifiErrs++;
//        SEGGER_RTT_printf (0, "send tcp cmd: %d\n", g_u8WifiErrs);
            if (g_u8WifiErrs >= WIFI_ERR_MAX)
            {
                g_u8WifiErrs = 0;
                s_u8Index = 7;
//                SEGGER_RTT_printf (0, "ready to send tcp 000\n");
                bsp_WifiSendString (WIFI_TCP_CMD); // OK 或 ERROR
//                SEGGER_RTT_printf (0, "ready to send tcp 111\n");
            }
            
            break;
            
        case 7:
            if (strstr (g_s8aBakRxBuf, "OK"))
			{
                // 连接TCP IP
//				bsp_WifiSendString ("AT+IPSTART=0,tcp,192.168.0.117,9527\r\n"); // OK 或 ERROR
				g_u8WifiUpdateFlag |= WIFI_UPDATE_HR;
				g_u8AckTicks = 0;
				s_u8Index++;
                g_u8WifiIsConn = 1;
			}
			else
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= (WIFI_ERR_MAX << 2))
				{
					g_u8WifiErrs = 0;
					s_u8Index = 0;
				}
			}
            break;//CLOSED 断开连接

			
		case 8:
            if (0 == g_u8WifiIsConn)
            {
                bsp_WifiSendString (WIFI_TCP_CMD); // OK 或 ERROR
                s_u8Index = 7;
                return;
            }
        
			_s8pBuf = strstr (g_s8aBakRxBuf, "+IPD,0,");
			if (_s8pBuf)
			{
				_u8Data = 0;
				_s8pBuf += 7; // 指向长度
                
                char    *_s8pBak = _s8pBuf;  
				
				while ((*_s8pBak >= '0') && (*_s8pBak <= '9'))
				{
					_u8Data *= 10;
					_u8Data += *_s8pBak - '0';
                    _s8pBak++;
				}
				
				WIFI_Print ("Get Wifi Data %d Bytes", _u8Data);
				
				_s8pBuf = strstr (_s8pBuf, FROM_IP_PORT);
				if (_s8pBuf)
				{
					_s8pBuf += 5;
					// 下方应该解析wifi数据
#if 1                    // 暂不确定需要怎么做，先屏蔽
                    if (strstr (_s8pBuf, WIFI_CMD_COMPLETE_CLEANING))
                    {
                        // 完整清洗 刷马桶 + 洗毛刷 + 消毒 
                        g_u32DevAllEvt |= DEV_EVT_CLEAN_COMPLETE;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_QUICK_CLEANING))
                    {
                        g_u32DevAllEvt |= DEV_EVT_CLEAN_FAST;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_SELF_CLEANING))
                    {
                        // 自清洗 只洗毛刷
                        g_u32DevAllEvt |= DEV_EVT_CLEAN_MYSELF;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_DISINFECT))
                    {
                        // 消毒
                        g_u32DevAllEvt |= DEV_EVT_CLEAN_UV;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_DOORROTATE))   // 门进出
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_MOTOR_HORIZONTAL;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_TRACK_MOVE))     // 轨道上下检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_MOTOR_VERTICAL;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_BRUSH_ROTATE))   // 毛刷旋转检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_MOTOR_BRUSH;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_WATER_SPRAY))    // 喷水检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_WATER_CLEAN;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_LIQUID_SPRAY))   // 喷液检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_LIQUID_CLEAN;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_OPEN_LAMP))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_UV_CLEAN;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    ////////////////////////////////////////
                    else if (strstr (_s8pBuf, WIFI_CMD_OPEN_LAMP))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_UV_CLEAN;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_JET))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_AIR_CLEAN;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_FLOOR_CLEANING))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_CLEAN_GROUND;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_FLOOR_DOORROTATE))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_GROUND_MOTOR;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_FLOOR_HYDRAULICROD))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_GROUND_AIR;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_FLOOR_WATERSPRAY))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_GROUND_WATER;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else if (strstr (_s8pBuf, WIFI_CMD_FLOOR_LIQUIDSPRAY))      // 紫外灯检测
                    {
                        g_u32DevAllEvt |= DEV_EVT_TEST_GROUND_LIQUID;
						bsp_WifiSendAck (TRUE);
                        g_u8WifiUpdateFlag |= WIFI_UPDATE_ACK;
                    }
                    else 
                    #endif
                    {
//                        LOG_DRAW_Print ("mm: %s\n", _s8pBuf);
                        
						_s8pBuf = strstr (_s8pBuf, "RTC");
						if (_s8pBuf)
						{
							bsp_WifiGetRtc (_s8pBuf + 6);// RTC":"23:59:30	
						}
                    }
//                    else if (strstr (_s8pBuf, "hr"))
//                    {
//                        // 消毒
//                        bsp_WifiSendHr ();
//                        _s8pBuf++;
//                        g_u8WifiUpdateFlag |= WIFI_UPDATE_HR;
//                    }
                    
                    /*
                    需要发送的数据：
                        1.心跳包
                        2.异常上报
                        3.每日数据上报，晚上12点
                        4.服务器唯一标识
                    */
				}
			}
            else if (strstr (g_s8aBakRxBuf, "CLOSED"))
            {
                bsp_WifiSendString (WIFI_TCP_CMD); // OK 或 ERROR
                s_u8Index = 7;
                return;
            }
            
            if (8 == s_u8Index)
            {
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= 30)
				{
					g_u8WifiErrs = 0;
					g_u8WifiUpdateFlag |= WIFI_UPDATE_HR;
				}	
#if 0
				if (urinal_update_flag)
				{
					g_u8WifiUpdateFlag |= WIFI_UPDATE_CLEAN_INFO;
				}
#endif
				
				if (g_u8WifiUpdateFlag & WIFI_UPDATE_ACK)
                {
                    g_u8WifiUpdateFlag &= ~WIFI_UPDATE_ACK;
                    s_u8Index++;
                }
                else if (g_u8WifiUpdateFlag & WIFI_UPDATE_CLEAN_INFO)
                {
                    g_u8WifiUpdateFlag &= ~WIFI_UPDATE_CLEAN_INFO;
                    bsp_WifiSendUsageInfo ();
                    s_u8Index++;
                }
                else if (g_u8WifiUpdateFlag & WIFI_UPDATE_HR)
                {
                    g_u8WifiUpdateFlag &= ~WIFI_UPDATE_HR;
                    bsp_WifiSendHr ();
                    s_u8Index++;
                }
                else if (g_u8WifiUpdateFlag & WIFI_UPDATE_RTC)
				{
					g_u8WifiUpdateFlag &= ~WIFI_UPDATE_RTC;
                    bsp_WifiSendRtc ();
                    s_u8Index++;
				}
				else if (g_u8WifiUpdateFlag & WIFI_UPDATE_UNUSUAL)
				{
					g_u8WifiUpdateFlag &= ~WIFI_UPDATE_UNUSUAL;
                    bsp_WifiSendUnusual ();
                    s_u8Index++;
				}
            }

			break;
			
		case 9:
			sprintf (g_s8aWifiTxBuf, "%s%d\r\n", WIFI_SEND_CMD, strlen(g_s8aWifiMsg));
        
//            SEGGER_RTT_printf (0, "%s", g_s8aWifiTxBuf);
            bsp_WifiSendString (g_s8aWifiTxBuf);
			s_u8Index++;
			break;
		
		case 10:
			if (strstr (g_s8aBakRxBuf, ">"))
			{
				bsp_WifiSendString (g_s8aWifiMsg); // OK 或 ERROR
				bsp_WifiSendByte ('\0');
				s_u8Index++;
			}
			else
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= WIFI_ERR_MAX)
				{
					g_u8WifiErrs = 0;
					s_u8Index = 5;
				}
			}
			break;
			
		case 11:
			if (strstr (g_s8aBakRxBuf, "bytes"))   // SEND 11 bytes
			{
				s_u8Index = 8;	// 等待接收指令
			}
			else
			{
				g_u8WifiErrs++;
				if (g_u8WifiErrs >= WIFI_ERR_MAX)
				{
					g_u8WifiErrs = 0;
					s_u8Index = 5;
				}
			}
			break;
	}
}

