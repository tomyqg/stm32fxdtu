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
	u32 datalen;
	uart_init_3210c();
	uart_rx_itconf(COM2, DISABLE);
	rx_datalen_piont = uart_rx_bufset(COM2, rx_testbuf, 2048);
	uart_rx_itconf(COM2, ENABLE);
	memset(rx_testbuf, 'x', 2048);
	while (1) {
		if (parg) {
			uart_rx_itconf(COM2, DISABLE);
			datalen =  *rx_datalen_piont;
			*rx_datalen_piont = 0;
			memcpy(tx_testbuf, rx_testbuf, datalen);
			uart_rx_itconf(COM2, ENABLE);
			uart2_senddata(tx_testbuf, datalen);			

		}
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}
void uart2_senddata(u8 *data, u32 len);
void uart_init_3210c(void)
{
	COM_Conf_T conf;
	conf.com = COM2;
	conf.BaudRate = 115200;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = Even;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);
}




