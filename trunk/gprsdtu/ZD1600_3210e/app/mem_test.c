#include <ucos_ii.h>
#include "gd_mem.h"
#include "mem_test.h"
#include "gd_system.h"

OS_STK App_TaskMemTestStk[APP_TASK_MEM_TEST_STK_SIZE];

extern gd_system_t gd_system; 

char rx_test_buf[300];

void App_mem_test(void *parg)
{

	OSTaskDel(OS_PRIO_SELF);
}


