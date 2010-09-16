#ifndef	__MYSTRING_H__
#define	__MYSTRING_H__
#include "stm32f10x_type.h"

/*×Ö·û´®´¦Àí*/
u32 char_to_int(u8 *cdata, u32 len);
u8* check_string(u8 *str, u8 *flg, u32 len);
u32 ascii_to_hex(u8 *O_data, u8 *N_data, u32 len);
u32 hex_to_ascii(u8 *data, u8 *buffer, u32 len);

u16 check_sum_16(u8 *addr, u16 count);
u32	gd_heart_beat_init(uc8 *dev_id, uc8 *dev_mac, u8 data_type, u8 *extra_data, u8 extra_len, u8 wifi_signal, u8 *buf);
u32	gd_frame_data_init(uc8 *dev_id, u8 data_type, u8 *data, u32 data_len, u8 *buf);

#endif


