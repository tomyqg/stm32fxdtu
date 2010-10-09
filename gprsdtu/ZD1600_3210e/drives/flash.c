
#include "flash.h"
#include "board_inf.h"
#include "stm32f10x_lib.h"



/**********************************************************************
存储数据到flash数据备份区域
入口参数: storage
出口参数: 正确返回0
**********************************************************************/
s8 store_to_flash(flash_storage_t * storage)
{
	volatile FLASH_Status flash_status = FLASH_COMPLETE;
	vu8	page_counter = 0;//stm32系列最大256页
	u32 i, *pdata, addr;

	if((storage->len > FLASH_STORAGE_SIZE) || (storage->len < 1))	return 1;
	page_counter = storage->len /FLASH_PAGE_SIZE;
	if((storage->len %FLASH_PAGE_SIZE) > 0)	page_counter++;

	
	
	  /* Unlock the Flash Program Erase controller */
  	FLASH_Unlock();
	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	/* Erase the FLASH pages */
	for(i = 0; i < page_counter; i++)
	{
		flash_status = FLASH_ErasePage(FLASH_STORAGE_HEAD + (FLASH_PAGE_SIZE * i));
		if(flash_status != FLASH_COMPLETE)	return 2;
	}
	/*  FLASH Word program of data 0x15041979 at addresses defined by StartAddr and EndAddr*/
	addr = FLASH_STORAGE_HEAD;
	pdata = (u32 *)storage->data;
	for(i=0; i < storage->len; i += 4)
	{
		flash_status = FLASH_ProgramWord(addr, *pdata++);
		if(flash_status != FLASH_COMPLETE)	return 3;
		addr += 4;
	}
	/* Check the corectness of written data */
	addr = FLASH_STORAGE_HEAD;
	pdata = (u32 *)storage->data;
	for(i=0; i < storage->len; i += 4)
	{
		if((*(vu32*) addr) != *pdata++)
		{
			return 4;
		}
		addr += 4;
	}
	
	return 0;  

}

/**********************************************************************
从flash数据备份区域加载数据
入口参数: storage
出口参数: 正确返回0
**********************************************************************/
s8 load_from_flash(flash_storage_t * storage)
{
	volatile FLASH_Status flash_status = FLASH_COMPLETE;
	vu8	page_counter = 0;//stm32系列最大256页
	u32 i, *pdata, addr;

	if((storage->len > FLASH_STORAGE_SIZE) || (storage->len < 1))	return 1;
	addr = FLASH_STORAGE_HEAD;
	pdata = (u32 *)storage->data;
	for(i=0; i < storage->len; i += 4)
	{
		*pdata++ =  *(vu32*) addr;
		addr += 4;
	}
	return 0;
}






