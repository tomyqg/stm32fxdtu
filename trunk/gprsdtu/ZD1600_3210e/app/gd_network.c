#include <ucos_ii.h>
#include "gd_network.h"
#include "gd_system.h"
#include "gd_gm.h"
#include "gd_guart.h"
#include "gd_string.h"

#define NULL 0

OS_STK gd_task_network_stk[GD_TASK_NETWORK_STK_SIZE];

extern gd_system_t gd_system; 

void gd_task_network(void *parg)
{
	INT32U heartbeat_len;
	INT8S res = 0;
	INT8U err;
	gd_msg_t *msg = NULL;
	INT8U gd_heart_beat[92];

	gd_network_task_t *network_task = &gd_system.network_task;

	// Reset GPRS Module
	while(1)
	{
		res = gprsmodule_reset();
		if( res == 0)	
		{	
			break;
		}
		
		OSTimeDlyHMSM(0, 0, 0, 100);
	}

	// Initialize GPRS Module
	while(1)
	{
		res = gprsmodule_init();
		if(res == 0)
		{
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 100);
	}

	// Initialize TCP/IP 
	while(1)
	{
		res = gprs_tcpip_init("", "");
		if(res == 0)
		{
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 100);
	}

	// Initialize TCP/IP data mode
	while(1)
	{
		res = gprs_tcpip_mode_init(0x01);
		if(res == 0)
		{
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 100);
	}

	// Create TCP connection
 	while(1)
	{
		res = gprs_tcpip_creat_connection(network_task->link_info[0].link_type ,network_task->link_info[0].svr_ip, network_task->link_info[0].svr_port, 9055, 9057);
		if(res == 0)
		{
			break;
		}
			
		OSTimeDlyHMSM(0, 0, 0, 100);
	}

	// Inform guart task the connection is ready	
	msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	msg->type = GD_MSG_CONNECTION_READY;
	msg->data =  (void*)NULL;
	OSQPost(gd_system.guart_task.q_guart, (void*)msg);

	// Inform suart task the connection is ready	
	msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	msg->type = GD_MSG_CONNECTION_READY;
	msg->data =  (void*)NULL;
	OSQPost(gd_system.suart_task.q_suart, (void*)msg);

	heartbeat_len = gd_heart_beat_init(GD_DEVID, GD_DEVMAC, NULL, 0, 70, gd_heart_beat);

	while(1)
	{
		OSSemPend(gd_system.gm_operate_sem, GM_OPERATE_TIMEOUT, &err);
		if(err == OS_NO_ERR)
		{
 			res = gprs_tcpip_send(gd_heart_beat, heartbeat_len, 0);
			if(res != 0)
			{
				// Send error
			}
			err = OSSemPost(gd_system.gm_operate_sem);
		}

		OSTimeDlyHMSM(0, 0, 10, 0);
	}
	
}






