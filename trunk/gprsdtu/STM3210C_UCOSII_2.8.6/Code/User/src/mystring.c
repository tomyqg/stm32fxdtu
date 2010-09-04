
#include "mystring.h"
#include <string.h>




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
s32 ascii_to_hex(u8 *O_data, u8 *N_data, u16 len)
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