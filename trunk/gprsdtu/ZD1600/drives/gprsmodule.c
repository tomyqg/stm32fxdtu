#include <ucos_ii.h>
#include <string.h>
#include "gprsmodule.h"
#include "uart_drv.h"

#define sendAT	uart2_senddata

gprs_databuf_t	gprs_databuf;
u8 GPRS_TCPIP_IOMODE;


void recvAT(u32 len)
{
	gprs_databuf.recvlen = 0;
 	while(gprs_databuf.recvlen < len) {
		OSTimeDlyHMSM(0, 0, 0, 50);
		}

}

/*
s8	sendAT(u8 *data, u16 len)//send data
{
	u16	i = 0;
	while(i<len)
	{
		usart_send_char(data[i++]);//

	}

	//usart2 ����gprs module
 	
	
	return 0;
}
*/
//���ո�Ϊ�жϷ�ʽ ͨ��gprs_databuf
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
s8	at_at(void)								//���Դ���
{
	sendAT("AT\n\r", sizeof("AT\n\r"));
 	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
	else	 return -1;

}

s8	at_ate0(void)                             //�رջ���
{
	sendAT("ATE0\n\r", strlen("ATE0\n\r"));
 	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
	else 	return -1;
}
s8	at_ate1(void)                             //�򿪻���
{
	sendAT("ATE1\n\r", strlen("ATE1\n\r"));
 	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
	else 	return -1;
}
s8	at_tsim(void)                             //sim	 ��⵽sim������0
{
	sendAT("AT%TSIM\n\r", strlen("AT%TSIM\n\r"));
 	recvAT(19);
	if(check_string(gprs_databuf.recvdata, "%TSIM 1", 19))	return 0;
 	else 	return -1;
}

/*****************************************
�����ź�ǿ��
��ڲ�������
���ز������ź�ǿ��ֵ 0 ~ 31
*******************************************/
s8	at_csq(void)                            
{
	u8  *p;
    	sendAT("AT+CSQ\n\r", strlen("AT+CSQ\n\r"));
 	recvAT(22);
	if(check_string(gprs_databuf.recvdata, "OK", 22) == NULL)	return -1;
	p =  check_string(gprs_databuf.recvdata, ",", 22);
	if(p)	return (*(p-2)*10 + *(p-1));
	else	 return -1;
}
/*****************************************
����ip���ݰ�ģʽ
��ڲ����� mode	����λ�ֱ��Ӧ(�ӵ׵���)����ת�������ӡ�����
���ز����� ��ȷ����0
*******************************************/
s8	at_iomode(u8 mode)                    
{
	sprintf(gprs_databuf.senddata, "AT%%IOMODE=%d,%d,%d\n\r", mode&0x01, (1+(mode&0x02)>>1), (mode&0x04)>>2);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
 	else 	return -1;
}
/*****************************************
��ѯip���ݰ�ģʽ����
��ڲ����� 
���ز����� �ɹ�����mode	����λ�ֱ��Ӧ����ת�������ӡ�����
*******************************************/
u8	at_iomode_check(void)                    
{
	u8 mode = 0;
	u8 *p;
	sendAT("AT%IOMODE?\n\r", strlen("AT%IOMODE?\n\r"));
	recvAT(17);
	if(check_string(gprs_databuf.recvdata, "OK", 24) == NULL)	return 8;
	//����ת��
	p = check_string(gprs_databuf.recvdata, ",", 24);
	if(p == NULL) return 9;
	if(*(p-1) == '1') mode |= 0x01;
	else if(*(p-1) != '0') return 10;
	//����
	p = check_string(p+1, ",", 17);
	if(p == NULL) return 11;
	if(*(p-1) == '2') mode |= 0x02;
	else if(*(p-1) != '1') return 12;
	//����
	p = check_string(p+1, ",", 17);
	if(p == NULL) return 13;
	if(*(p-1) == '1') mode |= 0x04;
	else if(*(p-1) != '0') return 14;

	return mode;
 	
}



/*=======================================================*/	 



/*****************************************
ģ��ػ�
��ڲ���:  
���ز���:
*******************************************/
void gprsmodule_shutdown(void)
{
	sendAT("AT%MSO\n\r", strlen("AT%MSO\n\r"));
}


