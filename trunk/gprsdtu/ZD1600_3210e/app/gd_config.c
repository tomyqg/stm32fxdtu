
#include "gd_config.h"
#include "gd_system.h"
#include "uart_drv.h"
#include "gd_gm.h"
#include "gd_string.h"
#include <string.h>	  
#include <stdio.h>
#include "hw_core.h"



#define config_rxbuf	gd_system.suart_task.rx_buf
#define config_databuf gprs_databuf

#define CONFIG_MODE_FLAG	'c'
#define NULL 0


void at_dispose(void);
void acti_config(INT8U *cdata, INT8U clen);
void server_config(INT8U *cdata, INT8U clen, INT8U ctype);
void apn_config(INT8U *cdata, INT8U clen, INT8U ctype) ;
void mode_config(INT8U *cdata, INT8U clen);
void pollint_config(INT8U *cdata, INT8U clen);
void retmain_config(INT8U *cdata, INT8U clen);
void smsc_config(INT8U *cdata, INT8U clen);
void sim_config(INT8U *cdata, INT8U clen);
void dataint_config(INT8U *cdata, INT8U clen);
void did_config(INT8U *cdata, INT8U clen);
void wakeup_config(INT8U *cdata, INT8U clen, INT8U ctype);
void com_config(INT8U *cdata, INT8U clen, INT8U ctype);
void config_require(void);

void at_command_ready(void);
void command_ok(void);
void error_command(void);
void sys_restart(void);

#define  GD_CONFIG_APN				0			
#define  GD_CONFIG_APN_USER			1
#define  GD_CONFIG_APN_PWD			2	
#define  GD_CONFIG_APN_CENTER		3

#define  GD_CONFIG_IP				4
#define  GD_CONFIG_PORT				5
#define  GD_CONFIG_IP1				6
#define  GD_CONFIG_PORT1				7
#define  GD_CONFIG_IP2				8
#define  GD_CONFIG_PORT2				9
#define  GD_CONFIG_BKPIP			10
#define  GD_CONFIG_BKPPORT			11

#define  GD_CONFIG_DNSSVR			12
#define  GD_CONFIG_BKPDNSSVR		13			
#define  GD_CONFIG_SVRCNT			14

#define  GD_CONFIG_WP				15
#define  GD_CONFIG_SMSPWD			16

#define  GD_CONFIG_SERMODE			17
#define  GD_CONFIG_COMBR			18





OS_STK gd_task_config_stk[GD_TASK_CONFIG_STK_SIZE];

INT32U gd_get_config()
{
	INT32U *pRxLen = NULL;
	INT32U i, len;
	INT32U time_index;
	memset(config_rxbuf, 0, 2048);
	uart_rx_itconf(SUART, DISABLE);
	pRxLen = uart_rx_bufset(SUART, config_rxbuf, 2048);
	uart_rx_itconf(SUART, ENABLE);

	len = 0;
	i = 0;
	time_index = 0;
	while(1)
	{
//		len = *pRxLen;
		if(*pRxLen != len)	
		{
			len = *pRxLen;
			if(i > len) i = 0;
			for( ; i<len; i++)
			{
				if(config_rxbuf[i] == 'c')
				{
					uart_rx_itconf(SUART, DISABLE);
					return GD_CONFIG_MODE;
				}
			
			}

		}
		time_index ++;
		if(time_index > 300)	return GD_TRANS_MODE;
		OSTimeDlyHMSM(0, 0, 0, 10);


	}

//	return GD_TRANS_MODE;
}


void gd_task_config(void *parg)
{
	INT32U *pRxLen = NULL;
	INT32U len;
	INT8U *posi = NULL;

	memset(config_rxbuf, 0, 2048);
	memset(config_databuf.senddata, 0, config_databuf.sendlen);
	uart_rx_itconf(SUART, DISABLE);
	pRxLen = uart_rx_bufset(SUART, config_rxbuf, 2048);
	uart_rx_itconf(SUART, ENABLE);

at_command_ready();

	len = 0;
	while(1)
	{
//		len = *pRxLen;
		if(*pRxLen != len)	
		{
			len = *pRxLen;
			posi = check_string(config_rxbuf, "\r\n", len);
			if(posi)
			{
				uart_rx_itconf(SUART, DISABLE);	
				*pRxLen = 0;
				config_databuf.recvlen = posi-(INT8U *)config_rxbuf;
				memcpy(config_databuf.recvdata, config_rxbuf, config_databuf.recvlen);
				len = 0;
				uart_rx_itconf(SUART, ENABLE);

				at_dispose();
			}

		}
		OSTimeDlyHMSM(0, 0, 0, 5);
			    

	}	




	OSTaskDel(OS_PRIO_SELF);
}


