
#include "gd_config.h"

OS_STK gd_task_config_stk[GD_TASK_CONFIG_STK_SIZE];

int gd_get_config()
{
	return 0;
}


void gd_task_config(void *parg)
{

	OSTaskDel(OS_PRIO_SELF);
}

