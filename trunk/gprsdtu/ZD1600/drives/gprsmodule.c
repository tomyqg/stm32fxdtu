
#include <string.h>
#include "gprsmodule.h"



t_gprs_databuf	gprs_databuf;


u8 GPRS_TCPIP_IOMODE;


s8	at_send_data(u8 *data, u16 len)//send data
{
	u16	i = 0;
	while(i<len)
	{
//		usart_send_char(data[i++]);//

	}
	return 0;
}
//接收改为中断方式 通过gprs_databuf
/*
s8	at_return_data(u8 *buf, u16 len)//receive data
{
	u16	i = 0;
	while(i<len)
	{
		usart_receive_char(data[i++]);//
  	}
	return 0;
}
*/


//at
s8	at_at(void)								//测试串口
{
    at_send_data("AT\r", sizeof("AT\r"));
 	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
	else	return -1;

}

s8	at_ate0(void)                             //关闭回显
{
    at_send_data("ATE0\r", strlen("ATE0\r"));
 	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
	else	return -1;
}
s8	at_ate1(void)                             //打开回显
{
	at_send_data("ATE1\r", strlen("ATE1\r"));
 	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
	else	return -1;
}
s8	at_tsim(void)                             //sim	 检测到sim卡返回0
{
	at_send_data("AT%TSIM\r", strlen("AT%TSIM\r"));
 	while(gprs_databuf.recvlen < 7);
	if(check_string(gprs_databuf.recvdata, "1", 7))	return 0;
 	else	return -1;
}

/*****************************************
测试信号强度
入口参数：无
返回参数：信号强度值 0 ~ 31
*******************************************/
s8	at_csq(void)                            
{
	u8  *p;
    at_send_data("AT+CSQ\r", strlen("AT+CSQ\r"));
 	while(gprs_databuf.recvlen < 16);
	if(check_string(gprs_databuf.recvdata, "OK", 16) == NULL)	return -1;
	p =  check_string(gprs_databuf.recvdata, ",", 16);
	if(p)	return (*(p-2)*10 + *(p-1));
	else	return -1;
}
/*****************************************
设置ip数据包模式
入口参数： mode	低三位分别对应(从底到高)数据转换、链接、缓存
返回参数： 正确返回0
*******************************************/
s8	at_iomode(u8 mode)                    
{
	u8 buf[17];
	sprintf(buf, "AT\%IOMODE=%d,%d,%d\r", mode&0x01, (1+(mode&0x02)>>1), (mode&0x04)>>2);
	at_send_data(buf, strlen(buf));
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
 	else	return -1;
}
/*****************************************
查询ip数据包模式设置
入口参数： 
返回参数： 成功返回mode	低三位分别对应数据转换、链接、缓存
*******************************************/
u8	at_iomode_check(void)                    
{
	u8 mode = 0;
	u8 *p;
	at_send_data("AT%IOMODE?\r", strlen("AT%IOMODE?\r"));
	while(gprs_databuf.recvlen < 17);
	if(check_string(gprs_databuf.recvdata, "OK", 17) == NULL)	return 8;
	//数据转换
	p = check_string(gprs_databuf.recvdata, ",", 17);
	if(p == NULL) return 9;
	if(*(p-1) == '1') mode |= 0x01;
	else if(*(p-1) != '0') return 10;
	//链接
	p = check_string(p+1, ",", 17);
	if(p == NULL) return 11;
	if(*(p-1) == '2') mode |= 0x02;
	else if(*(p-1) != '1') return 12;
	//缓存
	p = check_string(p+1, ",", 17);
	if(p == NULL) return 13;
	if(*(p-1) == '1') mode |= 0x04;
	else if(*(p-1) != '0') return 14;

	return mode;
 	
}



/*=======================================================*/	 
/********************************************************
//模块初始化 
成功返回
********************************************************/