void at_dispose(void)
{
	INT8U *posi = NULL;	 
	INT8U *pcheck = NULL;
	INT32U len = 0;

	posi = check_string(config_databuf.recvdata, "AT+Z", config_databuf.recvlen);
	if(posi == NULL)	return;

	len = config_databuf.recvlen - (posi - (INT8U *)config_databuf.recvdata);
	len -= 4;

	switch(posi[4])
	{
/*
+ZACTI:xxxx								//DTU激活方式
+ZAPN:xxxx								//APN接入点
+ZAPNCENT:xxx							//APN拨号中心号码
+ZAPNPWD:xxx							//APN接入点密码
+ZAPNUSER:xxx							//APN接入点用户名
*/	
	case 'A':
		pcheck = check_string(posi+4, "ACTI=", len);
		if(pcheck)	
		{	
			len -= 5;
			acti_config(posi+9, len);
			return;
		}
		pcheck = check_string(posi+4, "APN=", len);
		if(pcheck)	
		{
			len -= 4;
			apn_config(posi+8, len, GD_CONFIG_APN);
			return;
		}
		pcheck = check_string(posi+4, "APNCENT=", len);
		if(pcheck)	
		{
			len -= 8;
			apn_config(posi+12, len, GD_CONFIG_APN_CENTER);
			return;
		}
		pcheck = check_string(posi+4, "APNPWD=", len);
		if(pcheck)	
		{
			len -= 7;
			apn_config(posi+11, len, GD_CONFIG_APN_PWD);
			return;
		}
		pcheck = check_string(posi+4, "APNUSER=", len);
		if(pcheck)	
		{
			len -= 8;
			apn_config(posi+12, len, GD_CONFIG_APN_USER);
			return;
		}

		break;
/*
+ZBKPDNSSVR:aaa.bbb.ccc.ddd				//备份域名服务器IP
+ZBKPIP:xxx    							//备份中心服务器IP
+ZBKPPORT:xxx  							//备份中心服务器PORT
+ZBR:xxx								//DTU波特率
*/		
	case 'B':
		pcheck = check_string(posi+4, "BKPDNSSVR=", len);
		if(pcheck)	
		{	
			len -= 10;
			server_config(posi+14, len, GD_CONFIG_BKPDNSSVR);
			return;
		}
		pcheck = check_string(posi+4, "BKPIP=", len);
		if(pcheck)	
		{
			len -= 6;
			server_config(posi+10, len, GD_CONFIG_BKPIP);
			return;
		}
		pcheck = check_string(posi+4, "BKPPORT=", len);
		if(pcheck)	
		{
			len -= 8;
			server_config(posi+12, len, GD_CONFIG_BKPPORT);
			return;
		}
		pcheck = check_string(posi+4, "BR=", len);
		if(pcheck)	
		{
			len -= 3;
			com_config(posi+7, len, GD_CONFIG_COMBR);
			return;
		}

		break;
/*
+ZDATAINT:xxx							//DTU数据帧间隔时间
+ZDID:aabbccdd							//DTU设备ID
+ZDNSSVR:aaa.bbb.ccc.ddd 				//主域名服务器IP
*/		
	case 'D':
		pcheck = check_string(posi+4, "DATAINT=", len);
		if(pcheck)	
		{
			len -= 8;
			dataint_config(posi+12, len);
			return;
		}
		pcheck = check_string(posi+4, "DID=", len);
		if(pcheck)	
		{
			len -= 4;
			did_config(posi+8, len);
			return;
		}
		pcheck = check_string(posi+4, "DNSSVR=", len);
		if(pcheck)	
		{
			len -= 7;
			server_config(posi+11, len, GD_CONFIG_DNSSVR);
			return;
		}
		
		break;
/*
+ZIP:xxx     								//主中心服务器IP
+ZIPn:xxx        							//中心服务器n的IP
*/		
	case 'I':
		pcheck = check_string(posi+4, "IP=", len);
		if(pcheck)	
		{
			len -= 3;
			server_config(posi+7, len, GD_CONFIG_IP);
			return;
		}
	//	posi = check_string(posi+4, "IPn=", len);
		pcheck = check_string(posi+4, "IP1=", len);
		if(pcheck)	
		{
			len -= 4;
			server_config(posi+8, len, GD_CONFIG_IP1);
			return;
		}
		pcheck = check_string(posi+4, "IP2=", len);
		if(pcheck)	
		{
			len -= 4;
			server_config(posi+8, len, GD_CONFIG_IP2);
			return;
		}
		
		break;
/*		
+ZMODE:xxxx							//DTU工作模式		
*/
	case 'M':
		pcheck = check_string(posi+4, "MODE=", len);
		if(pcheck)	
		{
			len -= 5;
			mode_config(posi+9, len);
			return;
		}
				
		break;
/*
+ZPORT:xxx  								//主中心服务器PORT
+ZPORTn:xxx								//中心服务器n的PORT
+ZPOLLINT:xxx							//心跳包间隔时间（单位：秒）
*/	
	case 'P':
		pcheck = check_string(posi+4, "PORT=", len);
		if(pcheck)	
		{
			len -= 5;
			server_config(posi+9, len, GD_CONFIG_PORT);
			return;
		}
		pcheck = check_string(posi+4, "PORT1=", len);
		if(pcheck)	
		{
			len -= 6;
			server_config(posi+10, len, GD_CONFIG_PORT1);
			return;
		}
		pcheck = check_string(posi+4, "PORT2=", len);
		if(pcheck)	
		{
			len -= 6;
			server_config(posi+10, len, GD_CONFIG_PORT2);
			return;
		}
		pcheck = check_string(posi+4, "POLLINT=", len);
		if(pcheck)	
		{
			len -= 8;
			pollint_config(posi+12, len);
			return;
		}
		break;
/*
+ZRETMAIN:x								//DTU是否自动返回主中心
*/		
	case 'R':
		pcheck = check_string(posi+4, "RETMAIN=", len);
		if(pcheck)	
		{
			len -= 8;
			retmain_config(posi+12, len);
			return;
		}
		pcheck = check_string(posi+4, "RDPARAM?", len);
		if(pcheck)	
		{
			config_require();
			return;
		}
		pcheck = check_string(posi+4, "RESET?", len);
		if(pcheck)	
		{
			sys_restart();
			return;
		}
		break;
	
/*
+ZSVRCNT:x								//中心服务器数目
+ZSMSC:xxx								//短信中心号码
+ZSMSPWD=xxx							//短信唤醒密码
+ZSIM:xxxxxxxxxx							//DTU SIM卡号码
+ZSERMODE:xxx							//DTU数据、校验及停止位
*/		
	case 'S':
		pcheck = check_string(posi+4, "SVRCNT=", len);
		if(pcheck)	
		{
			len -= 7;
			server_config(posi+11, len, GD_CONFIG_SVRCNT);
			return;
		}
		pcheck = check_string(posi+4, "SMSC=", len);
		if(pcheck)	
		{
			len -= 5;
			smsc_config(posi+9, len);
			return;
		}
		pcheck = check_string(posi+4, "SMSPWD=", len);
		if(pcheck)	
		{
			len -= 7;
			wakeup_config(posi+11, len, GD_CONFIG_SMSPWD);
			return;
		}
		pcheck = check_string(posi+4, "SIM=", len);
		if(pcheck)	
		{
			len -= 3;
			sim_config(posi+8, len);
			return;
		}
		pcheck = check_string(posi+4, "SERMODE=", len);
		if(pcheck)	
		{
			len -= 3;
			com_config(posi+8, len, GD_CONFIG_SERMODE);
			return;
		}		
		break;
/*
+ZWP=xxx								//拨号唤醒号码		
*/
	case 'W':
		pcheck = check_string(posi+4, "WP=", len);
		if(pcheck)	
		{
			len -= 3;
			wakeup_config(posi+7, len, GD_WAKE_PHONE_LEN);
			return;
		}
		break;
	default :
		error_command();
		break;
	
	}


}



