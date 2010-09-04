/******************************* (C) Embest ***********************************
* File Name          : uart.h
* Author             : tary
* Date               : 2009-05-23
* Version            : 0.2
* Description        : Low Level UART Routines
******************************************************************************/

#ifndef	__UART2_H__
#define	__UART2_H__

void uart2_isr(void);
int uart2_send_ch(int ch);
int uart2_get_key(int ms);
int uart2_peek_key(void);
int uart2_init (int BaudRate);
void uart2_flush(void);
void uart2_read_abort(void);
#endif	//__UART_H__

/************************************END OF FILE******************************/
