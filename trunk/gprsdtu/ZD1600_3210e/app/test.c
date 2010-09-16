#include <ucos_ii.h>
#include "test.h"
#include "../drives/uart_drv.h"
#include <string.h>

u8	rx_testbuf[2048];
u32 *rx_datalen_piont;
u8	tx_testbuf[2048]; 

OS_STK App_TaskTestStk[APP_TASK_TEST_STK_SIZE];
void App_test(void *parg)
{
	u32 datalen = 0;
	uart_init_3210e();
	memset(rx_testbuf, 'x', 2048);
	uart_rx_itconf(COM1, DISABLE);
	rx_datalen_piont = uart_rx_bufset(COM1, rx_testbuf, 2048);
	uart_rx_itconf(COM1, ENABLE);

	while (1) 
	{
		datalen = *rx_datalen_piont;
		if (datalen) 
		{
			uart_rx_itconf(COM1, DISABLE);
			memcpy(tx_testbuf, rx_testbuf, datalen);
			*rx_datalen_piont = 0;
			uart_rx_itconf(COM1, ENABLE);
			uart1_senddata(tx_testbuf, datalen);
//			OSTimeDlyHMSM(0, 0, 1, 0);			
		}
//		uart1_senddata("test", strlen("test"));			
//		OSTimeDlyHMSM(0, 0, 1, 0);
//		uart2_senddata("usart", strlen("usart"));			
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}

void uart_init_3210e(void)
{
	COM_Conf_T conf;
	conf.com = COM1;
	conf.BaudRate = 115200;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);
}




