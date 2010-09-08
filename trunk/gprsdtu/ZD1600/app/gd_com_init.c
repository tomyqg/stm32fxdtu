
/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <ucos_ii.h>
#include "drv_init.h"
#include "gd_com_init.h"

/*=============================================================================
* Function	: gd_component_init
* Description	: 组件初使化
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int gd_component_init() 
{
	OS_CPU_SysTickInit();

#if (OS_TASK_STAT_STK_CHK_EN > 0) && (OS_TASK_CREATE_EXT_EN > 0)
	OSStatInit();
#endif

	return 0;
}

/************************************END OF FILE******************************/
