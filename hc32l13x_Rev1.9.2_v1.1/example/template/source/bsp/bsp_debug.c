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
    
    /* 串口初始化 */
    stc_uart_cfg_t  stcUartCfg;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcUartCfg);                        ///< 结构体变量初始值置零
	DDL_ZERO_STRUCT(stcBaud);
    
    ///<外设模块时钟使能
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);
    
    delay1ms (4000);
    
                               ///< 结构体变量初始值置零
    Sysctrl_SetFunc (SysctrlSWDUseIOEn, TRUE);
    ///<TX
    stcGpioCfg.enDir =  GpioDirOut;
    Gpio_Init(DEBUG_TX_PORT, DEBUG_TX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(DEBUG_TX_PORT, DEBUG_TX_PIN, DEBUG_AF); //配置PA00为LPUART1_TX

    //<RX
//    stcGpioCfg.enDir =  GpioDirIn;
//    Gpio_Init(WIFI_RX_PORT, WIFI_RX_PIN, &stcGpioCfg);
//    Gpio_SetAfMode(WIFI_RX_PORT, WIFI_RX_PIN, WIFI_AF); //配置PA01为LPUART1_RX
    

    ///<LPUART 初始化 (无校验：模式1，参考数据手册 24.6章节)
    
    //Mode0 8-Bit                     8-Bit Data
    //Mode1 10-Bit  1-Bit Start Bit + 8-Bit Data +            (1/1.5/2)-Bit Stop Bit
    //Mode2 11-Bit  1-Bit Start Bit + 8-Bit Data + 1-Bit B8 + (1/1.5/2)-Bit Stop Bit
    //Mode3 11-Bit  1-Bit Start Bit + 8-Bit Data + 1-Bit B8 + (1/1.5/2)-Bit Stop Bit
    
    ///<UART Init
    stcUartCfg.enRunMode        = UartMskMode1;          ///<模式3
    stcUartCfg.enStopBit        = UartMsk1bit;           ///<1bit停止位
    stcUartCfg.enMmdorCk        = UartMskEven;           ///<偶检验
    stcUartCfg.stcBaud.u32Baud  = 115200;                  ///<波特率9600
    stcUartCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<通道采样分频配置
    stcUartCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<获得外设时钟（PCLK）频率值
    Uart_Init(DEBUG_UART, &stcUartCfg);                   ///<串口初始化

    ///<UART中断使能
//    Uart_ClrStatus(WIFI_UART,UartRC);                ///<清接收请求
//    Uart_ClrStatus(WIFI_UART,UartTC);                ///<清接收请求
//    Uart_EnableIrq(WIFI_UART,UartRxIrq);             ///<使能串口接收中断
//    Uart_EnableIrq(WIFI_UART,UartTxIrq);             ///<使能串口接收中断
//    EnableNvic(WIFI_UIRQ, IrqLevel3, TRUE);       ///<系统中断使能
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

