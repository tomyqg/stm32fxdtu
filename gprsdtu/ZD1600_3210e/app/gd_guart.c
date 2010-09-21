#include <ucos_ii.h>
#include <stdlib.h>
#include "gd_guart.h"
#include "gd_gm.h"
#include "../drives/uart_drv.h"
#include "gd_system.h"
#include "gd_string.h"


OS_STK gd_task_guart_stk[GD_TASK_GUART_STK_SIZE];
OS_STK gd_task_guart_rx_stk[GD_TASK_GUART_RX_STK_SIZE];

void unrequest_at_dispose(u32 len);
void guart_recv_data_process(void);
void guart_send_data_process(frame_node_t *frame);
void guart_msg_process(gd_msg_t *recv_msg);


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

	gd_guart_task_t *guart_task = NULL;

	guart_task = &gd_system.guart_task;

	uart_rx_itconf(GUART, DISABLE);
	guart_rx_dp = 0;
	pRxLen = uart_rx_bufset(GUART, guart_task->rx_buf,  GUART_RX_BUF_SIZE);
	uart_rx_itconf(GUART, ENABLE);


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
	INT8S	res = 0;
	INT8U 	 		err;
	gd_msg_t *send_msg = NULL;
	res = gprs_unrequest_code_dispose(len);


	switch(res)
	{
	case GM_TCPIP_RECIEVED_DATA:
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_GM_RECV_DATA;
		send_msg->data = NULL;
		OSQPost(gd_system.guart_task.q_guart, (void*)send_msg);
		break;
	case GM_TCPIP_LINK1_CLOSE:

		break;
	case GM_TCPIP_LINK2_CLOSE:

		break;
	case GM_TCPIP_LINK3_CLOSE:

		break;
	case GM_TCPIP_CLOSE:

		break;
	case GM_TCPIP_SERVER_CLOSE:

		break;
	default:
		return; 
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
	
	// Wait for network task queue
	while(1)
	{
		recv_msg = (gd_msg_t*)OSQPend(gd_system.guart_task.q_guart, 100, &err);

		if(recv_msg->type == GD_MSG_CONNECTION_READY)	
		{
			break;
		}
		
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
	
	while (1) 
	{
		recv_msg = (gd_msg_t*)OSQAccept(gd_system.guart_task.q_guart, &err);

		if(recv_msg)	
		{
			guart_msg_process(recv_msg);
		}
		
		// recv data process
//		guart_recv_data_process();

		OSTimeDlyHMSM(0, 0, 0, 10);

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
	INT8U 	 		err;
	
	switch(recv_msg->type)
	{
	case GD_MSG_RES_FRAME_READY:			
		break;
	case GD_MSG_FRAME_READY:
		/*process data & send to gprs*/
		guart_send_data_process((frame_node_t*) recv_msg->data);
		/*queue back*/
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_RES_FRAME_READY;
		send_msg->data = (void*)recv_msg->data;
	
		OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);
		/*free msg*/
		OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
		break;
	case GD_MSG_GM_RECV_DATA:
		guart_recv_data_process();
		OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
		break;		
	default:
		break;
	}

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
	INT8U		buf[256];
	INT16U		buf_len = 0;
	
	OSSemPend(gd_system.gm_operate_sem, GM_OPERATE_TIMEOUT, &err);

	if(err != OS_NO_ERR)	
		return;

	res = gprs_tcpip_request_data(0, &data_index, &link_num, &buf_len, buf);

	err = OSSemPost(gd_system.gm_operate_sem);


	/*数据处理，如合包*/
	//....
	if(res == 0)	
	{
		gm2sp_cache_frame(buf, buf_len);
	   	send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_FRAME_READY;
		send_msg->data = (void*)gd_system.gm2sp_frame_list.head;
		OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);		
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
	INT32U 	len = 0;
	volatile INT8U	err;
	volatile INT8S 	res = 0;

//	if(frame->len > GPRS_DATA_LEN_MAX)	
	{
		/*分包、协议编解码*/
		//...


	 	len = gd_gm_data_init(GD_DEVID, 1, frame->pFrame, frame->len, gd_system.guart_task.tx_buf);
	}
	

	OSSemPend(gd_system.gm_operate_sem, GM_OPERATE_TIMEOUT, &err);

	if(err != OS_NO_ERR)	
		return;

	res = gprs_tcpip_send(gd_system.guart_task.tx_buf, len, 0);

	err = OSSemPost(gd_system.gm_operate_sem);
	
}





