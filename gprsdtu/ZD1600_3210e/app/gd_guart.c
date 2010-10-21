#include <ucos_ii.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gd_guart.h"
#include "gd_gm.h"
#include "../drives/uart_drv.h"
#include "gd_system.h"
#include "gd_string.h"


OS_STK gd_task_guart_stk[GD_TASK_GUART_STK_SIZE];
OS_STK gd_task_guart_rx_stk[GD_TASK_GUART_RX_STK_SIZE];


gm2sp_frame_t gm2sp_frame;

void unrequest_at_dispose(u32 len);
void guart_recv_data_process(void);
void guart_send_data_process(frame_node_t *frame);
void guart_msg_process(gd_msg_t *recv_msg);
void gm_recvpacket_request(void);

/**********************************************************************************************
task for guart rx dispose
入口参数:
返回参数:
**********************************************************************************************/
void gd_task_guart_rx(void *parg)
{
	INT8U 	*data, *peek;
	INT32U 	len,len_c, rx_dp, rx_len;
	INT32U	*pRxLen;
	OS_CPU_SR  cpu_sr;
//	INT8S	res = 0;

	gd_msg_t *send_msg;

	gd_guart_task_t *guart_task = NULL;

	guart_task = &gd_system.guart_task;

	uart_rx_itconf(GUART, DISABLE);
	guart_rx_dp = 0;
	pRxLen = uart_rx_bufset(GUART, guart_task->rx_buf,  GUART_RX_BUF_SIZE);
	uart_rx_itconf(GUART, ENABLE);
	
	gd_msg_malloc(&send_msg);

	send_msg->type = GD_MSG_GM_RESET;
	send_msg->data = NULL;
	OSQPost(gd_system.network_task.q_network, (void*)send_msg);


	while(1)
	{
		rx_dp = guart_rx_dp;
		rx_len = *pRxLen;

		len = (rx_len >= rx_dp) ? (rx_len - rx_dp) : (GUART_RX_BUF_SIZE - rx_dp);
		data = guart_task->rx_buf + rx_dp;
		peek = check_string(data, "\r\n", len);
		if(peek)
		{		
			len = peek - data;
			if(len) 
			{
				memcpy(gprs_databuf.recvdata, data, len);

/*give gm time dispose if at return 2 commands*/
//OSTimeDly(100);
				if(gprs_databuf.recvlen == 0)
				{
					gprs_databuf.recvlen = len;		
				}
				else
				{
					//非请求结果码
//					res = gprs_unrequest_code_dispose(len);
					unrequest_at_dispose(len);
				}
 			}	
			rx_dp += len+2;
			OS_ENTER_CRITICAL();
			guart_rx_dp = rx_dp;
			OS_EXIT_CRITICAL();
			}
		else
		{
			if(rx_len < rx_dp)
			{
				data = guart_task->rx_buf;
				len = rx_len;
				peek = check_string(data, "\r\n", len);	
				if(peek)
				{
					len_c =  GUART_RX_BUF_SIZE - rx_dp;
					len =  peek - data;
					memcpy(gprs_databuf.recvdata, data+rx_dp, len_c);
					memcpy(gprs_databuf.recvdata+len_c, data, len);
					rx_dp = len;
					len += len_c;
/*give gm time dispose if at return 2 commands*/
//OSTimeDly(100);
					if(gprs_databuf.recvlen == 0)
					{
						gprs_databuf.recvlen = len;		
					}
					else
					{
						//非请求结果码
//						res = gprs_unrequest_code_dispose(len);
						unrequest_at_dispose(len);
					}
					OS_ENTER_CRITICAL();
					guart_rx_dp = rx_dp;
					OS_EXIT_CRITICAL();
	 			}	
			}
		}	
		OSTimeDlyHMSM(0, 0, 0, 20); 
	}
}

void unrequest_at_dispose(u32 len)
{
	INT8S	 res = 0;
//	INT8U 	 err;
	gd_msg_t *send_msg = NULL;
	
	res = gprs_unrequest_code_dispose(len);


	switch(res)
	{
	case GM_AT_COMMAND_OK:
		return;
	case GM_TCPIP_RECEIVED_DATA:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_RECV_DATA;
		OSQPost(gd_system.guart_task.q_guart, (void*)send_msg);
		break;
	case GM_TCPIP_RECEIVED_SMS:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_RECV_SMS;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
		break;
	case GM_TCPIP_RECEIVED_RING:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_RECV_RING;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
		break;
		
	case GM_TCPIP_LINK1_CLOSE:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_TCP_LINK1_CLOSE;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
		break;
	case GM_TCPIP_LINK2_CLOSE:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_TCP_LINK2_CLOSE;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
		break;
	case GM_TCPIP_LINK3_CLOSE:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_TCP_LINK3_CLOSE;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
		break;
	case GM_TCPIP_CLOSE:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_TCP_CLOSE;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
		break;
	case GM_TCPIP_SERVER_CLOSE:
		gd_msg_malloc(&send_msg);
		send_msg->data = NULL;
		send_msg->type = GD_MSG_GM_TCP_SERVER_CLOSE;
		OSQPost(gd_system.network_task.q_network, (void*)send_msg);
		break;
	default:
		break; 
	}
}


