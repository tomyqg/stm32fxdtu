/****************************************************************************
gprsģ��ײ�����
������stm32f10x
20100901:ʵ��GTM900��EM310ģ��GPRS����
****************************************************************************/
#ifndef	__GPRSMODULE_H__
#define	__GPRSMODULE_H__

#include "mystring.h"


 
 
#define	 GPRS_DATA_LEN_MAX	1024
#define	 GPRS_DATA_BUFFER_LEN_MAX	(GPRS_DATA_LEN_MAX*2+40) //


/**************************************************
���ڱ���tcp ip����֡��ʽ
GTM900C_TCPIP_IOMODE:	
0 0 0 0  0 x x x
_ _ _ _  _ _ _ _
		   | | |___0: �������ݽ���ת��   1:�����ݽ���ת�� 	Ĭ�ϣ�0
		   | |___1:��ǰʹ�õ�����AT����	 2:��ǰʹ�ö�����AT����	  Ĭ�ϣ�1
		   |___ 0:ʹ�ý��ջ���	1����ʹ�ý��ջ���	Ĭ�ϣ�1

�ڳ�ʼ������s8	at_iomode(u8 mode)����
����u8	at_iomode_check(void)��ѯ				   
								      
**************************************************/
//extern u8 GPRS_TCPIP_IOMODE;



/**************************************************
GPRS���ջ�����  ���ڻ�������ģ�������
**************************************************/
typedef struct __gprs_databuf
{
	u8	recvdata[GPRS_DATA_LEN_MAX+40];
	u32 recvlen;
	u8	senddata[GPRS_DATA_LEN_MAX*2+20];
	u32 sendlen;
}t_gprs_databuf;
extern t_gprs_databuf	gprs_databuf;


s8	gprsmodules_init(void);
s8	gprs_tcpip_init(u8 *user, u8 *password);
s8	gprs_tcpip_mode_init(u8 mode);
s8	gprs_tcpip_creat_connection(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port);
s8	gprs_tcpip_creat_connection_n(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port, u8 link_num);
s8	gprs_tcpip_close_connection(u8 link_num);
s8	gprs_tcpip_send(u8 *data, u16 len, u8 link_num);
s8	gprs_tcpip_recvbuf_query(u8 *unread, u8 *total);
s8	gprs_tcpip_recvbuf_delete(u8 index, u8 type);


















	 


#endif

