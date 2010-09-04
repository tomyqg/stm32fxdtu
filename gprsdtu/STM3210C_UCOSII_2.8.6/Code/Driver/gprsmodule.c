
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
    at_send_data("AT\r", sizeof("AT\r"));
 	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
	else	return -1;

}

s8	at_ate0(void)                             //�رջ���
{
    at_send_data("ATE0\r", strlen("ATE0\r"));
 	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
	else	return -1;
}
s8	at_ate1(void)                             //�򿪻���
{
	at_send_data("ATE1\r", strlen("ATE1\r"));
 	while(gprs_databuf.recvlen < 3);
	if(check_string(gprs_databuf.recvdata, "OK", 3))	return 0;
	else	return -1;
}
s8	at_tsim(void)                             //sim	 ��⵽sim������0
{
	at_send_data("AT%TSIM\r", strlen("AT%TSIM\r"));
 	while(gprs_databuf.recvlen < 7);
	if(check_string(gprs_databuf.recvdata, "1", 7))	return 0;
 	else	return -1;
}

/*****************************************
�����ź�ǿ��
��ڲ�������
���ز������ź�ǿ��ֵ 0 ~ 31
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
����ip���ݰ�ģʽ
��ڲ����� mode	����λ�ֱ��Ӧ(�ӵ׵���)����ת�������ӡ�����
���ز����� ��ȷ����0
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
��ѯip���ݰ�ģʽ����
��ڲ����� 
���ز����� �ɹ�����mode	����λ�ֱ��Ӧ����ת�������ӡ�����
*******************************************/
u8	at_iomode_check(void)                    
{
	u8 mode = 0;
	u8 *p;
	at_send_data("AT%IOMODE?\r", strlen("AT%IOMODE?\r"));
	while(gprs_databuf.recvlen < 17);
	if(check_string(gprs_databuf.recvdata, "OK", 17) == NULL)	return 8;
	//����ת��
	p = check_string(gprs_databuf.recvdata, ",", 17);
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
/********************************************************
//ģ���ʼ�� 
�ɹ�����
********************************************************/

s8	gprsmodules_init(void)
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
tcp/ip��ʼ��
��ڲ�����user �û��� �������10
		  password ����	�������10
		  ����Ϊ�� 
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_init(u8 *user, u8 *password)                    
{
	u8 len = 0;
	u8 buf[40];
	if(strlen(user)>10 || strlen(password))	return 1;
	//����apn
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
tcp/ip�ر����ӡ�ע��gprs���رշ�����	 
��ڲ�����link_num 1~3������� 5ע��gprs 6�رշ�����
���ز�������ȷ����0
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
tcp/ip ���ݷ���
��ڲ�����data ����buffer��ַ 
		  len  ����	���GPRS_DATA_LEN_MAX
		  link_num ������ģʽʱ������� ������ģʽ�������ã��ɸ�ֵ0 
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_send(u8 *data, u16 len, u8 link_num)
{
	u8 databuf[GPRS_DATA_LEN_MAX*2+15];
	u16 databuf_len;
//	memset(buf, 0, GPRS_DATA_LEN_MAX*2);
	if(len > GPRS_DATA_LEN_MAX)  return 1;
	//ѡ��\�����ӣ�����
	if(GPRS_TCPIP_IOMODE & 0x02)
	{
		strcpy(databuf, "AT%IPSENDX=");
	}
	else
	{
		strcpy(databuf, "AT%IPSEND=");
	}
	databuf_len = strlen(databuf);
	//����ת��
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
tcp/ip ���ݽ��ջ�����ɾ��
��ڲ����� index ���ݰ����0-50
		   type ���� 0��1��2 ��index=0ʱ��Ч 0ɾ�Ѷ� 1ɾδ�� 2ȫ��
���ڲ����� �ɹ�����ɾ�������ݰ����� ���󷵻�ֵ<0
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
tcp/ip ���ݽ��ջ�����ɾ��ģʽ������
		����Ĭ�ϣ��Զ�ɾ��
**********************************************/

