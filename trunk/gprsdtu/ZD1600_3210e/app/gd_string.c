
#include "gd_string.h"
#include <string.h>






/**************************************************
���ַ���strǰlen�ֽ��в���ָ���ַ���flg
����str��flg��ʼλ��
û�з���NULL
**************************************************/
u8* check_string(u8 *str, u8 *flg, u32 len)
{
/*	u8	*p;
	u32	i;
	u32 flg_len = strlen(flg);
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
*/
	u32	i,j;
	u32 flg_len = strlen(flg);
	for(i=0; i<len; i++)
	{
		if(str[i] == flg[0])
		{
			for(j=0; j<flg_len; j++)
			{
				if(str[i+j] != flg[j])
					break;
			}
			if(j == flg_len)
				return str+i;
		}
	}
	return 0;

}

/*****************************************
ASCII �� HEX ��ת������
��ڲ����� O_data: ת�����ݵ����ָ�룬
		   N_data: ת���������ݵ����ָ��
		   len : ��Ҫת���ĳ���
���ز�����0: ת��ʧ��
������ת�������ݳ���
ע�⣺O_data[]�����е�������ת�������лᱻ�޸ġ�
****************************************/
u32 ascii_to_hex(u8 *O_data, u8 *N_data, u32 len)
{
	u32 i,j,tmp_len;
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
			return 0;
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
u32 hex_to_ascii(u8 *data, u8 *buffer, u32 len)
{
	uc8 ascTable[17] = {"0123456789ABCDEF"};
	u8 *tmp_p = buffer;
	u32 i, pos;
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
u32 char_to_int(u8 *cdata, u32 len)
{
	u8 i;
	u32 ndata;
	ndata = 0;
	for(i=0; i<len; i++)
	{
		if(cdata[i]>='0' && cdata[i]<='9')
		{
			ndata = (ndata * 10) + (cdata[i] -'0');
		}
		else
			return ndata;
	}
	return ndata;
}

/*******************************************************************************************************
WS-120MЭ�飬16λУ���
��ڲ���: 	addr ���ݵ�ַ
			count���ݳ���
���ڲ���:	16λУ���
********************************************************************************************************/
u16 check_sum_16(u8 *addr, u16 count)
{
        /* Compute Internet Checksum for "count" bytes
         * * beginning at location "addr".
         * */
        register long sum = 0;

        while( count > 1 )
        {
                /* This is the inner loop */
                sum += *((unsigned short *)addr)++;
                count -= 2;
        }
        /* Add left-over byte, if any */
        if( count > 0 )
                sum += *((unsigned char *)addr);

        /* Fold 32-bit sum to 16 bits */
        while (sum>>16)
                sum = (sum & 0xffff) + (sum >> 16);

//      checksum = ~sum;
        return (u16)(~sum);
}




/*******************************************************************************************************
//WS-120MЭ�飬��ʼ��������
//��ڲ���: 	dev_id	 �豸ID
//			dev_mac	 �豸MAC
//			data_type �������� �������̶�Ϊ0
//			extra_data ��������
//			extra_len  �������ݳ���
//			wifi_signal �ź�ǿ��
//			buf		 ��ʼ��������ݴ��buf
//���ڲ���:	len		 ��ʼ��������ݳ���
********************************************************************************************************/
u32	gd_heart_beat_init(uc8 *dev_id, uc8 *dev_mac, u8 *extra_data, 
										u8 extra_len, u8 wifi_signal, u8 *buf)
{
	u16	checksum;						

	sprintf(buf,"%s%c%-8.8s%c%c",GD_FRAME_HEAD_FLG, WS_120M_HEARTBEAT, dev_id,
										((extra_len+8)>>8)&0xff,(extra_len+8)&0xff);
	
 	checksum=check_sum_16(buf, 18);
	sprintf(buf+18,"%c%c", (checksum>>8)&0xff, checksum&0xff);
	memcpy(buf+20,dev_mac,6);
	sprintf(buf+26,"%c%c",wifi_signal,extra_len);
	memcpy(buf+28,extra_data,extra_len);
				
	return (28+extra_len);

}
/*******************************************************************************************************
//WS-120MЭ�飬��ʼ�����ݰ�
//��ڲ���: 	dev_id	 �豸ID
//			data_type�������ͣ�0���� 1��ҵ����
//			data	 ����
//			data_len ���ݳ���
//			buf		 ��ʼ��������ݴ��buf
//���ڲ���:	len		 ��ʼ��������ݳ���
********************************************************************************************************/
u32	gd_data_init(uc8 *dev_id, u8 data_type, u8 *data, u16 data_len, u8 *buf)
{

	u16	checksum;				
	
	sprintf(buf,"%s%c%-8.8s%c%c%c%c",GD_FRAME_HEAD_FLG,data_type,dev_id,
												(data_len>>8)&0xff,(data_len)&0xff);
	
	checksum = check_sum_16(buf, 18);
  	sprintf(buf+18,"%c%c", (checksum>>8)&0xff, checksum&0xff);
	memcpy(buf+20,data,data_len);
				
	return (20+data_len);

}
/*******************************************************************************************************
//WS-120MЭ�飬��ʼ������֡
//�ְ�
//��ڲ���: 	dev_id	 �豸ID
//			data_type�������ͣ�0���� 1��ҵ����,�̶�Ϊ1
//			packet_sum  ������
//			packet_index 	�����
//			packet_data	 ������
//			packet_len	�����ݳ���
//			frame_index  ֡���
//			buf		 ��ʼ��������ݴ��buf
//���ڲ���:	len		 ��ʼ��������ݳ���
********************************************************************************************************/
/*
u32	gd_gm_frame_data_init(uc8 *dev_id, 	u8 packet_sum, u8 packet_index, 
								u8 *packet_data, u16 packet_len, u16 frame_index,	u8 *buf)
{

	u16	checksum;
	
	sprintf(buf,"%s%c%-8.8s%c%c%c%c", GD_FRAME_HEAD_FLG, WS_120M_FRAME_DATA, dev_id, 
												((packet_len+4)>>8)&0xff, (packet_len+4)&0xff);
	
	checksum = check_sum_16(buf, 18);
	
  	sprintf(buf+18,"%c%c%c%c%c%c", (checksum>>8)&0xff, checksum&0xff, packet_index, packet_sum, 
														(frame_index>>8)&0xff, frame_index&0xff);
	
	memcpy(buf+24,packet_data, packet_len);
				
	return (24+packet_len);

}
*/
void	gd_frame_data_init(gd_frame_t *frame)
{
	u16	checksum;
	
	sprintf(frame->data,"%s%c%-8.8s%c%c%c%c", GD_FRAME_HEAD_FLG, WS_120M_FRAME_DATA, frame->dev_id, 
												((frame->packet_len+4)>>8)&0xff, (frame->packet_len+4)&0xff);
	
	checksum = check_sum_16(frame->data, 18);
	
  	sprintf(frame->data+18,"%c%c%c%c%c%c", (checksum>>8)&0xff, checksum&0xff, frame->packet_index, frame->packet_sum, 
														(frame->frame_index>>8)&0xff, frame->frame_index&0xff);
	
	memcpy(frame->data+24, frame->packet_data, frame->packet_len);
				
	frame->len = (frame->packet_len+24);

}


/*******************************************************************************************************
//WS-120MЭ�飬����֡����
//�ϰ�
//��ڲ���:
//���ڲ���:��ȷ����0
********************************************************************************************************/
u8	gd_frame_data_resolve(gd_frame_t *frame)
{

	u32	num = 0;

	if(frame->data[7] != 1)	return 1;//data type
	num = (frame->data[18] <<8) | frame->data[19];
	if(num != check_sum_16(frame->data, 18))	return 2;//checksum error
	num =(frame->data[16] <<8) | frame->data[17];
	frame->packet_len = num -4;
	if(frame->packet_len != frame->len -24) return 3;
	frame->packet_index = frame->data[20];
	frame->packet_sum= frame->data[21];
	num =(frame->data[22] <<8) | frame->data[23];
	frame->frame_index = num;
	memcpy(frame->dev_id, frame->data+8, 8);
	memcpy(frame->packet_data, frame->data+24, frame->packet_len);
				
	return 0;

}