void apn_config(INT8U *cdata, INT8U clen, INT8U ctype)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	switch(ctype)
	{
	case GD_CONFIG_APN:
		if(clen > GD_APN_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_apn.value, cdata, clen);
		gd_conf->gd_apn.value[GD_APN_LEN] = '\0';
		break;
	case GD_CONFIG_APN_USER:
		if(clen > GD_APN_USER_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_apn_user.value, cdata, clen);
		gd_conf->gd_apn_user.value[GD_APN_USER_LEN] = '\0';

		break;
	case GD_CONFIG_APN_PWD:
		if(clen > GD_APN_PWD_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_apn_pwd.value, cdata, clen);
		gd_conf->gd_apn_pwd.value[GD_APN_PWD_LEN] = '\0';

		break;
	case GD_CONFIG_APN_CENTER:
		if(clen > GD_APN_CENTER_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_apn_center.value, cdata, clen);
		gd_conf->gd_apn_center.value[GD_APN_CENTER_LEN] = '\0';

		break;
	default:
		error_command();
		return;
	}
	command_ok();
}
void acti_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	if(check_string(cdata, "AUTO", clen))
	{
		gd_conf->gd_acti_type.value = GD_ACTI_TYPE_AUTO;
		command_ok();
		return;
	}
	if(check_string(cdata, "SMS", clen))
	{
		gd_conf->gd_acti_type.value = GD_ACTI_TYPE_SMS;
		command_ok();
		return;
	}
	if(check_string(cdata, "PHONE", clen))
	{
		gd_conf->gd_acti_type.value = GD_ACTI_TYPE_PHONE;
		command_ok();
		return;
	}
	if(check_string(cdata, "DATA", clen))
	{
		gd_conf->gd_acti_type.value = GD_ACTI_TYPE_DATA;
		command_ok();
		return;
	}
	if(check_string(cdata, "MIX", clen))
	{
		gd_conf->gd_acti_type.value = GD_ACTI_TYPE_MIX;
		command_ok();
		return;
	}

	error_command();
}


