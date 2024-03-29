
#include "gd_gm.h"

#include <ucos_ii.h>
#include <string.h>
#include <stdio.h>
#include "../drives/uart_drv.h"
#include "gd_system.h"


#define sendAT(s, l)	guart_send_data((s), (l))
#define RECVAT_TIMEOUT	1200


gprs_databuf_t	gprs_databuf;
u8 GPRS_TCPIP_IOMODE;

//recv wait tick*10 ms
u16 recvAT(u16 tick)
{
	u16 time_count = 0;
	gprs_databuf.recvlen = 0;
 	while(gprs_databuf.recvlen == 0) 
	{
		OSTimeDlyHMSM(0, 0, 0, 10);
		time_count++;
		if(time_count >= tick) 
			return 0;
	}
	
	return 	gprs_databuf.recvlen;
}
void delays(u8 n_second)
{
	OSTimeDlyHMSM(0, 0, n_second, 0);	
}


//at
s8	at_at(void)								//测试串口
{
	sendAT("AT\r\n", sizeof("AT\r\n"));
 	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
	else	 return -1;

}

s8	at_ate0(void)                             //关闭回显
{
	sendAT("ATE0\r\n", strlen("ATE0\r\n"));
 	recvAT(RECVAT_TIMEOUT);
//	recvAT(2);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
	else 	return -1;
}
s8	at_ate1(void)                             //打开回显
{
	sendAT("ATE1\r\n", strlen("ATE1\r\n"));
 	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
	else 	return -1;
}
s8	at_tsim(void)                             //sim	 检测到sim卡返回0
{
	sendAT("AT%TSIM\r\n", strlen("AT%TSIM\r\n"));
 	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "%TSIM 1", gprs_databuf.recvlen)){
 		recvAT(RECVAT_TIMEOUT);
		return 0;
		}
	recvAT(10)  ;
	return -1;
}

/*****************************************
测试信号强度
入口参数：无
返回参数：信号强度值 0 ~ 31
*******************************************/
s8	at_csq(void)                            
{
	u8  *p, csq, num;
    	sendAT("AT+CSQ\r\n", strlen("AT+CSQ\r\n"));
	recvAT(RECVAT_TIMEOUT) ;
	csq = 0;
//	if(check_string(gprs_databuf.recvdata, "OK", 18) == NULL)	return -1;
	p =  check_string(gprs_databuf.recvdata, ",", gprs_databuf.recvlen);
	if(p)	{
		num = *(p-2);
		if(num>='0' && num<='9'){
			csq = (num-'0') * 10;
			} 
		num = *(p-1);	
		if(num>='0' && num<='9'){
			csq += num - '0';
			}
//		csq = (*(p-2)*10 + *(p-1));
		recvAT(10);
		return csq;
		}	
	recvAT(10);
	return -1;
}
/*****************************************
设置ip数据包模式
入口参数： mode	低三位分别对应(从底到高)数据转换、链接、缓存
返回参数： 正确返回0
*******************************************/
s8	at_iomode(u8 mode)                    
{
	sprintf(gprs_databuf.senddata, "AT%%IOMODE=%d,%d,%d\r\n", mode&0x01, ((mode&0x02)>>1)+1, (mode&0x04)>>2);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
 	else 	return -1;
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
	sendAT("AT%IOMODE?\r\n", strlen("AT%IOMODE?\r\n"));
	recvAT(RECVAT_TIMEOUT);
//	if(check_string(gprs_databuf.recvdata, "OK", 13) == NULL)	return 8;
	//数据转换
	p = check_string(gprs_databuf.recvdata, ",", gprs_databuf.recvlen);
	if(p == NULL) return 9;
	if(*(p-1) == '1') mode |= 0x01;
	else if(*(p-1) != '0') return 10;
	//链接
	p = check_string(p+1, ",", 17);
	if(p == NULL) return 11;
	if(*(p-1) == '2') mode |= 0x02;
	else if(*(p-1) != '1') return 12;
	//缓存
//	p = check_string(p+1, ",", 17);
//	if(p == NULL) return 13;
	if(*(p+1) == '1') mode |= 0x04;
	else if(*(p+1) != '0') return 14;
	recvAT(10);
	return mode;
 	
}



/*=======================================================*/	 



