#include <ucos_ii.h>
#include <stdio.h>
#include <string.h>
#include "drv_init.h"
#include "com_init.h"
#include "tasks.h"


OS_STK App_TaskMonitorStk[APP_TASK_MONITOR_STK_SIZE];

void AllAppStart(void) {
	struct user_task *ptask = &user_tasks[1];	//跳过自身

	for (; ptask->TaskName != NULL; ptask++) {
		if (ptask->autorun) {
			Run_Task(ptask);
		}
	}

	return;
}

void App_TaskMonitor(void *parg)
{

	drv_all_init();

	component_init();

	// 在打开优先级更高的任务前
	// 所有的设备及组件必须初使化
//	AllAppStart();

	for (;;) {
//	printf("\r\n uC/OS-II%% ");
			OSTimeDlyHMSM(0, 0, 1, 0);
 	}
}
