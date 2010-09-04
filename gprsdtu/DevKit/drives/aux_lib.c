/******************************* (C) Embest ***********************************
* File Name          : auxlib.c
* Author             : tary
* Date               : 2009-05-27
* Version            : 0.4u
* Description        : common used functions
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "aux_lib.h"

// 分解字符串为多个子串
int make_argv(char* s, int size, char *argv[], int av_max) {
	char* se;
	int n;

	n = 0;
	se = s + size - 1;
	while (s < se && *s && isspace(*s)) {
		s++;
	}

	while (s < se && *s && n < av_max - 1) {
		if (*s) {
			argv[n++] = s;
		}

		while (s < se && *s && ! isspace(*s)) {
			s++;
		}

		if (s < se && *s) {
			*s++ = '\0';
		}

		while (s < se && *s && isspace(*s)) {
			s++;
		}
	}

	argv[n] = NULL;

	return n;
}

// 转换字符串为无符号整数
u32 strtou32(const char *nptr, char **endp) {	
	u32 result = 0, value, base = 10;

        /* skip whitespace */
	while (isspace((int)(unsigned char)*nptr)) {
        	++nptr;
        }
	if ('0' == *nptr && 'X' == toupper(nptr[1])) {
		base = 16;
	} else if ('0' == *nptr) {
		base = 8;
	}
	if ((8 == base || 16 == base) && '0' == *nptr) {
		++nptr;
	}
	if (16 == base && (toupper(*nptr) == 'X')) {
		++nptr;
	}
	while (isxdigit(*nptr)) {
		value = (isdigit(*nptr)? (*nptr-'0'): (toupper(*nptr)-'A'+10));
		if (value >= base) break;
		result = result * base + value;
		++nptr;
	}
	if (endp) {
		*endp = (char *)nptr;
	}
	return result;
}

//交换字节顺序（双字节）  
u16 bswap16(u16 x) {
	return (x << 8) | (x >> 8);
}


//交换字节顺序（四字节） 
u32 bswap32(u32 x) {
	return (x << 24) | ((x << 8) & 0x00ff0000)
		| ((x >> 8) & 0x0000ff00) | (x >> 24);
}

// 字节交换
char* bswap(char* dst, char* src) {
	char ch;

	ch = *dst;
	*dst = *src;
	*src = ch;
	return dst;
}

// src指向的长度为len的字节数组逆序存放到dst指向的空间
void* breverse(void* dst, void* src, u32 len) {
	char *sd, *ss;
	u32 i, j = len - 1;
	
	sd = (char*)dst;
	ss = (char*)src;
	if (sd == ss) {
		for (i = 0; i < j; i++, j--) {
			bswap(&ss[i], &sd[j]);
		}
		return dst;
	}
	for (i = 0; i < len; i++,j--) {
		sd[i] = ss[j];
	}
	return dst;
}

// src与dst指向的长度为len的空间做交换
void* xswap(void* dst, void* src, int len) {
	char *sd, *ss;
	int i;

	sd = (char*)dst;
	ss = (char*)src;
	if (sd == ss) {
		return dst;
	}
	for (i = 0; i < len; i++) {
		bswap(ss + i, sd + i);
	}
	return dst;
}


//将buf开始的size个字节转换为长整数(4字节)
unsigned get_byte_uint(char* buf, int size, int bigend) {
	int i;
	unsigned r = 0;
	unsigned char* ubuf = (unsigned char*)buf;

	if (size > sizeof(unsigned)) size = sizeof(unsigned);

	if (! bigend) {
		memcpy(&r, &buf[0], size);
		return r;
	}
		
	for(i = 0; i < size; i++) {
		r = (r << 8) + ubuf[i];
	}
	return r;
}

//写长整数(4字节)到buf开始的size个字节
int set_byte_uint(char* buf, int size, unsigned value, int bigend) {
	int i;
	unsigned r;

	r = value;
	if (size > sizeof(unsigned)) size = sizeof(unsigned);

	if (! bigend) {
		memcpy(&buf[0], &r, size);
		return r;
	}

	for (i = size - 1; i >= 0; i--) {
		buf[i] = value & 0xFF;
		value >>= 8;
	}
	return r;
}

