#include "bsp_debug.h"
#include "gpio.h"
#include "uart.h"

#ifdef DEBUG_PRINT


#define DEBUG_TX_PORT         GpioPortA
#define DEBUG_TX_PIN          GpioPin14
#define DEBUG_TX_LOW       M0P_GPIO->PABSET = (1 << 14)
#define DEBUG_TX_HIGH      M0P_GPIO->PABCLR = (1 << 14) 

//#define WIFI_CH         UARTCH0
#define DEBUG_UART    M0P_UART1
#define DEBUG_AF      GpioAf1



void bsp_DebugInit (void)
{
    stc_gpio_cfg_t stcGpioCfg;

    DDL_ZERO_STRUCT(stcGpioCfg); 
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    
//    SEGGER_RTT_printf (0, "%s", WIFI_TCP_CMD);
    
    /* ���ڳ�ʼ�� */
    stc_uart_cfg_t  stcUartCfg;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcUartCfg);                        ///< �ṹ�������ʼֵ����
	DDL_ZERO_STRUCT(stcBaud);
    
    ///<����ģ��ʱ��ʹ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);
    
    delay1ms (4000);
    
                               ///< �ṹ�������ʼֵ����
    Sysctrl_SetFunc (SysctrlSWDUseIOEn, TRUE);
    ///<TX
    stcGpioCfg.enDir =  GpioDirOut;
    Gpio_Init(DEBUG_TX_PORT, DEBUG_TX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(DEBUG_TX_PORT, DEBUG_TX_PIN, DEBUG_AF); //����PA00ΪLPUART1_TX

    //<RX
//    stcGpioCfg.enDir =  GpioDirIn;
//    Gpio_Init(WIFI_RX_PORT, WIFI_RX_PIN, &stcGpioCfg);
//    Gpio_SetAfMode(WIFI_RX_PORT, WIFI_RX_PIN, WIFI_AF); //����PA01ΪLPUART1_RX
    

    ///<LPUART ��ʼ�� (��У�飺ģʽ1���ο������ֲ� 24.6�½�)
    
    //Mode0 8-Bit                     8-Bit Data
    //Mode1 10-Bit  1-Bit Start Bit + 8-Bit Data +            (1/1.5/2)-Bit Stop Bit
    //Mode2 11-Bit  1-Bit Start Bit + 8-Bit Data + 1-Bit B8 + (1/1.5/2)-Bit Stop Bit
    //Mode3 11-Bit  1-Bit Start Bit + 8-Bit Data + 1-Bit B8 + (1/1.5/2)-Bit Stop Bit
    
    ///<UART Init
    stcUartCfg.enRunMode        = UartMskMode1;          ///<ģʽ3
    stcUartCfg.enStopBit        = UartMsk1bit;           ///<1bitֹͣλ
    stcUartCfg.enMmdorCk        = UartMskEven;           ///<ż����
    stcUartCfg.stcBaud.u32Baud  = 115200;                  ///<������9600
    stcUartCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<ͨ��������Ƶ����
    stcUartCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<�������ʱ�ӣ�PCLK��Ƶ��ֵ
    Uart_Init(DEBUG_UART, &stcUartCfg);                   ///<���ڳ�ʼ��

    ///<UART�ж�ʹ��
//    Uart_ClrStatus(WIFI_UART,UartRC);                ///<���������
//    Uart_ClrStatus(WIFI_UART,UartTC);                ///<���������
//    Uart_EnableIrq(WIFI_UART,UartRxIrq);             ///<ʹ�ܴ��ڽ����ж�
//    Uart_EnableIrq(WIFI_UART,UartTxIrq);             ///<ʹ�ܴ��ڽ����ж�
//    EnableNvic(WIFI_UIRQ, IrqLevel3, TRUE);       ///<ϵͳ�ж�ʹ��
}

void bsp_DebugSendByte (uint8_t _u8Data)
{
    uint8_t		_u8Cnt = 0;
	
	M0P_UART1->ICR_f.TCCF = 0;
	M0P_UART1->SBUF_f.DATA = _u8Data;
	while (0 == M0P_UART1->ISR_f.TC)
	{
		delay10us (2);
		_u8Cnt++;
		if (_u8Cnt >= 250)
			break;
    }
	M0P_UART1->ICR_f.TCCF = 0;
}

int fputc(int ch, FILE *f)
{

    bsp_DebugSendByte (ch);

    return ch;
}


#endif