/**********************************************************************************************
task for guart manage
入口参数:
返回参数:
**********************************************************************************************/
void gd_task_guart(void *parg)
{
	gd_msg_t *recv_msg;
	INT8U err;
	static INT16U gm_request_index= 0;
	gm2sp_frame.frame_index = 12345;
	gm2sp_frame.packet_index = 0;
	gm2sp_frame.packet_sum = 0;
	
	// Wait for network task queue
	while(1)
	{
		recv_msg = (gd_msg_t*)OSQPend(gd_system.guart_task.q_guart, 100, &err);

		if(err == OS_NO_ERR)
		{
			if(recv_msg->type == GD_MSG_CONNECTION_READY)	
			{
			OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
				break;
			}
			OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
	 		
		}
		
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
	
	while (1) 
	{

		recv_msg = (gd_msg_t*)OSQAccept(gd_system.guart_task.q_guart, &err);

		if(recv_msg)	
		{
			guart_msg_process(recv_msg);
			/*free msg*/
			OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
		}
		
		if(gm_request_index > 200)
		{
			gm_request_index = 0;
//			gm_recvpacket_request();
		}
		gm_request_index++;
		OSTimeDlyHMSM(0, 0, 0, 20);
	}	
}



/**********************************************************************************************
Guart Message Process
入口参数:
返回参数:
**********************************************************************************************/
void guart_msg_process(gd_msg_t *recv_msg)
{
	gd_msg_t 		*send_msg = NULL;
//	INT8U 	 		err;
	
	switch(recv_msg->type)
	{
	case GD_MSG_RES_FRAME_READY:			
		break;
	case GD_MSG_FRAME_READY:
		/*process data & send to gprs*/
//		OSSemPend(gd_system.sp2gm_mem_sem, GD_SEM_TIMEOUT, &err);
		guart_send_data_process((frame_node_t*) recv_msg->data);
//		OSSemPost(gd_system.sp2gm_mem_sem);
		/*queue back*/
		gd_msg_malloc(&send_msg);
		send_msg->type = GD_MSG_RES_FRAME_READY;
		send_msg->data = (void*)recv_msg->data;
	
		OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);
	
		break;
	case GD_MSG_GM_RECV_DATA:
		guart_recv_data_process();
		break;		
	default:
		break;
	}

}
/**********************************************************************************************
gprs模块缓存数据包查询
入口参数:
返回参数:
**********************************************************************************************/
void gm_recvpacket_request(void)
{
	INT8U err = 0;
	INT8U unread =0; 
	INT8U total =0;
	INT8S res = 0;
	gd_msg_t *send_msg = NULL;
	OSSemPend(gd_system.gm_operate_sem, GD_SEM_TIMEOUT, &err);
	if(err != OS_NO_ERR)	
		return;
	res = gprs_tcpip_recvbuf_query(&unread, &total);
	if(res == 0)
	{
		if(unread >0)	
		{
			gd_msg_malloc(&send_msg);
			send_msg->data = NULL;
			send_msg->type = GD_MSG_GM_RECV_DATA;
			OSQPost(gd_system.guart_task.q_guart, (void*)send_msg);		
		
		}	
	}
	err = OSSemPost(gd_system.gm_operate_sem);

}


