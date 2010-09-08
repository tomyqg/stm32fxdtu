#include <ucos_ii.h>
#include "gd_suart.h"

OS_STK gd_task_suart_stk[GD_TASK_SUART_STK_SIZE];

void gd_task_suart(void *parg)
{
	OSTaskDel(OS_PRIO_SELF);
}

