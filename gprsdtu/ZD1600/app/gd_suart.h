#ifndef GD_SUART_H
#define GD_SUART_H



#define SUART_QMSG_COUNT 3

typedef struct GD_SUART_TASK
{	 
	INT8U 		task_id;
	INT8U 		prio;
	OS_EVENT 	*q_suart;
	void 		*QMsgTbl[SUART_QMSG_COUNT];
	INT8U		rx_buf[2048];
	
}gd_suart_task_t;
















#endif

