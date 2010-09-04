#ifndef	__MYSTRING_H__
#define	__MYSTRING_H__

/* 类型定义 */
#include <stdint.h>
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef volatile int32_t  vs32;
typedef volatile int16_t  vs16;
typedef volatile int8_t   vs8;

typedef volatile const int32_t vsc32;  /*!< Read Only */
typedef volatile const int16_t vsc16;  /*!< Read Only */
typedef volatile const int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8;

typedef volatile const uint32_t vuc32;  /*!< Read Only */
typedef volatile const uint16_t vuc16;  /*!< Read Only */
typedef volatile const uint8_t vuc8;   /*!< Read Only */



/*字符串处理*/
u32 char_to_int(u8 *cdata, u16 len);
u8* check_string(u8 *str, u8 *flg, u16 len);
s32 ascii_to_hex(u8 *O_data, u8 *N_data, u16 len);
u16 hex_to_ascii(u8 *data, u8 *buffer, u16 len);




#endif


