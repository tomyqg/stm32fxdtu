/******************************* (C) Embest ***********************************
* File Name          : uart.h
* Author             : tary
* Date               : 2009-05-23
* Version            : 0.2
* Description        : Low Level UART Routines
******************************************************************************/

#ifndef	__UART_H__
#define	__UART_H__

#include "uart_drv.h"

#define STD_UART_BAUD			115200
#define UART_MAX_USE			2
#define UART_RCV_Q_SIZE			80


extern uart_dev_t* udev_std;
//extern uart_dev_t* udev_cdrs;


#if USE_ASCANF
__inline int agetchar(void) {
	return uart_get_key();
}
int ascanf(const char*, ...);
#else
#define ascanf scanf
#define agetchar getchar
#endif

#endif	//__UART_H__

/************************************END OF FILE******************************/
