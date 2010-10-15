
/****************************************************************************
gprs模块底层驱动
环境：stm32f10x
20100901:实现GTM900、EM310模块GPRS控制
****************************************************************************/
#ifndef GD_GM_INIT_H
#define GD_GM_INIT_H

#include "gd_string.h"


/*非请求结果码处理结果*/
#define GM_AT_COMMAND_OK	0
#define GM_TCPIP_RECEIVED_DATA	1
#define GM_TCPIP_LINK1_CLOSE	2
#define GM_TCPIP_LINK2_CLOSE	3
#define GM_TCPIP_LINK3_CLOSE	4
#define GM_TCPIP_CLOSE			6
#define GM_TCPIP_SERVER_CLOSE	7

 
#define	 GPRS_DATA_LEN_MAX	1024
#define	 GPRS_DATA_BUFFER_SIZE	(GPRS_DATA_LEN_MAX*2+30) 


/**************************************************
用于保存tcp ip数据帧格式
GTM900C_TCPIP_IOMODE:	
0 0 0 0  0 x x x
_ _ _ _  _ _ _ _
		   | | |___0: 不对数据进行转换   1:对数据进行转换 	默认：0
		   | |___1:当前使用单链接AT命令	 2:当前使用多链接AT命令	  默认：1
		   |___ 0:使用接收缓存	1：不使用接收缓存	默认：1

在初始化后用s8	at_iomode(u8 mode)设置
可用u8	at_iomode_check(void)查询				   
								      
**************************************************/
//extern u8 GPRS_TCPIP_IOMODE;



/**************************************************
GPRS接收缓冲区  用于缓存来自模块的数据
**************************************************/
typedef struct __gprs_databuf
{
	u8	recvdata[GPRS_DATA_BUFFER_SIZE];
	u32 recvlen;
	u8	senddata[GPRS_DATA_BUFFER_SIZE];
	u32 sendlen;
}gprs_databuf_t;
extern gprs_databuf_t	gprs_databuf;


s8	at_csq(void);            
void	gprsmodule_shutdown(void);
s8 gprsmodule_reset(void);
s8	gprs_bps_set(u32 rate);
s8	gprsmodule_init(void);
s8	gprs_tcpip_init(u8 *user, u8 *password);
s8	gprs_tcpip_mode_init(u8 mode);
s8	gprs_tcpip_creat_connection(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port);
s8	gprs_tcpip_creat_connection_n(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port, u8 link_num);
s8	gprs_tcpip_close_connection(u8 link_num);
s8	gprs_tcpip_send(u8 *data, u16 len, u8 link_num);
s8	gprs_tcpip_recvbuf_query(u8 *unread, u8 *total);
s8	gprs_tcpip_request_data(u8 index, u8 *data_index_p, u8 *link_num_p, u16 *data_len_p, u8 *recvdata);
s8	gprs_tcpip_recvbuf_delete(u8 index, u8 type);


s8	gprs_unrequest_code_dispose(u32 len);


















	 


#endif




