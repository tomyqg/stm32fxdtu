#ifndef GD_GUART_H
#define GD_GUART_H

#include <ucos_ii.h>

#define GUART_QMSG_COUNT 3

typedef struct GD_GUART_TASK
{	 
	INT8U 		task_id;
	INT8U 		prio;
	OS_EVENT 	*q_guart;
	void 		*QMsgTbl[GUART_QMSG_COUNT];
	
}gd_guart_task_t;






#endif

