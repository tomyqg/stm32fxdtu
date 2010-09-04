/****************************************************************************
gprs模块底层驱动
环境：stm32f10x
20100901:实现GTM900、EM310模块GPRS控制
****************************************************************************/
#ifndef	__GPRSMODULE_H__
#define	__GPRSMODULE_H__

#include "mystring.h"


 
 
#define	 GPRS_DATA_LEN_MAX	1024
#define	 GPRS_DATA_BUFFER_LEN_MAX	(GPRS_DATA_LEN_MAX*2+40) //


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
	u8	recvdata[GPRS_DATA_LEN_MAX+40];
	u32 recvlen;
	u8	senddata[GPRS_DATA_LEN_MAX*2+20];
	u32 sendlen;
}t_gprs_databuf;
extern t_gprs_databuf	gprs_databuf;


















	 


#endif

