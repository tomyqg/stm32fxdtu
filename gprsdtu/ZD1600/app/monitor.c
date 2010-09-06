#include <ucos_ii.h>
#include <stdio.h>
#include <string.h>
#include "drv_init.h"
#include "com_init.h"
#include "tasks.h"


OS_STK App_TaskMonitorStk[APP_TASK_MONITOR_STK_SIZE];

void AllAppStart(void) {
	struct user_task *ptask = &user_tasks[1];	//��������

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

	// �ڴ����ȼ����ߵ�����ǰ
	// ���е��豸����������ʹ��
//	AllAppStart();

	for (;;) {
//	printf("\r\n uC/OS-II%% ");
			OSTimeDlyHMSM(0, 0, 1, 0);
 	}
}
