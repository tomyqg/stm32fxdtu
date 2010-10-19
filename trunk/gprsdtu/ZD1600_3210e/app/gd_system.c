#include <ucos_ii.h>
#include "gd_system.h"
#include <stdio.h>
#include <string.h>

#include "drv_init.h"
#include "gd_tasks.h"
#include "gd_mem.h"
#include "uart_drv.h"
#include "flash.h"
#include "gd_com_init.h"

gd_system_t gd_system;	  

OS_STK gd_task_init_stk[GD_TASK_INIT_STK_SIZE];

const INT8U GD_DEVMAC[] = {0x0a,0x0c,0x08,0x30,0x57,0x55};	

															  
char gd_msg_partition[GD_MSG_COUNT][GD_MSG_SIZE];
void  gd_task_init(void *parg);

extern INT8U sp2gm_buf_partition[SP2GM_FRAME_NODE_COUNT][GD_FRAME_NODE_SIZE];
extern INT8U gm2sp_buf_partition[GM2SP_FRAME_NODE_COUNT][GD_FRAME_NODE_SIZE];


void gd_config_info_store(gd_config_info_t *gd_conf);
void gd_config_link_info(gd_config_info_t *gd_conf);
void gd_config_info_init(gd_config_info_t *gd_conf);

void gd_uart_init(gd_config_info_t *gd_conf);


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
	{
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
	{
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
/*	{
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


void gd_judge_work_mode()
{
	gd_system.work_mode = gd_get_config();

//gd_system.work_mode =  GD_TRANS_MODE;

}


void gd_start_tasks() 
{
	struct user_task *ptask = NULL;

	if(gd_system.work_mode == GD_CONFIG_MODE)
	{
		Run_Task(&user_tasks[GD_TASK_CONFIG_ID]);
		Run_Task(&user_tasks[GD_TASK_LED_ID]);	
	}
	else if(gd_system.work_mode == GD_TRANS_MODE)
	{
		ptask = &user_tasks[2];
		
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
//	INT8S ret = 0;
	gd_config_info_t *gd_conf = NULL;
 	gd_conf = &gd_system.gd_config_info;

	drv_all_init();
	gd_config_info_init(gd_conf);
	gd_config_link_info(gd_conf);



	gd_component_init(gd_conf);	
	// Judge work mode : 0 GD_CONFIG_MODE, 1 GD_TRANS_MODE
	gd_judge_work_mode();
	if(gd_system.work_mode == GD_TRANS_MODE)
	{
		gd_uart_init(gd_conf);
	}	


	gd_start_tasks();
	
	OSTaskDel(OS_PRIO_SELF);	
}


void gd_start_init_task()
{
	Run_Task(&user_tasks[GD_TASK_INIT_ID]);	

}



int gd_system_init()
{
	INT8U err;

	memset(&gd_system, 0, sizeof(gd_system_t));

	/********************** Initialize Uart ********************************************/
	
	// Create guart task sem
	gd_system.gm_operate_sem = OSSemCreate(1);

	gd_system.gm2sp_buf_sem = OSSemCreate(1);

	gd_system.sp2gm_mem_sem = OSSemCreate(1);
	
	/********************** Create Memory Partitions *******************************/
	gd_system.gd_msg_PartitionPtr = OSMemCreate(gd_msg_partition, GD_MSG_COUNT, GD_MSG_SIZE, &err);

	gd_system.sp2gm_buf_PartitionPtr = OSMemCreate(sp2gm_buf_partition, SP2GM_FRAME_NODE_COUNT, GD_FRAME_NODE_SIZE, &err);
	//gd_system.gm2sp_buf_PartitionPtr = OSMemCreate(gm2sp_buf_partition, GM2SP_FRAME_NODE_COUNT, GD_FRAME_NODE_SIZE, &err);

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

	return 0;
}



void gd_config_link_info(gd_config_info_t *gd_conf)
{
	gd_system.network_task.link_count = gd_conf->gd_svr_cnt.value;
	
	gd_system.network_task.link_info[0].link_num = 0;
	gd_system.network_task.link_info[0].link_type = 0; //TCP
	gd_system.network_task.link_info[0].link_state = GD_LINK_STATE_IDLE;
	strcpy(gd_system.network_task.link_info[0].svr_ip, gd_conf->gd_svr_ip.value);
	gd_system.network_task.link_info[0].svr_port = gd_conf->gd_svr_port.value;
	
	gd_system.network_task.link_info[1].link_num = 1;
	gd_system.network_task.link_info[1].link_type = 0; //TCP
	gd_system.network_task.link_info[1].link_state = GD_LINK_STATE_IDLE;
	strcpy(gd_system.network_task.link_info[1].svr_ip, gd_conf->gd_bkp_svr_ip.value);
	gd_system.network_task.link_info[1].svr_port = gd_conf->gd_bkp_svr_port.value;

	gd_system.network_task.link_info[2].link_num = 2;
	gd_system.network_task.link_info[2].link_type = 0; //TCP
	gd_system.network_task.link_info[2].link_state = GD_LINK_STATE_IDLE;
	strcpy(gd_system.network_task.link_info[2].svr_ip, gd_conf->gd_svr2_ip.value);
	gd_system.network_task.link_info[2].svr_port = gd_conf->gd_svr2_port.value;


}

void gd_config_info_init(gd_config_info_t *gd_conf)
{
	flash_storage_t	flash_storage;
	INT8S res = 0;
	INT8U *ver = NULL;
	INT16U i = 0;

	flash_storage.data = (void*)gd_conf;
	flash_storage.len = sizeof(gd_config_info_t);
	res = load_from_flash(&flash_storage);	
	if(res != 0)
	{


	}
	ver = (INT8U*)gd_conf;
	for(i=0; i<sizeof(gd_config_info_t); i++)
	{
		if(ver[i] != 0xff) break;
	}
	if(i == sizeof(gd_config_info_t))
	{
		gd_conf->gd_dev_id.oid = GD_OID_DEVICE_ID;
		memset(gd_conf->gd_dev_id.value, '0', GD_DEV_ID_LEN);
		
		gd_conf->gd_sim_id.oid = GD_OID_SIM_ID;
		memset(gd_conf->gd_sim_id.value, '0', GD_SIM_ID_LEN);
		gd_conf->gd_sim_id.value[GD_SIM_ID_LEN -1] = '\0';
		
		gd_conf->gd_work_mode.oid = GD_OID_WORK_MODE;
		gd_conf->gd_work_mode.value= GD_WORK_MODE_PROT;
		
		gd_conf->gd_acti_type.oid = GD_OID_ACTI_TYPE;
		gd_conf->gd_acti_type.value = GD_ACTI_TYPE_AUTO;
		
		gd_conf->gd_serial_mode.oid = GD_OID_SERIAL_MODE;
		gd_conf->gd_serial_mode.value = GD_SERIAL_MODE_8N1;
		
		gd_conf->gd_baud_rate.oid = GD_OID_BAUD_RATE;
		gd_conf->gd_baud_rate.value = 9600;	
		
		gd_conf->gd_return_main.oid = GD_OID_RETURN_MAIN;
		gd_conf->gd_return_main.value = GD_RETURN_MAIN_NO;		 

		gd_conf->gd_data_int.oid = GD_OID_DATA_INT;
		gd_conf->gd_data_int.value = 100;		 

		gd_conf->gd_svr_cnt.oid = GD_OID_SERVER_COUNT;
		gd_conf->gd_svr_cnt.value = 1;		 

		gd_conf->gd_svr_ip.oid = GD_OID_SERVER_IP;
		strcpy(gd_conf->gd_svr_ip.value, "218.94.116.78");		 
		gd_conf->gd_svr_port.oid = GD_OID_SERVER_PORT;
		gd_conf->gd_svr_port.value = 20001;
		 
		gd_conf->gd_bkp_svr_ip.oid = GD_OID_BKP_SVR_IP;
		strcpy(gd_conf->gd_bkp_svr_ip.value, "192.168.1.101");		 	
		gd_conf->gd_bkp_svr_port.oid = GD_OID_BKP_SVR_PORT;
		gd_conf->gd_bkp_svr_port.value = 10001;
		 
		gd_conf->gd_svr1_ip.oid = GD_OID_SERVER1_IP;
		strcpy(gd_conf->gd_svr1_ip.value, "192.168.1.101");	
		gd_conf->gd_svr1_port.oid = GD_OID_SERVER1_PORT;
		gd_conf->gd_svr1_port.value = 10001;
		 
		gd_conf->gd_svr2_ip.oid = GD_OID_SERVER2_IP;
		strcpy(gd_conf->gd_svr2_ip.value, "192.168.1.102");	
		gd_conf->gd_svr2_port.oid = GD_OID_SERVER2_PORT;
		gd_conf->gd_svr2_port.value = 10001;
		
		gd_conf->gd_dns_svr.oid = GD_OID_DNS_SVR;
		strcpy(gd_conf->gd_dns_svr.value, "192.168.1.1");	
		 	
		gd_conf->gd_bkp_dns_svr.oid = GD_OID_BKP_DNS_SVR;
		strcpy(gd_conf->gd_bkp_dns_svr.value, "192.168.1.1");	
		 
		gd_conf->gd_apn.oid = GD_OID_APN;
		memset(gd_conf->gd_apn.value, 0, GD_APN_LEN);
		gd_conf->gd_apn_user.oid = GD_OID_APN_USER;
		memset(gd_conf->gd_apn_user.value, 0, GD_APN_USER_LEN);
		gd_conf->gd_apn_pwd.oid = GD_OID_APN_PWD;
		memset(gd_conf->gd_apn_pwd.value, 0, GD_APN_PWD_LEN);
		gd_conf->gd_apn_center.oid = GD_OID_APN_CENTER;
		memset(gd_conf->gd_apn_center.value, 0, GD_APN_CENTER_LEN);
		 
		gd_conf->gd_smsc.oid = GD_OID_SMSC;
		memset(gd_conf->gd_smsc.value, 0, GD_SMSC_LEN);
		 
		gd_conf->gd_poll_int.oid = GD_OID_POLL_INT;
		gd_conf->gd_poll_int.value = 10;
		 
		gd_conf->gd_wake_phone.oid = GD_OID_WAKE_PHONE;
		memset(gd_conf->gd_wake_phone.value, 0, GD_WAKE_PHONE_LEN);
		 
		gd_conf->gd_wake_sms.oid = GD_OID_WAKE_SMS;	
		memset(gd_conf->gd_wake_sms.value, 0, GD_WAKE_SMS_LEN);	 	

		gd_config_info_store(gd_conf);
		
			
	}


}
void gd_config_info_store(gd_config_info_t *gd_conf)
{
	flash_storage_t	flash_storage;
	INT8S res = 0;
		
	flash_storage.data = (void*)gd_conf;
	flash_storage.len = sizeof(gd_config_info_t);
	res = store_to_flash(&flash_storage);
	if(res != 0)
	{


	}
	
}
void gd_msg_malloc(gd_msg_t **msg)
{
	INT8U err = 0;

 	*msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);

	if((*msg) == NULL)
	{
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 100);	
		}
	}
}
void gd_uart_init(gd_config_info_t *gd_conf)
{
	COM_Conf_T conf;
	conf.com = COM1;
	conf.BaudRate = 15200;
	conf.Parity = (COM_Parity_T)gd_conf->gd_serial_mode.value;
	conf.BaudRate = gd_conf->gd_baud_rate.value;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);
}
