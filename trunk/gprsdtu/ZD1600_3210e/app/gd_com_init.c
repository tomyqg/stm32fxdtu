
/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <ucos_ii.h>
#include "drv_init.h"
#include "gd_com_init.h"
#include "gd_gm.h"
#include "uart_drv.h"
#include "gd_system.h"


void gd_uart_init(gd_config_info_t *gd_conf);

/*=============================================================================
* Function	: gd_component_init
* Description	: 组件初使化
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
void gd_component_init(gd_config_info_t *gd_conf) 
{
	gd_uart_init(gd_conf);

	OS_CPU_SysTickInit();

#if (OS_TASK_STAT_STK_CHK_EN > 0) && (OS_TASK_CREATE_EXT_EN > 0)
	OSStatInit();
#endif




}

void gd_uart_init(gd_config_info_t *gd_conf)
{
	COM_Conf_T conf;

	conf.com = COM2;
	conf.BaudRate = 9600;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);

	conf.com = COM1;
	conf.Parity = (COM_Parity_T)gd_conf->gd_serial_mode.value;
	conf.BaudRate = gd_conf->gd_baud_rate.value;
	ZD1600_COMInit(&conf);
	
}








/************************************END OF FILE******************************/