/*****************************************
模块关机
入口参数:  
返回参数:
*******************************************/
void gprsmodule_shutdown(void)
{
	sendAT("AT%MSO\r\n", strlen("AT%MSO\r\n"));
}
/*****************************************
模块复位 
入口参数:  
返回参数:
*******************************************/
s8 gprsmodule_reset(void)
{
	sendAT("AT%RST\r\n", strlen("AT%RST\r\n"));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "Interpreter ready", gprs_databuf.recvlen))	
	{
		return 0;
	}	
	return 1;
}	

/*****************************************
模块串口设置
入口参数：user 用户名 长度最大10
		  password 密码	长度最大10
		  可以为空 
返回参数：正确返回0
*******************************************/
s8 gprs_bps_set(u32 rate)
{
	sprintf(gprs_databuf.senddata, "AT+IPR=%d,\r\n", rate);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 1;
	return 0;
}


/*****************************************
模块初始化
入口参数：
返回参数：
*******************************************/

s8	gprsmodule_init(void)
{
	s8 res = 0;
	//模块启动\重启过后需等待6s	 //重启后有非请求at结果码
//	res = at_at();
//	if(res != 0) return 1;
	res = at_ate0();
	if(res != 0) return 2;
	delays(1);
	res = at_tsim();
	if(res != 0) return 3;

	//AT+CREG  //////  待进一步处理
	sendAT("AT+CREG?\r\n", strlen("AT+CREG?\r\n"));
	recvAT(RECVAT_TIMEOUT);
//	if(check_string(gprs_databuf.recvdata, "", 10) == NULL)	return 5;
	recvAT(10);
	if(check_string(gprs_databuf.recvdata, "OK", 2) == NULL)	return 4;
	delays(2);

	//AT+COPS?  
	sendAT("AT+COPS?\r\n", strlen("AT+COPS?\r\n"));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, /*中移动*/"CHINA  MOBILE", gprs_databuf.recvlen) == NULL)	return 5;
	recvAT(10);

	//AT+CGATT?
	sendAT("AT+CGATT?\r\n", strlen("AT+CGATT?\r\n"));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "+CGATT: 1", gprs_databuf.recvlen) == NULL)	return 5; 
	recvAT(10); 

	return 0;
}


/*****************************************
短信初始化
入口参数：
返回参数：
*******************************************/
s8	gm_sms_init(u8 *smssvr)
{
	//AT+CPMS 
	sendAT("AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n", strlen("AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n"));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "ERROR", gprs_databuf.recvlen) != NULL)	return 1;
	recvAT(10);

	//AT+CMGF 
	sendAT("AT+CMGF=1\r\n", strlen("AT+CMGF=1\r\n"));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 2;

	//AT+CSCA 
	sprintf(gprs_databuf.senddata,"AT+CSCA=\"86%s\",145\r\n", smssvr);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 3;

	//AT+CNMI 
	sendAT("AT+CNMI=2,1\r\n", strlen("AT+CNMI=2,1\r\n"));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 4;

	return 0;
}
/*****************************************
短信删除
入口参数：sms_index
				delete_type
返回参数：
*******************************************/
s8	gm_sms_delete(u8 sms_index, u8 delete_type)
{
	//AT+CMGD 
	sprintf(gprs_databuf.senddata,"AT+CMGD=%u, %u\r\n", sms_index, delete_type);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 1;

	return 0;
}
/*****************************************
短信读取
入口参数：sms_index
				buf
返回参数：
*******************************************/
s8	gm_sms_read(u8 sms_index, u8 *buf, u8 *len)
{
	//AT+CMGR 
	sprintf(gprs_databuf.senddata,"AT+CMGR=%u\r\n", sms_index);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "ERROR", gprs_databuf.recvlen) != NULL)	return 1;
	recvAT(RECVAT_TIMEOUT);
	if(gprs_databuf.recvlen > 100)	gprs_databuf.recvlen = 100;
	memcpy(buf, gprs_databuf.recvdata, gprs_databuf.recvlen);
	*len = gprs_databuf.recvlen;
	recvAT(10);

	return 0;
}
/*****************************************
短信查询
入口参数：query_type
				
返回参数：
*******************************************/
s8	gm_sms_query(u8 *query_type)
{
	//AT+CMGR 
	sprintf(gprs_databuf.senddata,"AT+CMGL=%s\r\n", query_type);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
//	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
//	recvAT(RECVAT_TIMEOUT);

	return 0;
}

/*****************************************
电话初始化
入口参数：
返回参数：
*******************************************/
s8	gm_phone_init(void)
{
	//AT+CPMS 
	sendAT("AT+CLIP=1\r\n", strlen("AT+CLIP=1\r\n"));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 1;

	return 0;
}



