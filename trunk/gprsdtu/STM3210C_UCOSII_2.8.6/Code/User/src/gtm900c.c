/*
*********************************************************************************************************
*	华为GTM900C模块AT命令                                             
*
*********************************************************************************************************
*/


/* Includes ------------------------------------------------------------------*/

#include "gtm900c.h"
#include <string.h>
#include <stdio.h>

#define	 GTM900C_TCPIP_SEND_DATALEN_MAX	1024

/**************************************************
用于保存tcp ip数据帧格式
GTM900C_TCPIP_IOMODE:	0 0 0 0  0 x x x
 						_ _ _ _  _ _ _ _
								   | | |___0: 不对数据进行转换   1:对数据进行转换 	默认：0
								   | |___1:当前使用单链接AT命令	 2:当前使用多链接AT命令	  默认：1
								   |___ 0:使用接收缓存	1：不使用接收缓存	默认：1

在初始化后用s8	at_iomode(u8 mode)设置
可用u8	at_iomode_check(void)查询				   
								      
**************************************************/
u8 GTM900C_TCPIP_IOMODE;


/**************************************************
在字符串str前len字节中查找指定字符串flg
返回str中flg起始位置
没有返回NULL
**************************************************/
u8* check_string(u8 *str, u8 *flg, u16 len)
{
	u8	*p;
	u16	i;
	u16 flg_len = strlen(flg);
	p = memchr(str, flg[0], len);
	while(p)
	{
		i = 0;
		while(p[i] == flg[i])	
		{
			i++;
			if(i >= flg_len)	return p;
		}
		p = memchr(p+1, flg[0], str-p-1);
		//p = strchr(p, flg[0]);
	}
	return NULL;
}

/*****************************************
ASCII 到 HEX 的转换函数
入口参数： O_data: 转换数据的入口指针，
		   N_data: 转换后新数据的入口指针
		   len : 需要转换的长度
返回参数：-1: 转换失败
其它：转换后数据长度
注意：O_data[]数组中的数据在转换过程中会被修改。
****************************************/
u16 ascii_to_hex(u8 *O_data, u8 *N_data, u16 len)
{
	u16 i,j,tmp_len;
	u8 tmpData;
	u8 *O_buf = O_data;
	u8 *N_buf = N_data;
	for(i = 0; i < len; i++)
	{
		if ((O_buf[i] >= '0') && (O_buf[i] <= '9'))
		{
			tmpData = O_buf[i] - '0';
		}
		else if ((O_buf[i] >= 'A') && (O_buf[i] <= 'F')) //A....F
		{
			tmpData = O_buf[i] - 0x37;
		}
		else if((O_buf[i] >= 'a') && (O_buf[i] <= 'f')) //a....f
		{
			tmpData = O_buf[i] - 0x57;
		}
		else
		{
			return -1;
		}
		O_buf[i] = tmpData;
	}
	for(tmp_len = 0,j = 0; j < i; j+=2)
	{
		N_buf[tmp_len++] = (O_buf[j]<<4) | O_buf[j+1];
	}
	return tmp_len;
}
/*****************************************
HEX 到 ASCII 的转换函数
入口参数： data: 转换数据的入口指针
			buffer: 转换后数据入口指针
			len : 需要转换的长度
返回参数：转换后数据长度
*******************************************/
u16 hex_to_ascii(u8 *data, u8 *buffer, u16 len)
{
	uc8 ascTable[17] = {"0123456789ABCDEF"};
	u8 *tmp_p = buffer;
	u16 i, pos;
	pos = 0;
	for(i = 0; i < len; i++)
	{
		tmp_p[pos++] = ascTable[data[i] >> 4];
		tmp_p[pos++] = ascTable[data[i] & 0x0f];
	}
	tmp_p[pos] = '\0';
	return pos;
}
/*****************************************
字符 到 整数 的转换函数
低位在前高位在后，遇到非‘0’~‘9’字符返回
入口参数：data 字符串
		  len 长度
返回参数：转换后的整数
*******************************************/
u32 char_to_int(u8 *cdata, u16 len)
{
	u8 i, weight;
	u32 ndata;
//	weight = 1;
	ndata = 0;
	for(i=0; i<len; i++)
	{
		if(cdata[i]>'0' && cdata[i]<'9')
		{
			//ndata += (cdata[i] - '0')*weight;
			//weight *= 10;
			ndata = (ndata * 10) + cdata[i];
		}
		else
			return ndata;
	}
	return ndata;
}


