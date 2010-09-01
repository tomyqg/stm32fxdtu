/*
*********************************************************************************************************
*	��ΪGTM900Cģ��AT����                                             
*
*********************************************************************************************************
*/


/* Includes ------------------------------------------------------------------*/

#include "gtm900c.h"
#include <string.h>
#include <stdio.h>

#define	 GTM900C_TCPIP_SEND_DATALEN_MAX	1024

/**************************************************
���ڱ���tcp ip����֡��ʽ
GTM900C_TCPIP_IOMODE:	0 0 0 0  0 x x x
 						_ _ _ _  _ _ _ _
								   | | |___0: �������ݽ���ת��   1:�����ݽ���ת�� 	Ĭ�ϣ�0
								   | |___1:��ǰʹ�õ�����AT����	 2:��ǰʹ�ö�����AT����	  Ĭ�ϣ�1
								   |___ 0:ʹ�ý��ջ���	1����ʹ�ý��ջ���	Ĭ�ϣ�1

�ڳ�ʼ������s8	at_iomode(u8 mode)����
����u8	at_iomode_check(void)��ѯ				   
								      
**************************************************/
u8 GTM900C_TCPIP_IOMODE;


/**************************************************
���ַ���strǰlen�ֽ��в���ָ���ַ���flg
����str��flg��ʼλ��
û�з���NULL
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
ASCII �� HEX ��ת������
��ڲ����� O_data: ת�����ݵ����ָ�룬
		   N_data: ת���������ݵ����ָ��
		   len : ��Ҫת���ĳ���
���ز�����-1: ת��ʧ��
������ת�������ݳ���
ע�⣺O_data[]�����е�������ת�������лᱻ�޸ġ�
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
HEX �� ASCII ��ת������
��ڲ����� data: ת�����ݵ����ָ��
			buffer: ת�����������ָ��
			len : ��Ҫת���ĳ���
���ز�����ת�������ݳ���
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
�ַ� �� ���� ��ת������
��λ��ǰ��λ�ں������ǡ�0��~��9���ַ�����
��ڲ�����data �ַ���
		  len ����
���ز�����ת���������
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
s8	at_at(void)								//���Դ���
{
	u8 buf[4];
    at_send_data(AT_AT, sizeof(AT_AT));
 	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
	else	return -1;

}

s8	at_ate0(void)                             //�رջ���
{
	u8 buf[4];
    at_send_data(AT_ATE0, strlen(AT_ATE0));
 	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
	else	return -1;
}
s8	at_ate1(void)                             //�򿪻���
{
	u8 buf[4];
    at_send_data(AT_ATE1, strlen(AT_ATE0));
 	at_return_data(buf, 4);
	if(check_string(buf, "OK", 4))	return 0;
	else	return -1;
}
s8	at_tsim(void)                             //sim	 ��⵽sim������0
{
	u8 buf[9];
	at_send_data("AT%TSIM\r", strlen("AT%TSIM\r"));
	at_return_data(buf, 9);
	if(check_string(buf, "1", 9))	return 0;
 	else	return -1;
}
/*****************************************
�����ź�ǿ��
��ڲ�������
���ز������ź�ǿ��ֵ 0 ~ 31
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
����ip���ݰ�ģʽ
��ڲ����� mode	����λ�ֱ��Ӧ(�ӵ׵���)����ת�������ӡ�����
���ز����� ��ȷ����0
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
��ѯip���ݰ�ģʽ����
��ڲ����� 
���ز����� �ɹ�����mode	����λ�ֱ��Ӧ����ת�������ӡ�����
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
	//����ת��
	p = check_string(buf, ",", 17);
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

s8	gtm900c_init(void)
{
	s8 res = 0;
	u8 buf[30];
	//ģ������\����������ȴ�6s
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
tcp/ip��ʼ��
��ڲ�����user �û��� �������10
		  password ����	�������10
		  ����Ϊ�� 
���ز�������ȷ����0
*******************************************/
s8	gtm900c_tcpip_init(u8 *user, u8 *password)                    
{
	u8 len = 0;
	u8 buf[40];
	if(strlen(user)>10 || strlen(password))	return 1;
	//����apn
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
����tcpip����֡ģʽ
����GTM900C_TCPIP_IOMODE�б��浱ǰģʽ����
��ǰʹ�û���
��ڲ����� mode	����λ�ֱ��Ӧ(�ӵ׵���)����ת�������ӡ�����
���ز����� ��ȷ����0
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
tcp/ip����һ������	��������ģʽ��
��ڲ�����type 0-tcp, 1-udp 
		  ip ip��ַ�����Ȳ�����15
		  dest_port Ŀ��˿� ���5λ
		  udp_dest_port udp�Զ� ���5λ
		  local_port ���ض˿� ���5λ
���ز�������ȷ����0
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
tcp/ip����һ������	��������ģʽ��
��ڲ�����type 0-tcp, 1-udp 
		  ip ip��ַ�����Ȳ�����15
		  dest_port Ŀ��˿� ���5λ
		  local_port ���ض˿� ���5λ
		  udp_dest_port udp�Զ� ���5λ
		  link_num ������� 
���ز�������ȷ����0
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
tcp/ip�ر����ӡ�ע��gprs���رշ�����	 
��ڲ�����link_num 1~3������� 5ע��gprs 6�رշ�����
���ز�������ȷ����0
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
tcp/ip ���ݷ���
��ڲ�����data ����buffer��ַ 
		  len  ����	���GTM900C_TCPIP_SEND_DATALEN_MAX
		  link_num ������ģʽʱ������� ������ģʽ�������ã��ɸ�ֵ0 
���ز�������ȷ����0
*******************************************/
s8	gtm900c_tcpip_send(u8 *data, u16 len, u8 link_num)
{
	u8 databuf[GTM900C_TCPIP_SEND_DATALEN_MAX*2+15];
	u16 databuf_len;
//	memset(buf, 0, GTM900C_TCPIP_SEND_DATALEN_MAX*2);
	if(len > GTM900C_TCPIP_SEND_DATALEN_MAX)  return 1;
	//ѡ��\�����ӣ�����
	if(GTM900C_TCPIP_IOMODE & 0x02)
	{
		strcpy(databuf, "AT%IPSENDX=");
	}
	else
	{
		strcpy(databuf, "AT%IPSEND=");
	}
	databuf_len = strlen(databuf);
	//����ת��
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
tcp/ip ���ݽ���(ʹ�û���)
	   ���ݲ�ѯ ���� ɾ��
*******************************************/

/*****************************************
tcp/ip ���ݽ��ջ�������ѯ
��ڲ�����unread ���ڷ���δ�����ݰ���
		  total	 ���ڷ��ػ��������ݰ�����
���ڲ�����
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
tcp/ip ���ݽ��ջ�����ɾ��
��ڲ�����
		 
���ڲ�����
*******************************************/
s8	gtm900c_tcpip_recvbuf_delete(u8 )



