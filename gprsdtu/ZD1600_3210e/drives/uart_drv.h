/******************************* (C) ZDC ***********************************
* File Name          : uart_drv.h
* Author             : kidss
* Date               : 2010-09-04
* Version            : 1.0
* Description        : UART Driver
******************************************************************************/
#ifndef	__UART_DRV_H__
#define	__UART_DRV_H__

#include "stm32f10x_type.h"


#define COMn		3
//#define	COM_BUF_LEN	1024




typedef enum 
{
	COM1 = 0,
	COM2 = 1
//	COM3 = 2
//  COM4 = 3,	//uart4
//  COM5 = 4	//uart5
}COM_TypeDef;

typedef enum 
{
	WL_8b = 0,
//	WL_9b = 1	
}COM_WordLength_T;
typedef enum 
{
	SB_1 =		0,
	SB_0_5 =	1,
	SB_2 =		2,
	SB_1_5 =	3
}COM_StopBits_T;
typedef enum 
{
	No = 	0,
	Even =	1,
	Odd =	2 
}COM_Parity_T;
typedef enum 
{
	None =		0,
	RTS =		1,
	CTS =		2,
	RTS_CTS =	3
}COM_HwFlowControl_T;


typedef	struct {
	COM_TypeDef	com;
	u32 BaudRate;
	COM_WordLength_T WordLength;
	COM_StopBits_T StopBits;	
	COM_Parity_T Parity;
	COM_HwFlowControl_T HwFlowCtrl;
}COM_Conf_T;
/* COM configured as follow:
       - BaudRate = 9600 baud  
       - Word Length = 8 Bits
       - One Stop Bit
       - Parity Odd
       - Hardware flow control desabled
       - Receive and transmit enabled
*/

typedef	struct {
	u8*	rxbuf;
	u32	rxlen;
	u32 rxbuflen_max;
	u8	*txbuf;
	u32	txlen;
}Uart_Buff_T;





void ZD1600_COMInit(COM_Conf_T *uart_conf);
void uart_rx_itconf(COM_TypeDef comx, FunctionalState newstate);
u32* uart_rx_bufset(COM_TypeDef comx, u8 *buf, u32 maxlen);
u32* uart1_senddata(u8 *data, u32 len);
u32* uart2_senddata(u8 *data, u32 len);
void uart1_rx_isr(void);
void uart1_tx_isr(void);
void uart2_rx_isr(void);
void uart2_tx_isr(void);


  


#endif

