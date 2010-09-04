#include <stdio.h>
#include <ucos_ii.h>
#include "drv_init.h"
//#include "mp3_dbg.h"
//#include "snd_rec.h"
#include "tasks.h"

struct user_task user_tasks[] = {
	{"monitor", 
     "Monitor task.", 
     1,
	 App_TaskMonitor, (void *) 0,
	 &App_TaskMonitorStk[APP_TASK_MONITOR_STK_SIZE - 1],
	 APP_TASK_MONITOR_PRIO,
	 0, App_TaskMonitorStk, APP_TASK_MONITOR_STK_SIZE, &user_tasks[0],
	 OS_TASK_OPT_STK_CHK,
	 },
	{"led", 
     "User led task.", 
     1,
	 App_led, (void *) 0, &App_TaskLedStk[APP_TASK_LED_STK_SIZE - 1],
	 APP_TASK_LED_PRIO,
	 2, App_TaskLedStk, APP_TASK_LED_STK_SIZE, 0, OS_TASK_OPT_STK_CHK,
	 },


	{NULL}
};

int Run_Task(struct user_task *ptask)
{
	INT8U err;

#if OS_TASK_CREATE_EXT_EN > 0
	err = OSTaskCreateExt(ptask->code, ptask->parg, ptask->pstk,	//&Task1Stk[TASK_STK_SIZE - 1],
			      ptask->prio, ptask->pid,	//task id
			      ptask->pstkb,
			      ptask->stksize, ptask->pdat, ptask->opt);
#else
	err = OSTaskCreate(ptask->code,
			   ptask->parg, ptask->pstk, ptask->prio);
#endif
#if (OS_TASK_NAME_SIZE >= TASK_NAME_LENTH)

	OSTaskNameSet(ptask->prio, (INT8U *) ptask->TaskName, &err);

#endif
	if (OS_ERR_NONE == err)
		return ptask->prio;
	return -1;
}

static void StartFirstTask(void)
{
	Run_Task(&user_tasks[0]);
}

int main(void)
{

	RCC_Configuration();
	NVIC_Configuration();

	OSInit();		/* Initialize "uC/OS-II, The Real-Time Kernel".         */

	StartFirstTask();

	OSStart();

	return 0;
}
