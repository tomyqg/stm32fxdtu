#include <ucos_ii.h>
#include "gd_system.h"
#include <stdio.h>

#include "drv_init.h"
#include "gd_tasks.h"
#include "gd_mem.h"
#include "../drives/uart_drv.h"
#include "test.h"
#include "mem_test.h"

gd_system_t gd_system;	  

OS_STK gd_task_init_stk[GD_TASK_INIT_STK_SIZE];

const INT8U GD_DEVMAC[] = {0x0a,0x0c,0x08,0x30,0x57,0x55};	

void  gd_task_init(void *parg);

extern INT8U sp2gm_buf_partition[SP2GM_FRAME_NODE_COUNT][GD_FRAME_NODE_SIZE];
extern INT8U gm2sp_buf_partition[GM2SP_FRAME_NODE_COUNT][GD_FRAME_NODE_SIZE];


struct user_task user_tasks[] = 
{
	{
		"init",
		"User init task.",
		1,
		gd_task_init,
		(void *)0,
		&gd_task_init_stk[GD_TASK_INIT_STK_SIZE - 1],
		GD_TASK_INIT_PRIO,
		GD_TASK_INIT_ID,
		gd_task_init_stk,
		GD_TASK_INIT_STK_SIZE,
		&user_tasks[0],
		OS_TASK_OPT_STK_CHK,
	},
	{
		"config",
		"User config task.",
		1,
		gd_task_config,
		(void *)0,
		&gd_task_config_stk[GD_TASK_CONFIG_STK_SIZE - 1],
		GD_TASK_CONFIG_PRIO,
		GD_TASK_CONFIG_ID,
		gd_task_config_stk,
		GD_TASK_CONFIG_STK_SIZE,
		&user_tasks[1],
		OS_TASK_OPT_STK_CHK,
	},
	{
		"guart_rx",
		"Uart to GPRS module recieve task.",
		1,
		gd_task_guart_rx,
		(void *)0,
		&gd_task_guart_rx_stk[GD_TASK_GUART_RX_STK_SIZE - 1],
		GD_TASK_GUART_RX_PRIO,
		GD_TASK_GUART_RX_ID,
		gd_task_guart_rx_stk,
		GD_TASK_GUART_RX_STK_SIZE,
		&user_tasks[2],
		OS_TASK_OPT_STK_CHK,
	},
/*	{
		"suart",
		"Uart to serial port task.",
		1,
		gd_task_suart,
		(void *)0,
		&gd_task_suart_stk[GD_TASK_SUART_STK_SIZE - 1],
		GD_TASK_SUART_PRIO,
		GD_TASK_SUART_ID,
		gd_task_suart_stk,
		GD_TASK_SUART_STK_SIZE,
		&user_tasks[3],
		OS_TASK_OPT_STK_CHK,
	},
*/	{
		"guart",
		"Uart to GPRS module task.",
		1,
		gd_task_guart,
		(void *)0,
		&gd_task_guart_stk[GD_TASK_GUART_STK_SIZE - 1],
		GD_TASK_GUART_PRIO,
		GD_TASK_GUART_ID,
		gd_task_guart_stk,
		GD_TASK_GUART_STK_SIZE,
		&user_tasks[4],
		OS_TASK_OPT_STK_CHK,
	},
	{
		"network",
		"Network task.",
		1,
		gd_task_network,
		(void *)0,
		&gd_task_network_stk[GD_TASK_NETWORK_STK_SIZE - 1],
		GD_TASK_NETWORK_PRIO,
		GD_TASK_NETWORK_ID,
		gd_task_network_stk,
		GD_TASK_NETWORK_STK_SIZE,
		&user_tasks[5],
		OS_TASK_OPT_STK_CHK,
	},
	{
		"led", 
	 	"User led task.", 
	 	1,
	 	gd_task_led, 
	 	(void *) "Felix", 
	 	&gd_task_led_stk[GD_TASK_LED_STK_SIZE - 1],
	 	GD_TASK_LED_PRIO,
	 	GD_TASK_LED_ID, 
	 	gd_task_led_stk, 
	 	GD_TASK_LED_STK_SIZE, 
	 	&user_tasks[6], 
	 	OS_TASK_OPT_STK_CHK,
	},
	{
		"test", 
     	"User test task.", 
     	1,
	 	App_test, 
	 	(void *) 0, 
	 	&App_TaskTestStk[APP_TASK_TEST_STK_SIZE - 1],
	 	APP_TASK_TEST_PRIO,
	 	APP_TASK_TEST_ID, 
	 	App_TaskTestStk, 
	 	APP_TASK_TEST_STK_SIZE, 
	 	0, 
	 	OS_TASK_OPT_STK_CHK,
	},
/*	{
		"memery test", 
		"User memery test task.", 
		1,
		App_mem_test, 
		(void *) 0, 
		&App_TaskMemTestStk[APP_TASK_MEM_TEST_STK_SIZE - 1],
		APP_TASK_MEM_TEST_PRIO,
		APP_TASK_MEM_TEST_ID, 
		App_TaskMemTestStk, 
		APP_TASK_MEM_TEST_STK_SIZE, 
		0, 
		OS_TASK_OPT_STK_CHK,
	},
*/
	{NULL}
};

