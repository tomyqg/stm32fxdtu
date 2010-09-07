#include <ucos_ii.h>
#include "gd_guart.h"

OS_STK gd_task_guart_stk[GD_TASK_GUART_STK_SIZE];

void gd_task_guart(void *parg)
{
	OSTaskDel(OS_PRIO_SELF);
}

