#ifndef GD_GUART_H
#define GD_GUART_H

#include <ucos_ii.h>

#define GUART_QMSG_COUNT 3
//#define GUART_ISR_QMSG_COUNT 3

typedef struct GD_GUART_TASK
{	 
	INT8U 		task_id;
	INT8U 		prio;
	OS_EVENT 	*q_guart;
	void 		*QMsgTbl[GUART_QMSG_COUNT];
	
}gd_guart_task_t;

/*
typedef struct GD_GUART_ISR
{	 
	OS_EVENT 	*q_guart_isr;
	void 		*QMsgTbl[GUART_ISR_QMSG_COUNT];
	
}gd_guart_isr_t;

*/
typedef struct GD_GUART_MSG
{
	int  len;
	char *send_data;

}gd_guart_msg_data_t;


extern INT8U	gd_guart_ready ;




#endif