s8	at_send_data(u8 *data, u16 len)//send data
{
	u16	i = 0;
	while(i<len)
	{
		usart_send_char(data[i++]);//

	}
	return 0;
}
s8	at_return_data(u8 *buf, u16 len)//receive data
{
	u16	i = 0;
	while(i<len)
	{
		usart_receive_char(data[i++]);//

	}
	return 0;
}


//at
s8	at_at(void)								//测试串口
{
	u8 buf[4];
    at_send_data(AT_AT, sizeof(AT_AT));
 	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
	else	return -1;

}

s8	at_ate0(void)                             //关闭回显
{
	u8 buf[4];
    at_send_data(AT_ATE0, strlen(AT_ATE0));
 	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
	else	return -1;
}
s8	at_ate1(void)                             //打开回显
{
	u8 buf[4];
    at_send_data(AT_ATE1, strlen(AT_ATE0));
 	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
	else	return -1;
}
s8	at_tsim(void)                             //sim	 检测到sim卡返回0
{
	u8 buf[9];
	at_send_data("AT%TSIM\r", strlen("AT%TSIM\r"));
	at_return_data(buf, 9);
	if(check_string(buf, "1", 9))	return 0;
 	else	return -1;
}
/*****************************************
测试信号强度
入口参数：无
返回参数：信号强度值 0 ~ 31
*******************************************/
s8	at_csq(void)                            
{
	u8 buf[16], *p;
    at_send_data("AT+CSQ\r", strlen("AT+CSQ\r"));
 	at_return_data(buf, 16);
	if(check_string(buf, "OK", 16) == NULL)	return -1;
	p =  check_string(buf, ",", 16);
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
	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
 	else	return -1;
}
/*****************************************
查询ip数据包模式设置
入口参数： 
返回参数： 成功返回mode	低三位分别对应数据转换、链接、缓存
*******************************************/
u8	at_iomode_check(void)                    
{
	u8 buf[17];
	u8 mode = 0;
	u8 *p;
	strcpy(buf, "AT%IOMODE?\r");
	at_send_data(buf, strlen(buf));
	at_return_data(buf, 17);
	if(check_string(buf, "OK", 17) == NULL)	return 8;
	//数据转换
	p = check_string(buf, ",", 17);
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

s8	gtm900c_init(void)
{
	s8 res = 0;
	u8 buf[30];
	//模块启动\重启过后需等待6s
	res = at_at();
	if(res != 0) return 1;
	res = at_ate0();
	if(res != 0) return 2;
	res = at_tsim();
	if(res != 0) return 3;
	//AT+CREG
	at_send_data("AT+COPS?\r", strlen("AT+COPS?\r"));
	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4) == NULL)	return 4;
	//AT+COPS?
	at_send_data("AT+COPS?\r", strlen("AT+COPS?\r"));
	at_return_data(buf, 28);
	if(check_string(buf, "CHINA MOBILE", 28) == NULL)	return 5;
	//AT+CGATT?
	at_send_data("AT+CGATT?\r", strlen("AT+CGATT?\r"));
	at_return_data(buf, 13);
	if(check_string(buf, "1", 13) == NULL)	return 5;  

	return 0;
}

/*****************************************
tcp/ip初始化
入口参数：user 用户名 长度最大10
		  password 密码	长度最大10
		  可以为空 
返回参数：正确返回0
*******************************************/
s8	gtm900c_tcpip_init(u8 *user, u8 *password)                    
{
	u8 len = 0;
	u8 buf[40];
	if(strlen(user)>10 || strlen(password))	return 1;
	//配置apn
	at_send_data("AT+CGDCONT=1,\"IP\",\"CMNET\"", strlen("AT+CGDCONT=1,\"IP\",\"CMNET\""));
	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4) == NULL)	return 1;
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
	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
 	else	return 2;
}

