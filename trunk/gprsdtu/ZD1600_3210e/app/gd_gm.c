
#include "gd_gm.h"

#include <ucos_ii.h>
#include <string.h>
#include "../drives/uart_drv.h"
#include "gd_system.h"

#define sendAT(s, l)	guart_send_data((s), (l))
#define RECVAT_TIMEOUT	1000


gprs_databuf_t	gprs_databuf;
u8 GPRS_TCPIP_IOMODE;


u16 recvAT(void)
{
	u16 time_count = 0;
	gprs_databuf.recvlen = 0;
 	while(gprs_databuf.recvlen == 0) {
		OSTimeDlyHMSM(0, 0, 0, 10);
		time_count++;
		if(time_count >= RECVAT_TIMEOUT) return 0;
		}
	return 	gprs_databuf.recvlen;
}
void delays(u8 n_second)
{
	OSTimeDlyHMSM(0, 0, n_second, 0);	
}
/*
u8 gprs_sem_pend(u16 timeout)
{
	INT8U err;
	OSSemPend(gd_system.gm_operate_sem, timeout, &err);
	return err;
}
u8 gprs_sem_post(void)
{
	INT8U err;
	err = OSSemPost(gd_system.gm_operate_sem);
	return err;
}




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
	sendAT("AT\r\n", sizeof("AT\r\n"));
 	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
	else	 return -1;

}

s8	at_ate0(void)                             //�رջ���
{
	sendAT("ATE0\r\n", strlen("ATE0\r\n"));
 	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
	else 	return -1;
}
s8	at_ate1(void)                             //�򿪻���
{
	sendAT("ATE1\r\n", strlen("ATE1\r\n"));
 	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
	else 	return -1;
}
s8	at_tsim(void)                             //sim	 ��⵽sim������0
{
	sendAT("AT%TSIM\r\n", strlen("AT%TSIM\r\n"));
 	recvAT();
	if(check_string(gprs_databuf.recvdata, "%TSIM 1", gprs_databuf.recvlen)){
 		recvAT();
		return 0;
		}
	recvAT()  ;
	return -1;
}

/*****************************************
�����ź�ǿ��
��ڲ�������
���ز������ź�ǿ��ֵ 0 ~ 31
*******************************************/
s8	at_csq(void)                            
{
	u8  *p, csq;
    	sendAT("AT+CSQ\r\n", strlen("AT+CSQ\r\n"));
	recvAT() ;
//	if(check_string(gprs_databuf.recvdata, "OK", 18) == NULL)	return -1;
	p =  check_string(gprs_databuf.recvdata, ",", gprs_databuf.recvlen);
	if(p)	{
		csq = (*(p-2)*10 + *(p-1));
		recvAT();
		return csq;
		}	
	recvAT();
	return -1;
}
/*****************************************
����ip���ݰ�ģʽ
��ڲ����� mode	����λ�ֱ��Ӧ(�ӵ׵���)����ת�������ӡ�����
���ز����� ��ȷ����0
*******************************************/
s8	at_iomode(u8 mode)                    
{
	sprintf(gprs_databuf.senddata, "AT%%IOMODE=%d,%d,%d\r\n", mode&0x01, ((mode&0x02)>>1)+1, (mode&0x04)>>2);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
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
	sendAT("AT%IOMODE?\r\n", strlen("AT%IOMODE?\r\n"));
	recvAT();
//	if(check_string(gprs_databuf.recvdata, "OK", 13) == NULL)	return 8;
	//����ת��
	p = check_string(gprs_databuf.recvdata, ",", gprs_databuf.recvlen);
	if(p == NULL) return 9;
	if(*(p-1) == '1') mode |= 0x01;
	else if(*(p-1) != '0') return 10;
	//����
	p = check_string(p+1, ",", 17);
	if(p == NULL) return 11;
	if(*(p-1) == '2') mode |= 0x02;
	else if(*(p-1) != '1') return 12;
	//����
//	p = check_string(p+1, ",", 17);
//	if(p == NULL) return 13;
	if(*(p+1) == '1') mode |= 0x04;
	else if(*(p+1) != '0') return 14;
	recvAT();
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
	sendAT("AT%MSO\r\n", strlen("AT%MSO\r\n"));
}
/*****************************************
ģ�鸴λ 
��ڲ���:  
���ز���:
*******************************************/
s8 gprsmodule_reset(void)
{
	sendAT("AT%RST\r\n", strlen("AT%RST\r\n"));
	recvAT();
	if(check_string(gprs_databuf.recvdata, "Interpreter ready", gprs_databuf.recvlen))	
	{
		return 0;
	}	
	return 1;
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
	sprintf(gprs_databuf.senddata, "AT+IPR=%d,\r\n", rate);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen) == NULL)	return 1;
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
	//ģ������\����������ȴ�6s	 //�������з�����at�����
