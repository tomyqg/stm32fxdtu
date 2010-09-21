#include <ucos_ii.h>
#include "gd_suart.h"
#include "gd_system.h"
#include <stdio.h>
#include "../drives/uart_drv.h"

OS_STK gd_task_suart_stk[GD_TASK_SUART_STK_SIZE];

extern gd_system_t gd_system; 

void suart_msg_process(gd_msg_t *recv_msg)
{
	gd_msg_t 		*send_msg = NULL;
	INT8U	 		err;
	frame_node_t	*frame = NULL;
	INT32U			*len;

	switch(recv_msg->type)
	{
	case GD_MSG_RES_FRAME_READY:
		sp2gm_remove_frame((frame_node_t*)recv_msg->data);
		OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
		break;
	case GD_MSG_FRAME_READY:
		/*process data & send to serial port*/
		frame = (frame_node_t*) recv_msg->data;
		len = suart_send_data(frame->pFrame, frame->len);

		// Wait until send data finished
		while((*len) > 0)
		{
			OSTimeDlyHMSM(0, 0, 0, 5);	
		}
	
		send_msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
		send_msg->type = GD_MSG_RES_FRAME_READY;
		send_msg->data = (void*)recv_msg->data;
	
		OSQPost(gd_system.guart_task.q_guart, (void*)send_msg);
	
		OSMemPut(gd_system.gd_msg_PartitionPtr, (void*)recv_msg);
		break;
	default:
		break;
	}
	
	return;
}

void gd_task_suart(void *parg)
{
	gd_msg_t 		*msg = NULL;
	INT8U 			err;
	gd_suart_task_t *suart_task = NULL;
	INT32U 			*pRxLen;
	INT32U			rx_pre_len = 0;
	INT32U 			rx_now_len = 0;
	INT32U			rx_pre_ticks = 0;
	INT32U			rx_now_ticks = 0;
	INT32U			rx_interval = 0;

	suart_task = &gd_system.suart_task;

	memset(suart_task->rx_buf, 0, 2048);
	uart_rx_itconf(SUART, DISABLE);
	pRxLen = uart_rx_bufset(SUART, suart_task->rx_buf, 2048);
	uart_rx_itconf(SUART, ENABLE);

	while(1)
	{
		msg = (gd_msg_t *)OSQPend (suart_task->q_suart, 100, &err);

		if(msg != NULL)
			break;
		
		OSTimeDlyHMSM(0, 0, 0, 100);
	}

	if(msg->type == GD_MSG_CONNECTION_READY)
	{	
		while(1)
		{
			msg = OSQAccept(suart_task->q_suart, &err);	

			if(msg != NULL)
			{
				suart_msg_process(msg);
			}

			rx_now_len = *pRxLen;

			rx_now_ticks = OSTimeGet();

			if(rx_now_len > rx_pre_len)
			{
				rx_pre_len = rx_now_len;

				rx_pre_ticks = rx_now_ticks;
			}
			else if((rx_now_len == rx_pre_len) && rx_now_len > 0)
			{
				rx_interval = rx_now_ticks - rx_pre_ticks;

				if(rx_interval >= 100) // Frame interval
				{
					uart_rx_itconf(SUART, DISABLE);
					sp2gm_cache_frame(suart_task->rx_buf, rx_now_len);
					*pRxLen = 0;
					uart_rx_itconf(SUART, ENABLE);

					// Send msg to guart task
					msg = (gd_msg_t *)OSMemGet(gd_system.gd_msg_PartitionPtr, &err);
					msg->type = GD_MSG_FRAME_READY;
					msg->data =  (void*)gd_system.sp2gm_frame_list.head;
					OSQPost(gd_system.guart_task.q_guart, (void*)msg);
				}
			}

			OSTimeDlyHMSM(0, 0, 0, 10);
		}
	}
}