/*****************************************
设置tcpip数据帧模式
并在GTM900C_TCPIP_IOMODE中保存当前模式设置
当前使用缓存
入口参数： mode	低三位分别对应(从底到高)数据转换、链接、缓存
返回参数： 正确返回0
*******************************************/
s8	gtm900c_tcpip_mode_init(u8 mode)
{
	s8 res;
	if(mode>=8)	return 1;
	res = at_iomode(mode);
	if(res != 0) return 2;
	GTM900C_TCPIP_IOMODE = at_iomode_check();
	if(GTM900C_TCPIP_IOMODE != mode)	return 3;
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
s8	gtm900c_tcpip_creat_connection(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port)                    
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
	at_return_data(buf, 4);	
	if(check_string(buf, "OK", 4))	return 0;
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
s8	gtm900c_tcpip_creat_connection_n(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port, u8 link_num)                    
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
	at_return_data(buf, 4);	
	if(check_string(buf, "OK", 4))	return 0;
 	else	return 4;	 

}
/*****************************************
tcp/ip关闭链接、注销gprs、关闭服务器	 
入口参数：link_num 1~3链接序号 5注销gprs 6关闭服务器
返回参数：正确返回0
*******************************************/
s8	gtm900c_tcpip_close_connection(u8 link_num)
{
	u8 buf[15];
	u8 len = 0;
	sprintf(buf,"AT\%IPCLOSE=%d,", link_num);
	len = strlen(buf);
	at_send_data(buf, len);
	at_return_data(buf, 4);	
	if(check_string(buf, "OK", 4))	return 0;
 	else	return 1;

} 

/*****************************************
tcp/ip 数据发送
入口参数：data 数据buffer地址 
		  len  长度	最大GTM900C_TCPIP_SEND_DATALEN_MAX
		  link_num 多连接模式时链接序号 单链接模式不起作用，可赋值0 
返回参数：正确返回0
*******************************************/
s8	gtm900c_tcpip_send(u8 *data, u16 len, u8 link_num)
{
	u8 databuf[GTM900C_TCPIP_SEND_DATALEN_MAX*2+15];
	u16 databuf_len;
//	memset(buf, 0, GTM900C_TCPIP_SEND_DATALEN_MAX*2);
	if(len > GTM900C_TCPIP_SEND_DATALEN_MAX)  return 1;
	//选择单\多链接，发送
	if(GTM900C_TCPIP_IOMODE & 0x02)
	{
		strcpy(databuf, "AT%IPSENDX=");
	}
	else
	{
		strcpy(databuf, "AT%IPSEND=");
	}
	databuf_len = strlen(databuf);
	//数据转换
	if(GTM900C_TCPIP_IOMODE & 0x01)
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
	at_return_data(databuf, 4);	
	if(check_string(databuf, "OK", 4))	return 0;
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
s8	gtm900c_tcpip_recvbuf_query(u8 *unread, u8 *total)
{
	u8	buf[30];
	u8	*pF, *pB;
	strcpy(buf, "AT%IPDQ\r");
	at_send_data(buf, strlen(buf));
	at_return_data(buf, 23);
	pF = check_string(buf, ":", 15);
	pB = check_string(buf, ",", 20);
	if(~(pF&&pB)) return 2;
	*unread =  char_to_int(pF, pB-pF);
	*unread =  char_to_int(pF, 2);
	pF = check_string(buf, ",", 15);
	pB = check_string(buf, "OK", 20);
	if(~(pF&&pB)) return 3;
	*total =  char_to_int(pF, pB-pF);
	return 0;
}

/*****************************************
tcp/ip 数据接收缓冲区删除
入口参数：
		 
出口参数：
*******************************************/
s8	gtm900c_tcpip_recvbuf_delete(u8 )



