#ifndef __GD_TASKS_H__

#define __GD_TASKS_H__

#define TASK_NAME_LENTH	     10

struct user_task 
{
	INT8U 	*TaskName;
	char 	*desc;
	char 	autorun;

	void 	(*code) (void *);
	void 	*parg;
	OS_STK 	*pstk;
	INT8U 	prio;
	/*        */
	INT8U 	pid;
	OS_STK 	*pstkb;
	INT32U 	stksize;
	void 	*pdat;
	INT16U 	opt;
};

extern struct user_task user_tasks[];
int Run_Task(struct user_task *ptask);

extern OS_STK gd_task_led_stk[GD_TASK_LED_STK_SIZE];
void gd_task_led(void *parg);

extern OS_STK gd_task_config_stk[GD_TASK_CONFIG_STK_SIZE];
void gd_task_config(void *parg);

extern OS_STK gd_task_suart_stk[GD_TASK_SUART_STK_SIZE];
void gd_task_suart(void *parg);

extern OS_STK gd_task_guart_stk[GD_TASK_GUART_STK_SIZE];
void gd_task_guart(void *parg);

extern OS_STK gd_task_guart_rx_stk[GD_TASK_GUART_RX_STK_SIZE];
void gd_task_guart_rx(void *parg);

extern OS_STK gd_task_network_stk[GD_TASK_NETWORK_STK_SIZE];
void gd_task_network(void *parg);

extern OS_STK App_TaskTestStk[];
void App_test(void *parg);


//extern OS_STK Sys_TaskKeySrvStk[SYS_TASK_KEY_SRV_STK_SIZE];
//void Task_Key_Srv(void *parg);

//extern OS_STK Sys_TaskDialSrvStk[SYS_TASK_DIAL_SRV_STK_SIZE];
//void Dial_Srv(void *parg);

/* produce task */
#define APP_PRODUCE_PRIO        (13)
#define APP_PROCECD_STK_SIZE    256
extern OS_STK App_ProduceStk[APP_PROCECD_STK_SIZE];
extern void App_Produce(void *parg);
/* end */
//#include "command.h"

#endif