s8	gprsmodules_init(void)
{
	s8 res = 0;
	//模块启动\重启过后需等待6s
	res = at_at();
	if(res != 0) return 1;
	res = at_ate0();
	if(res != 0) return 2;
	res = at_tsim();
	if(res != 0) return 3;
	//AT+CREG
	at_send_data("AT+COPS?\r", strlen("AT+COPS?\r"));
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3) == NULL)	return 4;
	//AT+COPS?
	at_send_data("AT+COPS?\r", strlen("AT+COPS?\r"));
	while(gprs_databuf.recvlen < 28);
	if(check_string(gprs_databuf.recvdata, "CHINA MOBILE", 28) == NULL)	return 5;
	//AT+CGATT?
	at_send_data("AT+CGATT?\r", strlen("AT+CGATT?\r"));
	while(gprs_databuf.recvlen < 13);
	if(check_string(gprs_databuf.recvdata, "1", 13) == NULL)	return 5;  

	return 0;
}

/*****************************************
tcp/ip初始化
入口参数：user 用户名 长度最大10
		  password 密码	长度最大10
		  可以为空 
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_init(u8 *user, u8 *password)                    
{
	u8 len = 0;
	u8 buf[40];
	if(strlen(user)>10 || strlen(password))	return 1;
	//配置apn
	at_send_data("AT+CGDCONT=1,\"IP\",\"CMNET\"", strlen("AT+CGDCONT=1,\"IP\",\"CMNET\""));
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3) == NULL)	return 1;
	//tcpip enable
	strcpy(buf, "AT%ETCPIP");
	len = strlen(buf);
	if(user)
	{
		buf[len++] = '=';
		buf[len++] = '\"';
		strcpy(buf+len, user);
		len += strlen(user);
		buf[len++] = '\"';
		buf[len++] = ',';
		buf[len++] = '\"';
		strcpy(buf+len, password);
		buf[len++] = '\"';
		len = strlen(buf);	
	}
	buf[len++] = '\r';
	buf[len++] = '\0'; 
	at_send_data(buf, len);
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
 	else	return 2;
}

/*****************************************
设置tcpip数据帧模式
并在GPRS_TCPIP_IOMODE中保存当前模式设置
当前使用缓存
入口参数： mode	低三位分别对应(从底到高)数据转换、链接、缓存
返回参数： 正确返回0
*******************************************/
s8	gprs_tcpip_mode_init(u8 mode)
{
	s8 res;
	if(mode>=8)	return 1;
	res = at_iomode(mode);
	if(res != 0) return 2;
	GPRS_TCPIP_IOMODE = at_iomode_check();
	if(GPRS_TCPIP_IOMODE != mode)	return 3;
	return 0;
}