void server_config(INT8U *cdata, INT8U clen, INT8U ctype)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	switch(ctype)
	{
	case GD_CONFIG_IP:
		if(clen > GD_SVR_IP_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_svr_ip.value, cdata, clen);
		gd_conf->gd_svr_ip.value[GD_SVR_IP_LEN] = '\0';
		break;

	case GD_CONFIG_PORT:
		gd_conf->gd_svr_port.value = char_to_int(cdata, clen);
		break;

	case GD_CONFIG_BKPIP:
		if(clen > GD_BKP_SVR_IP_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_bkp_svr_ip.value, cdata, clen);
		gd_conf->gd_bkp_svr_ip.value[GD_BKP_SVR_IP_LEN] = '\0';
		break;
	case GD_CONFIG_BKPPORT:
		gd_conf->gd_bkp_svr_port.value = char_to_int(cdata, clen);	
		break;
	case GD_CONFIG_IP1:
		if(clen > GD_SVR1_IP_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_svr1_ip.value, cdata, clen);
		gd_conf->gd_svr1_ip.value[GD_SVR1_IP_LEN] = '\0';
		
		break;
	case GD_CONFIG_PORT1:
		gd_conf->gd_svr1_port.value = char_to_int(cdata, clen);
		break;
	case GD_CONFIG_IP2:
		if(clen > GD_SVR2_IP_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_svr2_ip.value, cdata, clen);
		gd_conf->gd_svr2_ip.value[GD_SVR2_IP_LEN] = '\0';
		
		break;
	case GD_CONFIG_PORT2:
		gd_conf->gd_svr2_port.value = char_to_int(cdata, clen);		
		break;
	case GD_CONFIG_DNSSVR:
		if(clen > GD_DNS_SVR_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_dns_svr.value, cdata, clen);
		gd_conf->gd_dns_svr.value[GD_DNS_SVR_LEN] = '\0';
		
		break;
	case GD_CONFIG_BKPDNSSVR:
		if(clen > GD_BKP_DNS_SVR_LEN) 
		{
			error_command();
			return;

		}
		memcpy(gd_conf->gd_bkp_dns_svr.value, cdata, clen);
		gd_conf->gd_bkp_dns_svr.value[GD_BKP_DNS_SVR_LEN] = '\0';
		
		break;
	case GD_CONFIG_SVRCNT:
		if(*cdata < '0' || *cdata > '3') 
		{
			error_command();
			return;
		}	
		gd_conf->gd_svr_cnt.value = *cdata - '0';
		break;
		
	default:
		error_command();
		return;
	}
	command_ok();
}
void did_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	if(clen > GD_DEV_ID_LEN) 
	{
		error_command();
		return;

	}
	memcpy(gd_conf->gd_dev_id.value, cdata, clen);
	command_ok();

}
void sim_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	if(clen > GD_SIM_ID_LEN) 
	{
		error_command();
		return;

	}
	memcpy(gd_conf->gd_sim_id.value, cdata, clen);
	gd_conf->gd_sim_id.value[GD_SIM_ID_LEN] = '\0';
	command_ok();
}

