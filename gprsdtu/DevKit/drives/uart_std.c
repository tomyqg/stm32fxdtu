/******************************* (C) Embest ***********************************
* File Name          : uart.c
* Author             : tary
* Date               : 2009-05-23
* Version            : 0.2
* Description        : Low Level UART Routines
******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <ucos_ii.h>
#include "uart_hw.h"
#include "uart_std.h"

#if F_USE_UCOS
void *RCV_QUE_BUF[UART_MAX_USE][UART_RCV_Q_SIZE];
#endif

uart_dev_t uart_dev_buf[UART_MAX_USE] = {
	{
	1,
	STD_UART_BAUD,
	uart1_gpio_init,
	NULL,

	NULL,
	RCV_QUE_BUF[0],

	//
	//
	},

	{
	2,
	STD_UART_BAUD,
	uart2_gpio_init,
	NULL,

	NULL,
	RCV_QUE_BUF[1],

	//
	//
	},
	
		
};

uart_dev_t* udev_std = &uart_dev_buf[0];
uart_dev_t* udev_cdrs = &uart_dev_buf[1];
 
int getch(void) {
	return uart_get_key(udev_std);
}

int peekch(void) {
	return uart_peek_key(udev_std);
}

/************************************END OF FILE******************************/
