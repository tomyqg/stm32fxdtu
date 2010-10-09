#include <ucos_ii.h>
#include "gd_network.h"
#include "gd_system.h"
#include "gd_gm.h"
#include "gd_guart.h"
#include "gd_string.h"
#include "hw_core.h"

#define NULL 0

OS_STK gd_task_network_stk[GD_TASK_NETWORK_STK_SIZE];

extern gd_system_t gd_system; 


static INT8U	fail_tick = 0;
void network_gm_reset(void);
void network_gm_init(void);
void network_tcpip_init(void);
void network_tcpip_mode(void);
void network_tcpip_creat(void);
void network_heart_beat(void);

void network_tcpip_state_change(INT8U change);



void gd_task_network(void *parg)
{

	INT8S 		res = 0;
	INT8U 		err;

	gd_msg_t 	*recv_msg = NULL;
	gd_network_task_t *network_task = &gd_system.network_task;




	while(1)
	{
		recv_msg = (gd_msg_t *)OSQAccept(network_task->q_network, &err);
		fail_tick = 0;
		if(recv_msg != NULL)
		{
			switch(recv_msg->type)
			{
			case GD_MSG_GM_RESET:
				network_gm_reset();
				break;
			case GD_MSG_GM_INIT:
				network_gm_init();
				break;
			case GD_MSG_TCP_INIT:
				network_tcpip_init();
				break;
			case GD_MSG_TCP_MODE_INIT:
				network_tcpip_mode();
				break;
			case GD_MSG_TCP_CONNECT:
				network_tcpip_creat();
				break;
			case GD_MSG_SEND_HEARTBEAT:
				network_heart_beat();
				break;	
			/*unrequest at command*/
			case GD_MSG_GM_TCP_LINK1_CLOSE:
				network_tcpip_state_change(GM_TCPIP_LINK1_CLOSE);	
				break;				
			case GD_MSG_GM_TCP_LINK2_CLOSE:
				network_tcpip_state_change(GM_TCPIP_LINK2_CLOSE);	
				break;	
			case GD_MSG_GM_TCP_LINK3_CLOSE:
				network_tcpip_state_change(GM_TCPIP_LINK3_CLOSE);	
				break;	
			case GD_MSG_GM_TCP_SERVER_CLOSE:
				network_tcpip_state_change(GM_TCPIP_SERVER_CLOSE);	
				break;	
			case GD_MSG_GM_TCP_CLOSE:
				network_tcpip_state_change(GM_TCPIP_CLOSE);	
				break;	
				
			default:
				break;				
			}
			
			res = OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
			if(res != OS_ERR_NONE)	
			{
				//...error....
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 200);
	}
	
}

void network_gm_reset(void)
{
	INT8S	res = 0;
	INT8U 		err;
	gd_msg_t 	*send_msg = NULL;
	
	res = gprsmodule_reset();
	
	send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	send_msg->data =  (void*)NULL;
	if(res == 0)	
	{
		fail_tick = 0;
		send_msg->type = GD_MSG_GM_INIT;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
	}
	else
	{
		fail_tick++;
		if(fail_tick >= NETWORK_FAIL_COUNT)
		{
			fail_tick = 0;
			//system reset
			system_reset();
		}
		else
		{
			send_msg->type = GD_MSG_GM_RESET;
		}
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
	}

}
void network_gm_init(void)
{
	INT8S	res = 0;
	INT8U 		err;
	gd_msg_t 	*send_msg = NULL;
	
	res = gprsmodule_init();
	send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	send_msg->data =  (void*)NULL;				
	if(res == 0)
	{
		fail_tick = 0;				
		send_msg->type = GD_MSG_TCP_INIT;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);		
	}
	else
	{
		fail_tick++;
		if(fail_tick >= NETWORK_FAIL_COUNT)
		{
			fail_tick = 0;						
			send_msg->type = GD_MSG_GM_RESET;
		}
		else
		{
			send_msg->type = GD_MSG_GM_INIT;
		}			
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
	}


}
void network_tcpip_init(void)
{
	INT8S	res = 0;
	INT8U 		err;
	gd_msg_t 	*send_msg = NULL;
	send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	send_msg->data =  (void*)NULL;
	
	OSSemPend(gd_system.gm_operate_sem, GD_SEM_TIMEOUT, &err);
	if(err != OS_NO_ERR)	
	{
		goto TCPINITCONTINUE;	
	}

	res = gprs_tcpip_init("", "");

	if(res == 0)
	{
		fail_tick = 0;
		send_msg->type = GD_MSG_TCP_MODE_INIT;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);		
	}
	else
	{
TCPINITCONTINUE:		
		fail_tick++;
		if(fail_tick >= NETWORK_FAIL_COUNT)
		{
			fail_tick = 0;	
			send_msg->type = GD_MSG_GM_RESET;
		}
		else
		{
			send_msg->type = GD_MSG_TCP_INIT;
		}	
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
	}

	OSSemPost(gd_system.gm_operate_sem);


}
void network_tcpip_mode(void)
{
	INT8S	res = 0;
	INT8U 		err;
	gd_msg_t 	*send_msg = NULL;
	
	res = gprs_tcpip_mode_init(0x01);
	send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	send_msg->data =  (void*)NULL;
	if(res == 0)
	{
		fail_tick = 0;
		gd_system.state = GD_STATE_STANDBY;
		send_msg->type = GD_MSG_TCP_CONNECT;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
	}
	else
	{
		fail_tick++;
		if(fail_tick >= NETWORK_FAIL_COUNT)
		{
			fail_tick = 0;	
			send_msg->type = GD_MSG_GM_RESET;
		}
		else
		{
			send_msg->type = GD_MSG_TCP_MODE_INIT;
		}	
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
	}	

}
void network_tcpip_creat(void)
{
	INT8S		res = 0;
	INT8U 		err;
	gd_msg_t 	*send_msg = NULL;
	INT8U 		i = 0;

	gd_network_task_t *network_task = &gd_system.network_task;

	

	for(i=0; i<network_task->link_count; i++)
	{
		if(network_task->link_info[i].link_state == GD_LINK_STATE_IDLE)
		{
			res = gprs_tcpip_creat_connection(network_task->link_info[i].link_type ,network_task->link_info[i].svr_ip, network_task->link_info[i].svr_port, 9055, 9057);
			if(res == 0)
			{
				fail_tick = 0;
				network_task->link_info[i].link_state = GD_LINK_STATE_ONLINE;
			}
			else
			{
				fail_tick++;
				if(fail_tick >= (NETWORK_FAIL_COUNT * network_task->link_count))
				{
					fail_tick = 0;
					send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
					send_msg->data =  (void*)NULL;
					send_msg->type = GD_MSG_GM_RESET;
					OSQPost(gd_system.network_task.q_network, (void*)send_msg);
					return;
				}
				network_task->link_info[i].link_state = GD_LINK_STATE_IDLE;
			}
		}
	}

	for(i=0; i<network_task->link_count; i++)
	{
		if(network_task->link_info[i].link_state == GD_LINK_STATE_IDLE)
		{
			send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
			send_msg->type = GD_MSG_TCP_CONNECT;
			send_msg->data =  (void*)NULL;
			OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
			return ;	
		}
	}
	
	if(gd_system.state == GD_STATE_STANDBY)
	{
		// Inform guart task the connection is ready	
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_CONNECTION_READY;
		send_msg->data =  (void*)NULL;
		OSQPost(gd_system.guart_task.q_guart, (void*)send_msg);
		
		// Inform suart task the connection is ready	
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_CONNECTION_READY;
		send_msg->data =  (void*)NULL;
		OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);

		gd_system.state = GD_STATE_ONLINE;
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_SEND_HEARTBEAT;
		send_msg->data =  (void*)NULL;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);	
	}


}
void network_heart_beat(void)
{
	INT8S	res = 0;
	static INT8U		counter = 0;
	INT32U 		heartbeat_len;
	INT8U 		err;
	INT8U 		gd_heart_beat[92];
	INT8U 		i = 0;

	gd_msg_t 	*send_msg = NULL;
	gd_network_task_t *network_task = &gd_system.network_task;
	

	if(gd_system.state == GD_STATE_ONLINE)
	{
		counter++;
		
		if(counter > 50)
		{
			heartbeat_len = gd_heart_beat_init(GD_DEVID, GD_DEVMAC, NULL, 0, 70, gd_heart_beat);
			OSSemPend(gd_system.gm_operate_sem, GD_SEM_TIMEOUT, &err);
			if(err == OS_NO_ERR)
			{
				for(i=0; i<network_task->link_count; i++)
				{
					res = gprs_tcpip_send(gd_heart_beat, heartbeat_len, i);
					if(res == 0)
					{
						fail_tick = 0;								
					}
					else
					{
						// Send error
						fail_tick++;
						if(fail_tick >= (NETWORK_FAIL_COUNT * network_task->link_count))
						{
							fail_tick = 0;
							send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
							send_msg->data =  (void*)NULL;
							send_msg->type = GD_MSG_GM_RESET;
							OSQPost(gd_system.network_task.q_network, (void*)send_msg);
							break;
						}
					}
				}
				err = OSSemPost(gd_system.gm_operate_sem);
			}
			counter = 0;
		}
		
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_SEND_HEARTBEAT;
		send_msg->data =  (void*)NULL;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
				
	}
	
}