/**********************************************************************************************
从gprs module接收数据
入口参数:
返回参数:
**********************************************************************************************/
void guart_recv_data_process(void)
{
	INT8U 		link_num, data_index;
	gd_msg_t 	*send_msg = NULL;
	INT8S 		res = 0;
	INT8U 		err;
	gd_frame_t gd_frame;
	INT8U	dev_id[8], recv_buf[GPRS_DATA_LEN_MAX];
	INT16U	recv_len;
	

	OSSemPend(gd_system.gm2sp_buf_sem, 0, &err);
	if(err != OS_NO_ERR)	
		return;

	OSSemPend(gd_system.gm_operate_sem, GD_SEM_TIMEOUT, &err);
	if(err != OS_NO_ERR)	
		return;

	res = gprs_tcpip_request_data(0, &data_index, &link_num, &recv_len, recv_buf);

	err = OSSemPost(gd_system.gm_operate_sem);
	err = OSSemPost(gd_system.gm2sp_buf_sem);	



	/*数据处理，如合包*/
	//....
	//异常未处理
	if(res == 0)			
	{
		gd_frame.data = recv_buf;
		gd_frame.len = recv_len;
		gd_frame.dev_id = dev_id;
		gd_frame.packet_data = gm2sp_frame.buf + gm2sp_frame.frame_len;
		
		res = gd_frame_data_resolve(&gd_frame);
		if(res == 0)			
		{
			if(gd_frame.frame_index != gm2sp_frame.frame_index)
			{
				gm2sp_frame.frame_index = gd_frame.frame_index;
				if(gd_frame.packet_index != 1)	
				{
					gm2sp_frame.frame_len = 0;
					gm2sp_frame.frame_index = 0;
					return;
				}

				if(gm2sp_frame.packet_index != 0)	
				{
					memcpy(gm2sp_frame.buf, gm2sp_frame.buf+gm2sp_frame.frame_len, gd_frame.packet_len);
				}
			}
//			else
			{
				gm2sp_frame.packet_index = gd_frame.packet_index;
				gm2sp_frame.packet_sum = gd_frame.packet_sum;	
				gm2sp_frame.frame_len += gd_frame.packet_len;
			}
			if(gm2sp_frame.packet_sum == gm2sp_frame.packet_index)
			{
				gm2sp_frame.len = gm2sp_frame.frame_len;
				gm2sp_frame.packet_index = 0;
				gm2sp_frame.frame_len = 0;
				
				//gm2sp_cache_frame(buf, buf_len);
				gd_msg_malloc(&send_msg);
				send_msg->type = GD_MSG_FRAME_READY;
				send_msg->data = (void*)&gm2sp_frame;
				OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);		
			}
		}
	}
//	gprs_tcpip_recvbuf_delete(u8 index, u8 type);
	
}

/**********************************************************************************************
处理发送到gprs module的数据
入口参数:
返回参数:
**********************************************************************************************/
void guart_send_data_process(frame_node_t *frame)
{
	INT8U	err;
	INT8S 	res = 0;
	INT8U	packet_index, packet_sum;
	INT16U last_len;
	static INT16U	frame_index = 0;
	gd_frame_t gd_frame;
	
//	gd_msg_t 	*send_msg = NULL;
	
	/*分包、协议编解码*/
	packet_sum = frame->len / WS120M_FRAME_LEN;
	last_len = frame->len % WS120M_FRAME_LEN;
	if(last_len == 0)	
	{
		last_len = WS120M_FRAME_LEN;
	}
	else
	{
		packet_sum++;
	}

	gd_frame.data = gd_system.guart_task.tx_buf;
	gd_frame.dev_id = GD_DEVID;
	gd_frame.packet_sum = packet_sum;		
	gd_frame.frame_index = frame_index;

	for(packet_index=0; packet_index<packet_sum; packet_index++)
	{

		gd_frame.packet_index = packet_index+1;
		gd_frame.packet_data = frame->pFrame+(WS120M_FRAME_LEN*packet_index);
		if((packet_index+1) == packet_sum)
		{
			gd_frame.packet_len = last_len;
		}
		else
		{
			gd_frame.packet_len = WS120M_FRAME_LEN;
		}
		gd_frame_data_init(&gd_frame);
		OSSemPend(gd_system.gm_operate_sem, GD_SEM_TIMEOUT, &err);
		if(err == OS_NO_ERR)	
		{
		/************************ Test **********************************************/			
//			gm2sp_frame.len = frame->len;
//			memcpy(gm2sp_frame.buf, frame->pFrame, gm2sp_frame.len);
//		   	gd_msg_malloc(&send_msg);
//			send_msg->type = GD_MSG_FRAME_READY;
//			send_msg->data = (void*)&gm2sp_frame;
//			OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);	
		/***************************************************************************/		
			res = gprs_tcpip_send(gd_system.guart_task.tx_buf, gd_frame.len, 0);
		
			if(res !=0)
			{
			
			}
		}
		else
		{
		
		}
		err = OSSemPost(gd_system.gm_operate_sem);
	}

	frame_index++;

	if(frame_index >= 65535)	
		frame_index = 0;
	
}





