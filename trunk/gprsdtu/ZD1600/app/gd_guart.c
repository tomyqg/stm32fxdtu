#include <ucos_ii.h>
#include <stdlib.h>
#include "gd_guart.h"
#include "gd_gm.h"
#include "../drives/uart_drv.h"
#include "gd_system.h"
#include "mystring.h"


#define GUART COM2
#define GD_GUART_RXBUF_SIZE	2048
#define GD_GUART2SUART_BUF_SIZE	1024

INT8U	gd_guart_rxbuf[GD_GUART_RXBUF_SIZE];
INT8U	gd_guart2suart_buf[GD_GUART2SUART_BUF_SIZE];
INT32U	*gd_guart2suart_send_len;
INT32U	*gd_guart_rxlen_p;

OS_STK gd_task_guart_stk[GD_TASK_GUART_STK_SIZE];
OS_STK gd_task_guart_rx_stk[GD_TASK_GUART_RX_STK_SIZE];

INT8U	gd_guart_ready = 1;	  //used to tell that the guart is ready;



void guart_recv_data_dispose(void);
void guart_send_data_dispose(frame_node_t *frame);




/**********************************************************************************************
task for guart rx dispose
入口参数:
返回参数:
**********************************************************************************************/
void gd_task_guart_rx(void *parg)
{
	INT8U *data, *peek;
	INT32U len;
	gd_guart_ready = 1;
	uart_rx_itconf(GUART, DISABLE);
	gd_guart_rxdp = 0;
	gd_guart_rxlen_p= uart_rx_bufset(GUART, gd_guart_rxbuf,  GD_GUART_RXBUF_SIZE);
	uart_rx_itconf(GUART, ENABLE);



	while(1)
	{
		data = gd_guart_rxbuf + gd_guart_rxdp;
		len = (*gd_guart_rxlen_p >= gd_guart_rxdp)?(*gd_guart_rxlen_p - gd_guart_rxdp):(GD_GUART_RXBUF_SIZE - gd_guart_rxdp + *gd_guart_rxlen_p);

		peek = check_string(data, "\r\n", len);
//		peek = check_string(data, "\r", len);
		if(peek)
		{
			len = peek - data;
			if(len) 
			{
/*//mode 1		while(gprs_databuf.recvlen)	
					OSTimeDlyHMSM(0, 0, 0, 1);
				memcpy(gprs_databuf.recvdata, data, len);
				gprs_databuf.recvlen = len;
*/				if(gprs_databuf.recvlen == 0)
				{
					memcpy(gprs_databuf.recvdata, data, len);
					gprs_databuf.recvlen = len;		
				}
				else
				{
					//非请求结果码
					//...
				}
/**/
 			}
			gd_guart_rxdp += len+2;

		}
		OSTimeDlyHMSM(0, 0, 0, 20); 
	}

}
/**********************************************************************************************
task for guart manage
入口参数:
返回参数:
**********************************************************************************************/
void gd_task_guart(void *parg)
{
	gd_msg_t *recv_msg, *send_msg;
	INT8U err;
	gd_guart2suart_send_len = 0;


	/*tell network task start*/
	for(;;)
	{
		if(gprsmodule_reset() == 0)	
		{
			gd_guart_ready = 0;			
			break;
		}
		OSTimeDlyHMSM(0, 0, 5, 0);
	}
	/*wait for network task queue*/
	for(;;)
	{
		recv_msg = (gd_msg_t*)OSQPend(gd_system.guart_task.q_guart, 0, &err);
		if(recv_msg->type == GD_MSG_CONNECTION_READY)	
		{
			break;
		}
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
	/*task begin*/
	while (1) 
	{
		recv_msg = (gd_msg_t*)OSQAccept(gd_system.suart_task.q_suart, &err);
		if(recv_msg)	
		{
			switch(recv_msg->type)
			{
			case GD_MSG_RES_FRAME_READY:			
				break;
			case GD_MSG_FRAME_READY:
				/*dispose data & send to gprs*/
				guart_send_data_dispose((frame_node_t*) recv_msg->data);
				/*queue back*/
				send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
				send_msg->type = GD_MSG_RES_FRAME_READY;
				send_msg->data = (void*)recv_msg->data;

				OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);

				OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
 				break;
			default:
				break;
			}
		}
		/*recv data dispose*/
		guart_recv_data_dispose();

		
	OSTimeDlyHMSM(0, 0, 0, 50);
	}	
	
//	OSTaskDel(OS_PRIO_SELF);
}


/**********************************************************************************************
从gprs module接收数据
入口参数:
返回参数:
**********************************************************************************************/
void guart_recv_data_dispose(void)
{
	INT8U link_num, data_index;
	INT16U	data_len;
	gd_msg_t *send_msg = NULL;
	INT8S res = 0;
	INT8U 			err;
	
//	res = gprs_tcpip_recvbuf_query(unread_num, total_num);
	OSSemPend(gd_system.gm_operate_sem, GM_OPERATE_TIMEOUT, &err);
	if(err != OS_NO_ERR)	return;

	res = gprs_tcpip_request_data(0, &data_index, &link_num, &data_len, gd_guart2suart_buf);

	err = OSSemPost(gd_system.gm_operate_sem);

	gm2sp_cache_frame(gd_guart2suart_buf, data_len);
	/*数据处理，如合包*/
	//....
	if(res == 0)	
	{
	   	send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_FRAME_READY;
		send_msg->data = (void*)gd_system.gm2sp_frame_list.head;
		OSQPost(gd_system.suart_task.q_suart, (void*)send_msg);		
	}
//	gprs_tcpip_recvbuf_delete(u8 index, u8 type);
	
}
/**********************************************************************************************
处理发送到gprs module的数据(中断接收)
入口参数:
返回参数:
**********************************************************************************************/
void guart_send_data_dispose(frame_node_t *frame)
{
	INT32U len, i;
	INT8U	err;
	INT8S res = 1;
	len = frame->len;
	if(len > GPRS_DATA_LEN_MAX)	
	{
		/*分包*/
		
	}
	else		
	{
		OSSemPend(gd_system.gm_operate_sem, GM_OPERATE_TIMEOUT, &err);
		if(err != OS_NO_ERR)	return;

		res = gprs_tcpip_send(frame->pFrame, len, 0);

		err = OSSemPost(gd_system.gm_operate_sem);
	}
	
}







