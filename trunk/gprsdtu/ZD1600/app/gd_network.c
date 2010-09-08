#include <ucos_ii.h>
#include "gd_network.h"

OS_STK gd_task_network_stk[GD_TASK_NETWORK_STK_SIZE];

void gd_task_network(void *parg)
{
	OSTaskDel(OS_PRIO_SELF);
}


