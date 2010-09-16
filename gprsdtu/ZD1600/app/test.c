#include <ucos_ii.h>
#include "test.h"
#include "../drives/uart_drv.h"
#include <string.h>
#include "gd_gm.h"



OS_STK App_TaskTestStk[APP_TASK_TEST_STK_SIZE];
void App_test(void *parg)
{
	u8 datalen, total;
	uart_init_3210c();
//	memset(rx_testbuf, 'x', 2048);
	uart_rx_itconf(COM2, DISABLE);
//	gprs_databuf.recvlen = uart_rx_bufset(COM2, gprs_databuf.recvdata,  GPRS_DATA_BUFFER_SIZE);
	uart_rx_itconf(COM2, ENABLE);

	while (1) 
	{
	
		gprsmodule_init();
		gprs_tcpip_init("usertest", "password");
		gprs_tcpip_mode_init(0x03);
		gprs_tcpip_creat_connection(0, "192.168.101.111", 9050, 9353, 9444);
	//	gprs_tcpip_creat_connection_n(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port, u8 link_num);
		gprs_tcpip_close_connection(2);
		gprs_tcpip_send("send data test", strlen("send data test"), 1);
		gprs_tcpip_recvbuf_query(&datalen, &total);
		gprs_tcpip_recvbuf_delete(datalen, total);

	OSTimeDlyHMSM(0, 0, 1, 0);
	}
}

void uart_init_3210c(void)
{
	COM_Conf_T conf;
	conf.com = COM2;
	conf.BaudRate = 115200;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);
}