/*****************************************
tcp/ip创建一条链接	（单链接模式）
入口参数：type 0-tcp, 1-udp 
		  ip ip地址，长度不超过15
		  dest_port 目标端口 最高5位
		  udp_dest_port udp对端 最高5位
		  local_port 本地端口 最高5位
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_creat_connection(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port)                    
{
	u8	buf[80];
	u8  len = 0;
	if(strlen(ip)>15)	return 1;
	strcpy(buf, "AT%IPOPEN=");
	if(type == 1)	strcat(buf, "\"UDP\"");	
	else if(type == 0)	strcat(buf, "\"TCP\"");
	else return 2;
	len = strlen(buf);
	sprintf(buf+len,",\"%s\",%d,%d,%d\r", ip, dest_port, udp_dest_port, local_port);
	len = strlen(buf);
	at_send_data(buf, len);
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
 	else	return 3;	 


}
/*****************************************
tcp/ip创建一条链接	（多链接模式）
入口参数：type 0-tcp, 1-udp 
		  ip ip地址，长度不超过15
		  dest_port 目标端口 最高5位
		  local_port 本地端口 最高5位
		  udp_dest_port udp对端 最高5位
		  link_num 链接序号 
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_creat_connection_n(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port, u8 link_num)                    
{
	u8	buf[80];
	u8  len = 0;
	if(strlen(ip)>15)	return 1;
	strcpy(buf, "AT%IPOPEN=");
	if(link_num=0 || link_num>3)	return 2;
	len = strlen(buf);
	sprintf(buf+len,"%d,", link_num);
	if(type == 1)	strcat(buf, "\"UDP\"");	
	else if(type == 0)	strcat(buf, "\"TCP\"");
	else return 3;
	len = strlen(buf);
	sprintf(buf+len,",\"%s\",%d,%d,%d\r", ip, dest_port, udp_dest_port, local_port);
	len = strlen(buf);
	at_send_data(buf, len);
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
 	else	return 4;	 

}
/*****************************************
tcp/ip关闭链接、注销gprs、关闭服务器	 
入口参数：link_num 1~3链接序号 5注销gprs 6关闭服务器
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_close_connection(u8 link_num)
{
	u8 buf[15];
	u8 len = 0;
	sprintf(buf,"AT\%IPCLOSE=%d,", link_num);
	len = strlen(buf);
	at_send_data(buf, len);
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
 	else	return 1;

} 

/*****************************************
tcp/ip 数据发送
入口参数：data 数据buffer地址 
		  len  长度	最大GPRS_DATA_LEN_MAX
		  link_num 多连接模式时链接序号 单链接模式不起作用，可赋值0 
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_send(u8 *data, u16 len, u8 link_num)
{
	u8 databuf[GPRS_DATA_LEN_MAX*2+15];
	u16 databuf_len;
//	memset(buf, 0, GPRS_DATA_LEN_MAX*2);
	if(len > GPRS_DATA_LEN_MAX)  return 1;
	//选择单\多链接，发送
	if(GPRS_TCPIP_IOMODE & 0x02)
	{
		strcpy(databuf, "AT%IPSENDX=");
	}
	else
	{
		strcpy(databuf, "AT%IPSEND=");
	}
	databuf_len = strlen(databuf);
	//数据转换
	if(GPRS_TCPIP_IOMODE & 0x01)
	{
		databuf_len += hex_to_ascii(data, databuf+databuf_len, len);	
	}
	else
	{
	 	memcpy(databuf+databuf_len, data, len);
		databuf_len += len;
	}
	databuf[databuf_len++] = '\r';	
	databuf[databuf_len++] = '\0';
	at_send_data(databuf, databuf_len);
	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
 	else	return 2;

}



/*****************************************
tcp/ip 数据接收(使用缓冲)
	   数据查询 接收 删除
*******************************************/

/*****************************************
tcp/ip 数据接收缓冲区查询
入口参数：unread 用于返回未读数据包数
		  total	 用于返回缓冲区数据包总数
出口参数：
*******************************************/
s8	gprs_tcpip_recvbuf_query(u8 *unread, u8 *total)
{
	u8	buf[30];
	u8	*pF, *pB;
	strcpy(buf, "AT%IPDQ\r");
	at_send_data(buf, strlen(buf));
	while(gprs_databuf.recvlen < 20);
	pF = check_string(gprs_databuf.recvdata, ":", 15);
	pB = check_string(gprs_databuf.recvdata, ",", 20);
	if(~(pF&&pB)) return 2;
	*unread =  char_to_int(pF, pB-pF);
	*unread =  char_to_int(pF, 2);
	pF = check_string(gprs_databuf.recvdata, ",", 15);
	pB = check_string(gprs_databuf.recvdata, "OK", 20);
	if(~(pF&&pB)) return 3;
	*total =  char_to_int(pF, pB-pF);
	return 0;
}

/*****************************************
tcp/ip 数据接收缓冲区删除
入口参数： index 数据包序号0-50
		   type 类型 0、1、2 在index=0时有效 0删已读 1删未读 2全部
出口参数： 成功返回删除的数据包数量 错误返回值<0
*******************************************/
s8	gprs_tcpip_recvbuf_delete(u8 index, u8 type)
{
	u8 buf[17], *pF, *pB;
	if(index>50 || type>2)	return -1;	
	sprintf(buf,"AT\%IPDD=%d,%d", index, type);
	at_send_data(buf, strlen(buf));
	while(gprs_databuf.recvlen < 17);
	pB =  check_string(buf, "OK", 17);
	pF =  check_string(buf, ":", 17);
	if(~(pF&&pB)) return -2;
	return char_to_int(pF, pB-pF);
	
}

/**********************************************
tcp/ip 数据接收缓冲区删除模式不设置
		采用默认：自动删除
**********************************************/