//	res = at_at();
//	if(res != 0) return 1;
	res = at_ate0();
	if(res != 0) return 2;
	delays(1);
	res = at_tsim();
	if(res != 0) return 3;

	//AT+CREG  //////  ����һ������
	sendAT("AT+CREG?\r\n", strlen("AT+CREG?\r\n"));
	recvAT();
//	if(check_string(gprs_databuf.recvdata, "", 10) == NULL)	return 5;
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", 2) == NULL)	return 4;
	delays(2);

	//AT+COPS?  
	sendAT("AT+COPS?\r\n", strlen("AT+COPS?\r\n"));
	recvAT();
	if(check_string(gprs_databuf.recvdata, /*���ƶ�*/"CHINA  MOBILE", gprs_databuf.recvlen) == NULL)	return 5;
	recvAT();

	//AT+CGATT?
	sendAT("AT+CGATT?\r\n", strlen("AT+CGATT?\r\n"));
	recvAT();
	if(check_string(gprs_databuf.recvdata, "+CGATT: 1", gprs_databuf.recvlen) == NULL)	return 5; 
	recvAT(); 

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
	sendAT("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", strlen("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n"));
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", 2) == NULL)	return 1;
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
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\r';
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\n';
	gprs_databuf.senddata[gprs_databuf.sendlen++] = '\0'; 
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
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
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,",\"%s\",%d,%d,%d\r\n", ip, dest_port, udp_dest_port, local_port);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT();
	if(check_string(gprs_databuf.recvdata, "CONNECT", gprs_databuf.recvlen))	return 0;
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
	sprintf(gprs_databuf.senddata+gprs_databuf.sendlen,",\"%s\",%d,%d,%d\r\n", ip, dest_port, udp_dest_port, local_port);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
 	else	return 4;	 

}
/*****************************************
tcp/ip�ر����ӡ�ע��gprs���رշ�����	 
��ڲ�����link_num 1~3������� 5ע��gprs 6�رշ�����
���ز�������ȷ����0
*******************************************/
s8	gprs_tcpip_close_connection(u8 link_num)
{
	sprintf(gprs_databuf.senddata,"AT%%IPCLOSE=%d,\r\n", link_num);
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	return 0;
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
		sprintf(gprs_databuf.senddata, "AT%%IPSENDX=%d,\"", link_num);
	}
	else
	{
		strcpy(gprs_databuf.senddata, "AT%IPSEND=\"");
	}
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	//����ת��
	if(GPRS_TCPIP_IOMODE & 0x01)
	{
		hex_to_ascii(data, gprs_databuf.senddata+gprs_databuf.sendlen, len);
	}
	else
	{
	 	memcpy(gprs_databuf.senddata+gprs_databuf.sendlen, data, len);
	}
	strcat(gprs_databuf.senddata, "\"\r\n");
	gprs_databuf.sendlen = strlen(gprs_databuf.senddata);
	sendAT(gprs_databuf.senddata, gprs_databuf.sendlen);
	recvAT();
	recvAT();
	if(check_string(gprs_databuf.recvdata, "OK", gprs_databuf.recvlen))	
		return 0;
 	else	
		return 2;

}