void network_tcpip_state_change(INT8U change)
{
//	INT8S	res = 0;
	INT8U 		err;
	gd_msg_t 	*send_msg = NULL;
	gd_network_task_t *network_task = &gd_system.network_task;	

	switch(change)
	{
	case GM_TCPIP_LINK1_CLOSE:
		network_task->link_info[0].link_state = GD_LINK_STATE_IDLE;
		gd_system.state = GD_STATE_STANDBY;
		break;
	case GM_TCPIP_LINK2_CLOSE:
		network_task->link_info[1].link_state = GD_LINK_STATE_IDLE;
		gd_system.state = GD_STATE_STANDBY;
		break;
	case GM_TCPIP_LINK3_CLOSE:
		network_task->link_info[2].link_state = GD_LINK_STATE_IDLE;
		gd_system.state = GD_STATE_STANDBY;
		break;
	case GM_TCPIP_SERVER_CLOSE:
		/*服务器功能暂未使用*/
		return;
	case GM_TCPIP_CLOSE:
		network_task->link_info[0].link_state = GD_LINK_STATE_IDLE;
		network_task->link_info[1].link_state = GD_LINK_STATE_IDLE;
		network_task->link_info[2].link_state = GD_LINK_STATE_IDLE;
		gd_system.state = GD_STATE_IDLE;
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->data =  (void*)NULL;
		send_msg->type = GD_MSG_GM_RESET;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);		
		return;
			
	default:
		return;
	}
	
	send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
	send_msg->data =  (void*)NULL;
	send_msg->type = GD_MSG_TCP_CONNECT;
	OSQPost(gd_system.network_task.q_network, (void*)send_msg);		

}








