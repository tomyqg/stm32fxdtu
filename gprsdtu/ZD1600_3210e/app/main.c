#include <stdio.h>
#include <ucos_ii.h>
#include "drv_init.h"
#include "gd_tasks.h"
#include "gd_com_init.h"
#include "gd_system.h"

int main(void)
{
	int ret = 0;

	RCC_Configuration();
	NVIC_Configuration();

	OSInit();		/* Initialize "uC/OS-II, The Real-Time Kernel".*/

	ret = gd_system_init();
	if(ret < 0)
		return -1;

	gd_start_init_task();

	OSStart();

	return 0;
}


