#include <ucos_ii.h>
#include "test.h"
#include "../drives/uart_drv.h"
#include <string.h>
#include "gd_gm.h"

#include "../drives/flash.h"



OS_STK App_TaskTestStk[APP_TASK_TEST_STK_SIZE];
void App_test(void *parg)
{
	vu32 len;
	vs8 res;
	flash_storage_t storage_data, load_data;
 	COM_Conf_T conf, conf_r;

	conf.com = COM2;
	conf.BaudRate = 9600;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;

	len = sizeof(conf);
	storage_data.len = len;
	storage_data.data = (void *)(&conf);
	res = store_to_flash(&storage_data);

	load_data.len = len;
	load_data.data = (void *)(&conf_r);
	res = load_from_flash(&load_data);
	

	while (1) 
	{
	

	OSTimeDlyHMSM(0, 0, 5, 0);
	}
}





