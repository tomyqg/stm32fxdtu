#ifndef GD_GUART_H
#define GD_GUART_H

#include <ucos_ii.h>
#include "gd_mem.h"

#define GUART_QMSG_COUNT    10

#define GUART_RX_BUF_SIZE   2078
#define GUART_TX_BUF_SIZE	1024

#define WS120M_HEADER_LEN 24
#define WS120M_FRAME_LEN (GPRS_DATA_LEN_MAX - WS120M_HEADER_LEN)
//debug
//#define WS120M_FRAME_LEN	30




// GM2SP Frame
typedef struct GM2SP_FRAME
{
	INT16U	len;
	INT8U 	buf[GM2SP_BUF_LEN];//用于缓存发往suart的数据

	
	INT8U	packet_index;
	INT8U	packet_sum;
	INT16U	frame_index;
	INT16U	frame_len;
}gm2sp_frame_t;




typedef struct GD_GUART_TASK
{	 
	INT8U 		task_id;
	INT8U 		prio;
	OS_EVENT 	*q_guart;
	void 		*QMsgTbl[GUART_QMSG_COUNT];

	INT8U		rx_buf[GUART_RX_BUF_SIZE];//usart rxbuf
	INT8U       tx_buf[GUART_TX_BUF_SIZE];//usart txbuf
	
}gd_guart_task_t;


#endif

