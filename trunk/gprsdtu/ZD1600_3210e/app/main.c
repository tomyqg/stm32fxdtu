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

	drv_all_init();
	gd_component_init();

	ret = gd_system_init();
	if(ret < 0)
		return -1;

	// Judge work mode : 0 GD_CONFIG_MODE, 1 GD_TRANS_MODE
	gd_judge_work_mode();

	gd_start_tasks();

	OSStart();

	return 0;
}