/*****************************************
ģ�鴮������
��ڲ�����user �û��� �������10
		  password ����	�������10
		  ����Ϊ�� 
���ز�������ȷ����0
*******************************************/
s8 gprs_bps_set(u32 rate)
{
	sprintf(gprs_databuf.senddata, "AT+IPR=%d,\n\r", rate);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6) == NULL)	return 1;
	return 0;
}


/*****************************************
ģ���ʼ��
��ڲ�����
���ز�����
*******************************************/

s8	gprsmodule_init(void)
{
	s8 res = 0;
	//ģ������\����������ȴ�6s
	res = at_at();
	if(res != 0) return 1;
	res = at_ate0();
	if(res != 0) return 2;
	res = at_tsim();
	if(res != 0) return 3;
	//AT+CREG
	sendAT("AT+CREG=1\n\r", strlen("AT+CREG=1\n\r"));
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6) == NULL)	return 4;
	//AT+COPS?  
	sendAT("AT+COPS?\n\r", strlen("AT+COPS?\n\r"));
	recvAT(28);
	if(check_string(gprs_databuf.recvdata, /*���ƶ�*/"CHINA MOBILE", 28) == NULL)	return 5;
	//AT+CGATT?
	sendAT("AT+CGATT?\n\r", strlen("AT+CGATT?\n\r"));
	recvAT(19);
	if(check_string(gprs_databuf.recvdata, "1", 19) == NULL)	return 5;  

	return 0;
}

/*****************************************
tcp/ip��ʼ��
��ڲ�����user �û��� �������10
		  password ����	�������10
		  ����Ϊ�� 
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_init(u8 *user, u8 *password)                    
{
	if(strlen(user)>10 || strlen(password)>10)	return 1;
	//����apn
	sendAT("AT+CGDCONT=1,\"IP\",\"CMNET\"\n\r", strlen("AT+CGDCONT=1,\"IP\",\"CMNET\"\n\r"));
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6) == NULL)	return 1;
	//tcpip enable
	strcpy(gprs_databuf.senddata, "AT%ETCPIP");
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	if(user)
	{
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '=';
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
		strcpy(gprs_databuf.senddata+gprs_databuf.sendlen, user);
		gprs_databuf.sendlen += strlen(user);
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
		gprs_databuf.senddata[gprs_databuf.sendlen++] = ',';
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
		strcpy(gprs_databuf.senddata+gprs_databuf.sendlen, password);
		gprs_databuf.senddata[gprs_databuf.sendlen++] = '\"';
		gprs_databuf.sendlen = strlen(gprs_databuf.senddata);	
	}
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\r\n';
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\0'; 
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
 	else	return 2;
}

/*****************************************
����tcpip����֡ģʽ
����GPRS_TCPIP_IOMODE�б��浱ǰģʽ����
��ǰʹ�û���
��ڲ����� mode	����λ�ֱ��Ӧ(�ӵ׵���)����ת�������ӡ�����
���ز����� ��ȷ����0
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
tcp/ip����һ������	��������ģʽ��
��ڲ�����type 0-tcp, 1-udp 
		  ip ip��ַ�����Ȳ�����15
		  dest_port Ŀ��˿� ���5λ
		  udp_dest_port udp�Զ� ���5λ
		  local_port ���ض˿� ���5λ
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_creat_connection(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port)                    
{
	if(strlen(ip)>15)	return 1;
	strcpy(gprs_databuf.senddata, "AT%IPOPEN=");
	if(type == 1)	strcat(gprs_databuf.senddata, "\"UDP\"");	
	else if(type == 0)	strcat(gprs_databuf.senddata, "\"TCP\"");
	else return 2;
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,",\"%s\",%d,%d,%d\n\r", ip, dest_port, udp_dest_port, local_port);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
 	else	return 3;	 


}
/*****************************************
tcp/ip����һ������	��������ģʽ��
��ڲ�����type 0-tcp, 1-udp 
		  ip ip��ַ�����Ȳ�����15
		  dest_port Ŀ��˿� ���5λ
		  local_port ���ض˿� ���5λ
		  udp_dest_port udp�Զ� ���5λ
		  link_num ������� 
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_creat_connection_n(u8 type, u8 *ip, u16 dest_port, u16 udp_dest_port, u16 local_port, u8 link_num)                    
{
	if(strlen(ip)>15)	return 1;
	strcpy(gprs_databuf.senddata, "AT%IPOPEN=");
	if(link_num==0 || link_num>3)	return 2;
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,"%d,", link_num);
	if(type == 1)	strcat(gprs_databuf.senddata, "\"UDP\"");	
	else if(type == 0)	strcat(gprs_databuf.senddata, "\"TCP\"");
	else return 3;
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,",\"%s\",%d,%d,%d\n\r", ip, dest_port, udp_dest_port, local_port);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
 	else	return 4;	 

}
/*****************************************
tcp/ip�ر����ӡ�ע��gprs���رշ�����	 
��ڲ�����link_num 1~3������� 5ע��gprs 6�رշ�����
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_close_connection(u8 link_num)
{
	sprintf(gprs_databuf.senddata,"AT%%IPCLOSE=%d,\n\r", link_num);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
 	else	return 1;

} 

/*****************************************
tcp/ip ���ݷ���
��ڲ�����data ����buffer��ַ 
		  len  ����	���GPRS_DATA_LEN_MAX
		  link_num ������ģʽʱ�������
		  		  ������ģʽ�������ã��ɸ�ֵ0 
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_send(u8 *data, u16 len, u8 link_num)
{
//	memset(buf, 0, GPRS_DATA_LEN_MAX*2);
	if(len > GPRS_DATA_LEN_MAX)  return 1;
	//ѡ��\�����ӣ�����
	if(GPRS_TCPIP_IOMODE & 0x02)
	{
		sprintf(gprs_databuf.senddata, "AT%%IPSENDX=%d,", link_num);
	}
	else
	{
		strcpy(gprs_databuf.senddata, "AT%IPSEND=");
	}
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	//����ת��
	if(GPRS_TCPIP_IOMODE & 0x01)
	{
		gprs_databuf.sendlen += hex_to_ascii(data, gprs_databuf.senddata+gprs_databuf.sendlen, len);	
	}
	else
	{
	 	memcpy(gprs_databuf.senddata+gprs_databuf.sendlen, data, len);
		gprs_databuf.sendlen += len;
	}
	strcat(gprs_databuf.senddata, "\"\n\r");
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(6);
	if(check_string(gprs_databuf.recvdata, "OK", 6))	return 0;
 	else	return 2;

}



/*****************************************
tcp/ip ���ݽ���(ʹ�û���)
	   ���ݲ�ѯ ���� ɾ��
*******************************************/