//(有符号)整数转字符数组
int int2arr(int num, u8 *buf, char len) {
	u8 *p = (u8 *)&num;
	int i;

	len = (len > 4) ?4 :len;
	for (i = 0; i < len; i++) {
		buf[i] = p[i];
	}
	return num;
}

//字符数组转(有符号)整数
int arr2int(u8 *buf, char len) {
	int i, num = 0;
	u8 * p = (u8 *)&num;
	
	len = (len > 4) ?4 :len;
	if (buf[len - 1] & (1 << 7)) {
		num = -1;
	}

	for (i = 0; i < len; i++) {
		p[i] = buf[i];
	}
	return num;
}

/*int int2org(int val) {
	int u;

	u = val & 0x7FFFFF;
	if (val & 800000) {
		u = 0 - u;
	}
	return u;
}
*/

//原补码互相转换, size为该编码字节数[1-4]
int org2int(int org, int size) {
	int val;

	val = (org & ((1 << (size * 8 - 1)) - 1));
	if (org & (1 << (size * 8 - 1))) {
		val = 0 - val;
	}
	return val;
}

//求8位数据校验和
// buf: 将要校验数据的缓冲区
// len: 缓冲区长度,单位字节(8bit)
unsigned short checksum_8bit(unsigned char* buf, int len) {
	unsigned i, sum = 0;

	for (i = 0; i < len; i++) {
		sum += *(buf + i);
	}
	return (unsigned short)(sum);
}

#if CRC2XOR

u16 crc_ccitt(u16 crc, const u8* buffer, unsigned len) {
	while (len--) {
		if (!(len & 0x01)) {
			crc ^= *buffer++;
			continue;
		}
		crc ^= ((u16)*buffer++ << 8);
	}
	return crc;
}

#else

u16 const crc_ccitt_table[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

static inline u16 crc_ccitt_byte(u16 crc, const u8 c)
{
	return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}

/**
 *	crc_ccitt - recompute the CRC for the data buffer
 *	@crc - previous CRC value
 *	@buffer - data pointer
 *	@len - number of bytes in the buffer
 */
u16 crc_ccitt(u16 crc, u8 const *buffer, unsigned len)
{
	while (len--) {
		crc = crc_ccitt_byte(crc, *buffer++);
	}
	return crc;
}

#endif

int sleep_us(int us) {
	volatile int delay;

	delay = us * LOOP_PER_USECS;
	// delay -= 2;
	while (delay-- != 0) {
		;
	}
	return 0;
}

//	sys_timeout(0);
//	i = 0; 
//	for (i = 0;!sys_timeout(3000);i++) {
//		sleep_us(100000);
//	}
//	printf("\r\n100ms * %d", i);
//	DISABLE_IRQ();
//	sleep_us(100000);
//  	ENABLE_IRQ();


int debug_buf(const char* head, char* buf, int len) {
	int i;

	printf("\r\nDBG:%s[%d] = \r\n\t", head, len);
	for (i = 0; i < len; i++) {
		printf("%02X ", buf[i]);
	}
	return len;
}

int debug_line(const char* file, int lin, int nr, ...) {
	int val = 0;
	const char* s;
	va_list ap;

	printf("\r\nDBG:%s() L%d\t", file, lin);
	
	va_start(ap, nr);
	while (nr-- > 0) {
		s = va_arg(ap, char*);
		if (nr-- <= 0) {
			printf(s);
			break;
		}
		val = va_arg(ap, int);
		if (strchr(s, '%') == NULL) {
			printf("%s=%d", s, val);
		} else {
			printf(s, val);
		}
	}
 	va_end(ap);

	return nr;
}

//产生随机数, 大小(0 ~ max-1)
int wid_rand(int max) {
	return abs((rand() * rand()) % max);
}

/************************************END OF FILE******************************/
