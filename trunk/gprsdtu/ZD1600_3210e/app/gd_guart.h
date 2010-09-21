#ifndef GD_GUART_H
#define GD_GUART_H

#include <ucos_ii.h>

#define GUART_QMSG_COUNT 3

#define GUART_RX_BUF_SIZE   512
#define GUART_TX_BUF_SIZE	1024

typedef struct GD_GUART_TASK
{	 
	INT8U 		task_id;
	INT8U 		prio;
	OS_EVENT 	*q_guart;
	void 		*QMsgTbl[GUART_QMSG_COUNT];

	INT8U		rx_buf[GUART_RX_BUF_SIZE];
	INT8U       tx_buf[GUART_TX_BUF_SIZE];
	
}gd_guart_task_t;


#endif