void mode_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	if(check_string(cdata, "PROT", clen))
	{
		gd_conf->gd_work_mode.value = GD_WORK_MODE_PROT;
		command_ok();
		return;
	}
	if(check_string(cdata, "TTRN", clen))
	{
		gd_conf->gd_work_mode.value = GD_WORK_MODE_TTRN;
		command_ok();
		return;
	}
	if(check_string(cdata, "TUDP", clen))
	{
		gd_conf->gd_work_mode.value = GD_WORK_MODE_TUDP;
		command_ok();
		return;
	}

	error_command();
}
void pollint_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	
	gd_conf->gd_poll_int.value = char_to_int(cdata, clen);
	command_ok();
}
void retmain_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	if(*cdata == '0') 
	{
		gd_conf->gd_return_main.value = GD_RETURN_MAIN_NO;
		command_ok();		
		return;

	}
	else if(*cdata == '1') 
	{
		gd_conf->gd_return_main.value = GD_RETURN_MAIN_YES;
		command_ok();		
		return;

	}
	error_command();

}
void smsc_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	if(clen > GD_SMSC_LEN) 
	{
		error_command();
		return;

	}
	memcpy(gd_conf->gd_smsc.value, cdata, clen);
	gd_conf->gd_sim_id.value[GD_SIM_ID_LEN] = '\0';
	command_ok();
}
void dataint_config(INT8U *cdata, INT8U clen)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	
	gd_conf->gd_data_int.value = char_to_int(cdata, clen);
	command_ok();
}
void wakeup_config(INT8U *cdata, INT8U clen, INT8U ctype)
{
	if(ctype ==  GD_WAKE_PHONE_LEN)	
	{
		gd_config_info_t *gd_conf = NULL;
		gd_conf = &gd_system.gd_config_info;
		if(clen > GD_WAKE_PHONE_LEN) 
		{
			error_command();
			return;
	
		}
		memcpy(gd_conf->gd_wake_phone.value, cdata, clen);
		gd_conf->gd_wake_phone.value[GD_WAKE_PHONE_LEN] = '\0';
	}
	else if(ctype == GD_CONFIG_SMSPWD)
	{
		gd_config_info_t *gd_conf = NULL;
		gd_conf = &gd_system.gd_config_info;
		if(clen > GD_WAKE_SMS_LEN) 
		{
			error_command();
			return;
	
		}
		memcpy(gd_conf->gd_wake_sms.value, cdata, clen);
		gd_conf->gd_wake_sms.value[GD_WAKE_SMS_LEN] = '\0';
	}
	command_ok();
}

void com_config(INT8U *cdata, INT8U clen, INT8U ctype)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	if(ctype == GD_CONFIG_SERMODE)
	{
		if(check_string(cdata, "8N1", clen))
		{
			gd_conf->gd_serial_mode.value = GD_SERIAL_MODE_8N1;
			command_ok();
			return;
		}
		if(check_string(cdata, "8O1", clen))
		{
			gd_conf->gd_serial_mode.value = GD_SERIAL_MODE_8O1;
			command_ok();
			return;
		}
		if(check_string(cdata, "8E1", clen))
		{
			gd_conf->gd_serial_mode.value = GD_SERIAL_MODE_8E1;
			command_ok();
			return;
		}


	}
	else if(ctype == GD_CONFIG_COMBR)
	{
		gd_conf->gd_baud_rate.value = char_to_int(cdata, clen);
		command_ok();
		return;
	}
	error_command();
}

