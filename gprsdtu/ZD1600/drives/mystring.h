#ifndef	__MYSTRING_H__
#define	__MYSTRING_H__
#include "stm32f10x_type.h"

/*�ַ�������*/
u32 char_to_int(u8 *cdata, u32 len);
u8* check_string(u8 *str, u8 *flg, u32 len);
s32 ascii_to_hex(u8 *O_data, u8 *N_data, u32 len);
u32 hex_to_ascii(u8 *data, u8 *buffer, u32 len);




#endif


