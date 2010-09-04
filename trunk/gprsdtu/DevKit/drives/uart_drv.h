/******************************* (C) Embest ***********************************
* File Name          : uart_drv.h
* Author             : tary
* Date               : 2009-06-25
* Version            : 0.1
* Description        : (多)串口驱动
******************************************************************************/

#ifndef __UART_DRV_H__
#define __UART_DRV_H__

#include "aux_lib.h"
#include <ucos_ii.h>

#define UART_DEL        		0x7F
#define UART_BACKSPACE			0x08
#define UART_CR				0x0D
#define UART_LF				0x0A
#define USE_ASCANF			0
#define F_USE_UCOS			1


typedef struct _uart_dev_t_tag_ {
	int port_nr;	// 在uart_flds中找到硬件设备
	int baud_rate;
	func_none_arg_t port_init;
	func_one_arg_t rcv_isr;
	OS_EVENT* rcv_evt;
	void** rcv_que;
	// OS_EVENT* snd_evt;
	// void** snd_que;
} uart_dev_t;


int uart_init(uart_dev_t* udev);
int uart_flush(uart_dev_t* udev);
int uart_get_key(uart_dev_t* udev);
int uart_peek_key(uart_dev_t* udev);
int uart_pend_abort(uart_dev_t* udev);
int uart_send_ch(uart_dev_t* udev, int ch);
int uart_wait_key(uart_dev_t* udev, int msecs);
int uart_gets(uart_dev_t* udev, char* lin, int n);

#endif //__UART_DRV_H__

/************************************END OF FILE******************************/