/*****************************************
tcp/ip初始化
入口参数：apnuser 用户名 长度最大10
		  password 密码	长度最大10
		  可以为空 
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_init(u8 *apn, u8 *apnuser, u8 *apnpassword)                    
{
	if(strlen(apnuser)>10 || strlen(apnpassword)>10)	return 1;
	//配置apn
	sprintf(gprs_databuf.senddata,"AT+CGDCONT=1,\"IP\",\"%s\"\r\n", apn);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 1;
	//tcpip enable
	strcpy(gprs_databuf.senddata, "AT%ETCPIP");
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	if(apnuser)
	{
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '=';
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
		strcpy(gprs_databuf.senddata+gprs_databuf.sendlen, apnuser);
		gprs_databuf.sendlen += strlen(apnuser);
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
		gprs_databuf.senddata[gprs_databuf.sendlen++] = ',';
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
		strcpy(gprs_databuf.senddata+gprs_databuf.sendlen, apnpassword);
		gprs_databuf.sendlen += strlen(apnpassword);	
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
	
	}
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\r';
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\n';
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\0'; 
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
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
	if(strlen(ip)>15)	return 1;
	strcpy(gprs_databuf.senddata, "AT%IPOPEN=");
	if(type == 1)	strcat(gprs_databuf.senddata, "\"UDP\"");	
	else if(type == 0)	strcat(gprs_databuf.senddata, "\"TCP\"");
	else return 2;
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,",\"%s\",%d,%d,%d\r\n", ip, dest_port, udp_dest_port, local_port);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "CONNECT", gprs_databuf.recvlen))	return 0;
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
	if(strlen(ip)>15)	return 1;
	strcpy(gprs_databuf.senddata, "AT%IPOPEN=");
	if(link_num==0 || link_num>3)	return 2;
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,"%u,", link_num);
	if(type == 1)	strcat(gprs_databuf.senddata, "\"UDP\"");	
	else if(type == 0)	strcat(gprs_databuf.senddata, "\"TCP\"");
	else return 3;
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,",\"%s\",%u,%u,%u\r\n", ip, dest_port, udp_dest_port, local_port);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
 	else	return 4;	 

}
/*****************************************
tcp/ip关闭链接、注销gprs、关闭服务器	 
入口参数：link_num 1~3链接序号 5注销gprs 6关闭服务器
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_close_connection(u8 link_num)
{
	sprintf(gprs_databuf.senddata,"AT%%IPCLOSE=%u\r\n", link_num);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
 	else	return 1;

} 

/*****************************************
tcp/ip 数据发送
入口参数：data 数据buffer地址 
		  len  长度	最大GPRS_DATA_LEN_MAX
		  link_num 多连接模式时链接序号
		  		  单链接模式不起作用，可赋值0 
返回参数：正确返回0
*******************************************/
s8	gprs_tcpip_send(u8 *data, u16 len, u8 link_num)
{
//	memset(buf, 0, GPRS_DATA_LEN_MAX*2);
	if(len > GPRS_DATA_LEN_MAX)  return 1;
	//选择单\多链接，发送
	if(GPRS_TCPIP_IOMODE & 0x02)
	{
		sprintf(gprs_databuf.senddata, "AT%%IPSENDX=%d,\"", link_num);
	}
	else
	{
		strcpy(gprs_databuf.senddata, "AT%IPSEND=\"");
	}
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	//数据转换
	if(GPRS_TCPIP_IOMODE & 0x01)
	{
		len = hex_to_ascii(data, gprs_databuf.senddata+gprs_databuf.sendlen, len);
	}
	else
	{
	 	memcpy(gprs_databuf.senddata+gprs_databuf.sendlen, data, len);
	}
	gprs_databuf.sendlen += len;
	memcpy(gprs_databuf.senddata+gprs_databuf.sendlen, "\"\r\n", 4);
	gprs_databuf.sendlen += 4;
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "ERROR", gprs_databuf.recvlen))
		return 2;		
	recvAT(10);
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	
		return 0;
 	else	
		return 3;

}



