#include <ucos_ii.h>
#include "gd_network.h"
#include "gd_system.h"
#include "gd_gm.h"
#include "gd_guart.h"
#include "mystring.h"

#define NULL 0

OS_STK gd_task_network_stk[GD_TASK_NETWORK_STK_SIZE];


/*heartbeat*/
INT8U gd_heart_beat[92];


void gd_task_network(void *parg)
{
	INT32U heartbeat_len;
	INT8S res = 1;
	INT8U err, time_tick;
	gd_msg_t *msg = NULL;
	
	while(gd_guart_ready)		
	{
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
	
	while(res)
	{
		res = gprsmodule_init();
		if(res == 0)
		{
			res = 1;
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
	while(res)
	{
		res = gprs_tcpip_init("", "");
		if(res == 0)
		{
			res = 1;
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
	while(res)
	{
		res = gprs_tcpip_mode_init(0x01);
		if(res == 0)
		{
			res = 1;
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
 	while(res)
	{
		res = gprs_tcpip_creat_connection(gd_system.network_task.link_info[0].link_type ,gd_system.network_task.link_info[0].svr_ip, gd_system.network_task.link_info[0].svr_port, 9055, 9057);
		if(res == 0)
		{
			res = 1;
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 500);
	}

	
	/*message that GD_MSG_CONNECTION_READY	*/		
	msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	msg->type = GD_MSG_CONNECTION_READY;
	msg->data =  (void*)NULL;
	OSQPost(gd_system.guart_task.q_guart, (void*)msg);
	
	msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	msg->type = GD_MSG_CONNECTION_READY;
	msg->data =  (void*)NULL;
	OSQPost(gd_system.suart_task.q_suart, (void*)msg);



	heartbeat_len = gd_heart_beat_init(GD_DEVID, GD_DEVMAC, 0, "test00000000", strlen("test00000000"), 70, gd_heart_beat);
	time_tick = 0;
//	gd_heart_beat_init_t(gd_heart_beat);
	while(1)
	{
		/*heart beat*/
//		if(time_tick++ >=10)
		{
			time_tick = 0;
			OSSemPend(gd_system.gm_operate_sem, GM_OPERATE_TIMEOUT, &err);
			if(err == OS_NO_ERR)
			{
	 			res = gprs_tcpip_send(gd_heart_beat, heartbeat_len, 0);
				if(res != 0)
				{
					/*send error*/
				}
				err = OSSemPost(gd_system.gm_operate_sem);
				if(err != OS_NO_ERR)
				{

				}
			}
		}


		OSTimeDlyHMSM(0, 0, 10, 0);
	}

//	OSTaskDel(OS_PRIO_SELF);
}