void config_require(void)
{
	INT32U *sendlen;
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
/*
+ZDID:aabbccdd							//DTU设备ID
+ZSIM:xxxxxxxxxx						//DTU SIM卡号码
+ZMODE:xxxx							//DTU工作模式
+ZACTI:xxxx								//DTU激活方式
+ZSERMODE:xxx							//DTU数据、校验及停止位
+ZBR:xxx								//DTU波特率
+ZRETMAIN:x								//DTU是否自动返回主中心
+ZDATAINT:xxx							//DTU数据帧间隔时间
+ZSVRCNT:x								//中心服务器数目
+ZIP:xxx     								//主中心服务器IP
+ZPORT:xxx  								//主中心服务器PORT
+ZBKPIP:xxx    							//备份中心服务器IP
+ZBKPPORT:xxx  							//备份中心服务器PORT
+ZIPn:xxx        							//中心服务器n的IP
+ZPORTn:xxx								//中心服务器n的PORT
+ZDNSSVR:aaa.bbb.ccc.ddd 				//主域名服务器IP
+ZBKPDNSSVR:aaa.bbb.ccc.ddd				//备份域名服务器IP
+ZAPN:xxxx								//APN接入点
+ZAPNUSER:xxx							//APN接入点用户名
+ZAPNPWD:xxx							//APN接入点密码
+ZAPNCENT:xxx							//APN拨号中心号码
+ZSMSC:xxx								//短信中心号码
+ZPOLLINT:xxx							//心跳包间隔时间（单位：秒）
+ZWP=xxx								//拨号唤醒号码
+ZSMSPWD=xxx							//短信唤醒密码
OK
*/	
	config_databuf.sendlen = strlen("+ZDID:");				
	strcpy(config_databuf.senddata, "+ZDID:");
	memcpy(config_databuf.senddata + config_databuf.sendlen, gd_conf->gd_dev_id.value, GD_DEV_ID_LEN);
	config_databuf.sendlen += GD_DEV_ID_LEN;
	config_databuf.senddata[config_databuf.sendlen++] = '\r';
	config_databuf.senddata[config_databuf.sendlen++] = '\n';
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	
	config_databuf.sendlen = strlen("+ZSIM:");
	strcpy(config_databuf.senddata, "+ZSIM:");
	strcat(config_databuf.senddata + config_databuf.sendlen, gd_conf->gd_sim_id.value);
//	config_databuf.sendlen += GD_SIM_ID_LEN;
	config_databuf.sendlen = strlen(config_databuf.senddata);
	config_databuf.senddata[config_databuf.sendlen++] = '\r';
	config_databuf.senddata[config_databuf.sendlen++] = '\n';	
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	strcpy(config_databuf.senddata, "+ZMODE:");
	if(gd_conf->gd_work_mode.value == GD_WORK_MODE_PROT)
	{
		strcat(config_databuf.senddata, "PROT\r\n");
	}
	else if(gd_conf->gd_work_mode.value == GD_WORK_MODE_TTRN)
	{
		strcat(config_databuf.senddata, "TTRN\r\n");
	}
	else if(gd_conf->gd_work_mode.value == GD_WORK_MODE_TUDP)
	{
		strcat(config_databuf.senddata, "TUDP\r\n");
	}
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	strcpy(config_databuf.senddata, "+ZACTI:");
	if(gd_conf->gd_acti_type.value == GD_ACTI_TYPE_AUTO)
	{
		strcat(config_databuf.senddata, "AUTO\r\n");
	}
	else if(gd_conf->gd_acti_type.value == GD_ACTI_TYPE_SMS)
	{
		strcat(config_databuf.senddata, "SMS\r\n");
	}
	else if(gd_conf->gd_acti_type.value == GD_ACTI_TYPE_PHONE)
	{
		strcat(config_databuf.senddata, "PHONE\r\n");
	}
	else if(gd_conf->gd_acti_type.value == GD_ACTI_TYPE_DATA)
	{
		strcat(config_databuf.senddata, "DATA\r\n");
	}
	else if(gd_conf->gd_acti_type.value == GD_ACTI_TYPE_MIX)
	{
		strcat(config_databuf.senddata, "MIX\r\n");
	}
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	strcpy(config_databuf.senddata, "+ZSERMODE:");
	if(gd_conf->gd_serial_mode.value == GD_SERIAL_MODE_8N1)
	{
		strcat(config_databuf.senddata, "8N1\r\n");
	}
	else if(gd_conf->gd_serial_mode.value == GD_SERIAL_MODE_8E1)
	{
		strcat(config_databuf.senddata, "8E1\r\n");
	}
	else if(gd_conf->gd_serial_mode.value == GD_SERIAL_MODE_8O1)
	{
		strcat(config_databuf.senddata, "8O1\r\n");
	}
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZBR:");
	strcpy(config_databuf.senddata, "+ZBR:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_baud_rate.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZRETMAIN:");
	strcpy(config_databuf.senddata, "+ZRETMAIN:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_return_main.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZDATAINT:");
	strcpy(config_databuf.senddata, "+ZDATAINT:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_data_int.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZSVRCNT:");
	strcpy(config_databuf.senddata, "+ZSVRCNT:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_svr_cnt.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZIP:");
	strcpy(config_databuf.senddata, "+ZIP:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_svr_ip.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZPORT:");
	strcpy(config_databuf.senddata, "+ZPORT:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_svr_port.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZBKPIP:");
	strcpy(config_databuf.senddata, "+ZBKPIP:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_bkp_svr_ip.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZBKPPORT:");
	strcpy(config_databuf.senddata, "+ZBKPPORT:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_bkp_svr_port.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZIP1:");
	strcpy(config_databuf.senddata, "+ZIP1:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_svr1_ip.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZPORT1:");
	strcpy(config_databuf.senddata, "+ZPORT1:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_svr1_port.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZIP2:");
	strcpy(config_databuf.senddata, "+ZIP2:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_svr2_ip.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZPORT2:");
	strcpy(config_databuf.senddata, "+ZPORT2:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_svr2_port.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZDNSSVR:");
	strcpy(config_databuf.senddata, "+ZDNSSVR:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_dns_svr.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZBKPDNSSVR:");
	strcpy(config_databuf.senddata, "+ZBKPDNSSVR:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_bkp_dns_svr.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZAPN:");
	strcpy(config_databuf.senddata, "+ZAPN:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_apn.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZAPNUSER:");
	strcpy(config_databuf.senddata, "+ZAPNUSER:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_apn_user.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZAPNPWD:");
	strcpy(config_databuf.senddata, "+ZAPNPWD:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_apn_pwd.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZAPNCENT:");
	strcpy(config_databuf.senddata, "+ZAPNCENT:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_apn_center.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZSMSC:");
	strcpy(config_databuf.senddata, "+ZSMSC:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_smsc.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZPOLLINT:");
	strcpy(config_databuf.senddata, "+ZPOLLINT:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%d\r\n", gd_conf->gd_poll_int.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZWP:");
	strcpy(config_databuf.senddata, "+ZWP:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_wake_phone.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

	config_databuf.sendlen = strlen("+ZSMSPWD:");
	strcpy(config_databuf.senddata, "+ZSMSPWD:");
	sprintf(config_databuf.senddata + config_databuf.sendlen, "%s\r\n", gd_conf->gd_wake_sms.value);
	config_databuf.sendlen = strlen(config_databuf.senddata);
	sendlen = suart_send_data(config_databuf.senddata, config_databuf.sendlen);
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(*sendlen == 0)	break;
	}

		
	
	command_ok();
	
}


void error_command(void)
{
	INT32U *sendlen;
	sendlen = suart_send_data("ERROR\r\n", 7);
//	while(1)
//	{
//		OSTimeDlyHMSM(0,0,0,1);
//		if(*sendlen == 0)	break;
//	}
}
void command_ok(void)
{
	INT32U *sendlen;
	sendlen = suart_send_data("OK\r\n", 4);
//	while(1)
//	{
//		OSTimeDlyHMSM(0,0,0,1);
//		if(*sendlen == 0)	break;
//	}

}
void at_command_ready(void)
{
	INT32U *sendlen;
	sendlen = suart_send_data("AT-Command Interpreter ready\r\n", strlen("AT-Command Interpreter ready\r\n"));
//	while(1)
//	{
//		OSTimeDlyHMSM(0,0,0,1);
//		if(*sendlen == 0)	break;
//	}

}
void sys_restart(void)
{
	gd_config_info_t *gd_conf = NULL;
	gd_conf = &gd_system.gd_config_info;
	gd_config_info_store(gd_conf);
	
	command_ok();
	
	system_reset();
}