/*****************************************
tcp/ip 数据接收(使用缓冲)
入口参数:data_index_p 用于存放数据包序号，0时默认第一包未读数据
		 link_num_p  用于存放连接序号
		 data_len_p	 用于存放数据长度
		 recvdata 用于存放接收到的数据
出口参数:正确返回0
*******************************************/
s8 gprs_tcpip_request_data(u8 index, u8 *data_index_p, u8 *link_num_p, u16 *data_len_p, u8 *recvdata)
{
	u8	*pF, *pB;
	sprintf(gprs_databuf.senddata, "AT%%IPDR=%d\r\n", index);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(RECVAT_TIMEOUT);
	if(check_string(gprs_databuf.recvdata, "ERROR", 6))	
		return 1;
	pF = check_string(gprs_databuf.recvdata, ":", 9);
	pB = check_string(gprs_databuf.recvdata, ",", 9);
	if((pF == NULL) || (pB == NULL)) 
		return 2;
	*link_num_p =  char_to_int(pF+1, pB-pF);
	pF = pB+1;
	pB = check_string(pF, ",", 6);
	if(pB == NULL) 
		return 3;
	*data_index_p =  char_to_int(pF, pB-pF);
	pF = pB+1;
	pB = check_string(pF, ",", 6);
	if(pB == NULL) 
		return 4;
	*data_len_p =  char_to_int(pF, pB-pF);
	pF = pB+2;
	ascii_to_hex(pF, recvdata, (*data_len_p)*2);
	recvAT(10);
	return 0;
}

/*****************************************
tcp/ip 数据接收缓冲区查询
入口参数：unread 用于返回未读数据包数
		  		total	 用于返回缓冲区数据包总数
出口参数：
*******************************************/
s8	gprs_tcpip_recvbuf_query(u8 *unread, u8 *total)
{
	u8	*pF, *pB;
	strcpy(gprs_databuf.senddata, "AT%IPDQ\r\n");
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(RECVAT_TIMEOUT);
	pF = check_string(gprs_databuf.recvdata, ":", gprs_databuf.recvlen);
	pB = check_string(gprs_databuf.recvdata, ",", gprs_databuf.recvlen);
	if((pF == NULL) || (pB == NULL)) return 2;
	*unread =  char_to_int(pF+1, pB-pF);
//	pF = check_string(gprs_databuf.recvdata, ",", 15);
//	pB = check_string(gprs_databuf.recvdata, "OK", 20);
//	if((pF == NULL) || (pB == NULL)) return 3;
	*total =  char_to_int(pB+1, gprs_databuf.recvlen-(pB-gprs_databuf.recvdata));
	recvAT(10);
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
	u8  *pF, *pB, res;
	if(index>50 || type>2)	return -1;	
	sprintf(gprs_databuf.senddata,"AT%%IPDD=%d,%d\r\n", index, type);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(RECVAT_TIMEOUT);	
	pF =  check_string(gprs_databuf.recvdata, ":", gprs_databuf.recvlen);
	if(pF == NULL) return -2;
	pB =  gprs_databuf.recvdata;
	res = char_to_int(pF, gprs_databuf.recvlen - (pF-pB));
	recvAT(10);
	return res;
	
}

/**********************************************
tcp/ip 数据接收缓冲区删除模式不设置
		采用默认：自动删除
**********************************************/


/*****************************************
模块非请求结果码处理
		从gprs接收到数据
		连接断开
入口参数： 
出口参数： 
*******************************************/
s8	gprs_unrequest_code_dispose(u32 len)
{
	u8	*p=NULL, res, *pb=NULL;
	if(len < 6)	
		return -1;
	p = check_string(gprs_databuf.recvdata, "OK", len);
	if(p)	
		return GM_AT_COMMAND_OK;

	p = check_string(gprs_databuf.recvdata, "%IPDATA:", len);
	if(p)	
		return GM_TCPIP_RECEIVED_DATA;

	p = check_string(gprs_databuf.recvdata, "%IPCLOSE:", len);
	if(p)	
	{
		res	= char_to_int(p+9, 1);
		if(res >=1 && res <=6)
			return (res+1); 
	}
	p = check_string(gprs_databuf.recvdata, "+CLIP:", len);
	if(p)	
	{
		p = check_string(p+5, "\"", len);
		p++;
		pb = check_string(p, "\"", len);
		if(p==NULL || pb==NULL)	return -3;
		memcpy(gd_system.network_task.ring_num, p, 11);
		return GM_TCPIP_RECEIVED_RING;
	}
	p = check_string(gprs_databuf.recvdata, "+CMTI:", len);
	if(p)	
	{
		p = check_string(p+6, ",", len);
		if(p==NULL)	return -5;
		p++;
		gd_system.network_task.sms_index = char_to_int(p, len-(p-(u8 *)gprs_databuf.recvdata));
		return GM_TCPIP_RECEIVED_SMS;
	}
	

	return -2; 
}






