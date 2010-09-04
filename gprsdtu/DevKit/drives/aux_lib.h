/******************************* (C) Embest ***********************************
* File Name          : auxlib.h
* Author             : tary
* Date               : 2009-05-27
* Version            : 0.4u
* Description        : common used functions
******************************************************************************/

#ifndef __AUX_LIB_H__
#define __AUX_LIB_H__

#include "stm32f10x_lib.h"
#include "systick.h"


#define member_offset(s, m)	((int)(&(((s*)0)->m)))
#define countof(a)		(sizeof(a) / sizeof(a[0]))

#define BCD2BIN(val)		(((val) & 0x0F) + ((val) >> 4) * 10)
#define BIN2BCD(val)		((((val) / 10) << 4) + (val) % 10)

#define DISABLE_IRQ()		__SETPRIMASK()
#define ENABLE_IRQ()		__RESETPRIMASK()

#if defined(LBP_DBG)
#define DBG_LINE(...)		do {					\
	if (LOCAL_DBG) debug_line(__FUNCTION__, __LINE__, __VA_ARGS__);	\
					}while(0)
#define DBG_BUF(...)		do {					\
	if (LOCAL_DBG) debug_buf(__VA_ARGS__);	\
					}while(0)
#define DBG_PRINT(...)		do {					\
	if (LOCAL_DBG){							\
		printf("\r\nDBG:%s() L%d\t", __FUNCTION__, __LINE__);	\
		printf(__VA_ARGS__);					\
	}				}while(0)
#else
#define DBG_LINE(...)
#define DBG_BUF(...)
#define DBG_PRINT(...)
#endif	//LBP_DBG

#if defined(NO_SERIAL) 
#define printf(...)
#define scanf(...)
#endif

#define CRC2XOR			1		//CRC算法改为异或算法
#define LOOP_PER_USECS		(12)		// under 72 Mhz

typedef int (*func_none_arg_t)(void);
typedef void* (*func_one_arg_t)(void*);

int getch(void);
int peekch(void);
u16 crc_ccitt(u16 crc, const u8 *buffer, unsigned len);
u32 strtou32(const char *nptr, char **endp);
u16 bswap16(u16 x);
u32 bswap32(u32 x);
char* bswap(char* dst, char* src);
void* breverse(void* dst, void* src, u32 len);
void* xswap(void* dst, void* src, int len);
unsigned get_byte_uint(char* buf, int size, int bigend);
int set_byte_uint(char* buf, int size, unsigned value, int bigend);
int int2arr(int num, u8 *buf, char len);
int arr2int(u8 *buf, char len);
int org2int(int org, int size);
unsigned short checksum_8bit(unsigned char* buf, int len);
int sleep_us(int us);
int debug_buf(const char* head, char* buf, int len);
int debug_line(const char* file, int lin, int nr, ...);
int make_argv(char* s, int size, char *argv[], int av_max);
int wid_rand(int max);

#endif	//__AUX_LIB_H__

/************************************END OF FILE******************************/
