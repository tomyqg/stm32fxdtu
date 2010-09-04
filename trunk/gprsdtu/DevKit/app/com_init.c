/******************************* (C) Embest ***********************************
* File Name          : com_init.c
* Author             : tary
* Date               : 2009-06-02
* Version            : 0.1
* Description        : 组件初使化
******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <ucos_ii.h>
#include "drv_init.h"
#include "com_init.h"

/* Variables ----------------------------------------------------------------*/
//FATFS glbl_fs;

/*=============================================================================
* Function	: component_init
* Description	: 组件初使化
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int component_init(void) {
	int r;

	OS_CPU_SysTickInit();

#if (OS_TASK_STAT_STK_CHK_EN > 0) && (OS_TASK_CREATE_EXT_EN > 0)
	OSStatInit();
#endif

//	GUI_Init();

//    tcpip_init( NULL, NULL );

//    lwip_socket_init();
//	nand_dbg();

	return 0;
}

/************************************END OF FILE******************************/
