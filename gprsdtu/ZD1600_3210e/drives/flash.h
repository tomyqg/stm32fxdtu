#ifndef __FLASH_H__
#define __FLASH_H__
#include "stm32f10x_type.h"


typedef struct __FLASH_STOAGE_TYPE
{
	void *data;//数据
	u32	len;//数据长度
}flash_storage_t;


s8	store_to_flash(flash_storage_t *storage);
s8	load_from_flash(flash_storage_t *storage);







#endif