/*****************************************
tcp/ip ���ݽ���(ʹ�û���)
��ڲ���:data_index_p ���ڴ�����ݰ���ţ�0ʱĬ�ϵ�һ��δ������
		 link_num_p  ���ڴ���������
		 data_len_p	 ���ڴ�����ݳ���
		 recvdata ���ڴ�Ž��յ�������
���ڲ���:��ȷ����0
*******************************************/
s8 gprs_tcpip_request_data(u8 index, u8 *data_index_p, u8 *link_num_p, u16 *data_len_p, u8 *recvdata)
{
	u8	*pF, *pB;
	sprintf(gprs_databuf.senddata, "AT%%IPDR=%d\r\n", index);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT();
	if(check_string(gprs_databuf.recvdata, "ERROR:", 6))	return 1;
	pF = check_string(gprs_databuf.recvdata, ":", 9);
	pB = check_string(gprs_databuf.recvdata, ",", 9);
	if((pF == NULL) || (pB == NULL)) return 2;
	*link_num_p =  char_to_int(pF+1, pB-pF);
	pF = pB+1;
	pB = check_string(pF, ",", 6);
	if(pB == NULL) return 3;
	*data_index_p =  char_to_int(pF, pB-pF);
	pF = pB+1;
	pB = check_string(pF, ",", 6);
	if(pB == NULL) return 4;
	*data_len_p =  char_to_int(pF, pB-pF);
	pF = pB+2;
	ascii_to_hex(pF, recvdata, (*data_len_p)*2);
	recvAT();
	return 0;
}

/*****************************************
tcp/ip ���ݽ��ջ�������ѯ
��ڲ�����unread ���ڷ���δ�����ݰ���
		  		total	 ���ڷ��ػ��������ݰ�����
���ڲ�����
*******************************************/
s8	gprs_tcpip_recvbuf_query(u8 *unread, u8 *total)
{
	u8	*pF, *pB;
	strcpy(gprs_databuf.senddata, "AT%IPDQ\r\n");
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT();
	pF = check_string(gprs_databuf.recvdata, ":", gprs_databuf.recvlen);
	pB = check_string(gprs_databuf.recvdata, ",", gprs_databuf.recvlen);
	if((pF == NULL) || (pB == NULL)) return 2;
	*unread =  char_to_int(pF+1, pB-pF);
	pF = check_string(gprs_databuf.recvdata, ",", 15);
	pB = check_string(gprs_databuf.recvdata, "OK", 20);
	if((pF == NULL) || (pB == NULL)) return 3;
	*total =  char_to_int(pF, pB-pF);
	recvAT();
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
	u8  *pF, *pB, res;
	if(index>50 || type>2)	return -1;	
	sprintf(gprs_databuf.senddata,"AT%%IPDD=%d,%d\r\n", index, type);
	sendAT(gprs_databuf.senddata, strlen(gprs_databuf.senddata));
	recvAT();	
	pF =  check_string(gprs_databuf.recvdata, ":", gprs_databuf.recvlen);
	if(pF == NULL) return -2;
	pB =  gprs_databuf.recvdata;
	res = char_to_int(pF, gprs_databuf.recvlen - (pF-pB));
	recvAT();
	return res;
	
}

/**********************************************
tcp/ip ���ݽ��ջ�����ɾ��ģʽ������
		����Ĭ�ϣ��Զ�ɾ��
**********************************************/


/*****************************************
ģ����������봦��
		��gprs���յ�����
		���ӶϿ�
��ڲ����� 
���ڲ����� 
*******************************************/
s8	gprs_unrequest_code_dispose(u32 len)
{
	u8	*p, res;
	if(len < 6)	return -1;
	p = check_string(gprs_databuf.recvdata, "%IPDATA:", len);
	if(p)	return GM_TCPIP_RECIEVED_DATA;
 	p = check_string(gprs_databuf.recvdata, "%IPCLOSE:", len);
	if(p)	
	{
		res	= char_to_int(p+9, 1);
		return (res+1); 
	}
	return -2; 
}