/*****************************************
tcp/ip ���ݽ��ջ�������ѯ
��ڲ�����unread ���ڷ���δ�����ݰ���
		  		total	 ���ڷ��ػ��������ݰ�����
���ڲ�����
*******************************************/
s8	gprs_tcpip_recvbuf_query(u8 *unread, u8 *total)
{
	u8	*pF, *pB;
	strcpy(gprs_databuf.senddata, "AT%IPDQ\n\r");
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(20);
	pF = check_string(gprs_databuf.recvdata, ":", 20);
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
tcp/ip ���ݽ��ջ�����ɾ��
��ڲ����� index ���ݰ����0-50
		   		type ���� 0��1��2 ��index=0ʱ��Ч 0ɾ�Ѷ� 1ɾδ�� 2ȫ��
���ڲ����� �ɹ�����ɾ�������ݰ����� ���󷵻�ֵ<0
*******************************************/
s8	gprs_tcpip_recvbuf_delete(u8 index, u8 type)
{
	u8  *pF, *pB;
	if(index>50 || type>2)	return -1;	
	sprintf(gprs_databuf.senddata,"AT%%IPDD=%d,%d\n\r", index, type);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT(17);
	pB =  check_string(gprs_databuf.recvdata, "OK", 17);
	pF =  check_string(gprs_databuf.recvdata, ":", 17);
	if(~(pF&&pB)) return -2;
	return char_to_int(pF, pB-pF);
	
}

/**********************************************
tcp/ip ���ݽ��ջ�����ɾ��ģʽ������
		����Ĭ�ϣ��Զ�ɾ��
**********************************************/