int Run_Task(struct user_task *ptask)
{
	INT8U err;

#if OS_TASK_CREATE_EXT_EN > 0
	err = OSTaskCreateExt(ptask->code, ptask->parg, ptask->pstk,
			      		  ptask->prio, ptask->pid,
			              ptask->pstkb, ptask->stksize, 
						  ptask->pdat, ptask->opt);
#else
	err = OSTaskCreate(ptask->code, ptask->parg, ptask->pstk, ptask->prio);
#endif

#if (OS_TASK_NAME_SIZE >= TASK_NAME_LENTH)

	OSTaskNameSet(ptask->prio, (INT8U *) ptask->TaskName, &err);

#endif
	if (OS_ERR_NONE == err)
		return ptask->prio;

	return -1;
}


int gd_judge_work_mode()
{
	gd_system.work_mode = GD_TRANS_MODE;

	gd_get_config();

	return 0;	
}


void gd_start_tasks() 
{
	struct user_task *ptask = NULL;

	if(gd_system.work_mode == GD_CONFIG_MODE)
	{
		Run_Task(&user_tasks[GD_TASK_CONFIG_ID]);	
	}
	else if(gd_system.work_mode == GD_TRANS_MODE)
	{
		ptask = &user_tasks[1];
		
		for (; ptask->TaskName != NULL; ptask++) 
		{
			if (ptask->autorun) 
			{
				Run_Task(ptask);
			}
		}
	}

	return;
}

void gd_task_init(void *parg)
{
	drv_all_init();
	gd_component_init();

	// Judge work mode : 0 GD_CONFIG_MODE, 1 GD_TRANS_MODE
	gd_judge_work_mode();

	gd_start_tasks();
	
	OSTaskDel(OS_PRIO_SELF);	
}


int gd_start_init_task()
{
	Run_Task(&user_tasks[GD_TASK_INIT_ID]);	

	return 0;
}

void gd_uart_init(COM_TypeDef com_type)
{
	COM_Conf_T conf;
	conf.com = com_type;
	conf.BaudRate = 115200;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);
}
void gd_guart_init(void)//test 
{
	COM_Conf_T conf;
	conf.com = COM2;
	conf.BaudRate = 9600;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);
}


char gd_msg_partition[GD_MSG_COUNT][GD_MSG_SIZE];


int gd_system_init()
{
	INT8U err;

	memset(&gd_system, 0, sizeof(gd_system_t));

	/********************** Initialize Uart ********************************************/
	gd_uart_init(COM1);
	gd_guart_init();
	
	// Create guart task sem
	gd_system.gm_operate_sem = OSSemCreate(1);
	/********************** Create Memory Partitions *******************************/
	gd_system.gd_msg_PartitionPtr = OSMemCreate(gd_msg_partition, GD_MSG_COUNT, GD_MSG_SIZE, &err);

	gd_system.sp2gm_buf_PartitionPtr = OSMemCreate(sp2gm_buf_partition, SP2GM_FRAME_NODE_COUNT, GD_FRAME_NODE_SIZE, &err);
	gd_system.gm2sp_buf_PartitionPtr = OSMemCreate(gm2sp_buf_partition, GM2SP_FRAME_NODE_COUNT, GD_FRAME_NODE_SIZE, &err);

	/********************** Initialize Config Task *********************************/

	// TaskId
	gd_system.config_task.task_id = user_tasks[GD_TASK_CONFIG_ID].pid;

	// Priority
	gd_system.config_task.prio= user_tasks[GD_TASK_CONFIG_ID].prio;

	/********************** Initialize SUART Task *********************************/

	// TaskId
	gd_system.suart_task.task_id = user_tasks[GD_TASK_SUART_ID].pid;

	// Priority
	gd_system.suart_task.prio= user_tasks[GD_TASK_SUART_ID].prio;
	
	// Create suart task queue
	gd_system.suart_task.q_suart = OSQCreate(&gd_system.suart_task.QMsgTbl[0], SUART_QMSG_COUNT);

	/********************** Initialize GUART Task *********************************/

	// TaskId
	gd_system.guart_task.task_id = user_tasks[GD_TASK_GUART_ID].pid;

	// Priority
	gd_system.guart_task.prio= user_tasks[GD_TASK_GUART_ID].prio;


	// Create guart task queue
	gd_system.guart_task.q_guart = OSQCreate(&gd_system.guart_task.QMsgTbl[0], GUART_QMSG_COUNT);


	/********************** Initialize Network Task *********************************/

	// TaskId
	gd_system.network_task.task_id = user_tasks[GD_TASK_NETWORK_ID].pid;

	// Priority
	gd_system.network_task.prio= user_tasks[GD_TASK_NETWORK_ID].prio;

	// Create network task queue
	gd_system.network_task.q_network = OSQCreate(&gd_system.network_task.QMsgTbl[0], NETWORK_QMSG_COUNT);

	// Link info
	gd_system.network_task.link_count = 3;
	
	gd_system.network_task.link_info[0].link_type = 0; //TCP
	strcpy(gd_system.network_task.link_info[0].svr_ip, "218.94.116.78");
	gd_system.network_task.link_info[0].svr_port = 10001;
	
	gd_system.network_task.link_info[1].link_type = 0; //TCP
	strcpy(gd_system.network_task.link_info[1].svr_ip, "218.94.116.78");
	gd_system.network_task.link_info[1].svr_port = 6001;

	gd_system.network_task.link_info[2].link_type = 0; //TCP
	strcpy(gd_system.network_task.link_info[2].svr_ip, "218.94.116.78");
	gd_system.network_task.link_info[2].svr_port = 6002;
	
	return 0;
}


