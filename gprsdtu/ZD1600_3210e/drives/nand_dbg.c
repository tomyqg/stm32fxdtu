/******************************* (C) Embest ***********************************
* File Name          : nand_dbg.c
* Author             : tary
* Date               : 2009-05-20
* Version            : 0.1
* Description        : 
******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "fsmc_nand.h"
#include "aux_lib.h"
#include "nand_dbg.h"

#define SPARE_DBG			0
#define BLK_TST_CNT			NAND_ZONE_SIZE	//NAND_ZONE_SIZE
#define F_ERASE_BLK			0
#define F_PAGE_WR			0
#define F_PAGE_RD			1
#define F_CMP_RW			0

/* Variables ----------------------------------------------------------------*/
#if (defined(SPARE_DBG) && (SPARE_DBG))
#define BUFFER_SIZE         		16
#else
#define BUFFER_SIZE         		0x200
#endif

#define NAND_ST_MakerID     		0x20
#define NAND_ST_DeviceID    		0x73

u8 TxBuffer[BUFFER_SIZE], RxBuffer[BUFFER_SIZE];

/* Private functions ---------------------------------------------------------*/
void Fill_Buffer(u8 * pBuffer, u16 BufferLenght, u32 Offset);

/*******************************************************************************
* Function Name  : nand_dbg
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
int nand_dbg(void)
{
	u32 PageNumber = 1;
	u32 WriteReadStatus = 0;
	u32 status = 0;
	NAND_IDTypeDef nand_id;
	GPIO_InitTypeDef GPIO_InitStructure;
	NAND_ADDRESS WriteReadAddr;
	int i, j;

	/* PF.06, PF.07 and PF.08  config to drive LD1, LD2 and LD3 **************** */
	/* Enable GPIOF clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

	/* Configure PF.06, PF.07 and PF.08 as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* Enable the FSMC Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	/* FSMC Initialization */
	FSMC_NAND_Init();

	/* NAND read ID command */
	FSMC_NAND_ReadID(&nand_id);
	printf("\r\nMaker_ID:  0x%X", nand_id.Maker_ID);
	printf("\r\nDevice_ID: 0x%X", nand_id.Device_ID);
	printf("\r\nThird_ID:  0x%X", nand_id.Third_ID);
	printf("\r\nFourth_ID: 0x%X", nand_id.Fourth_ID);


	/* Verify the NAND ID */
	if (0
	|| (nand_id.Maker_ID != NAND_ST_MakerID)
	|| (nand_id.Device_ID != NAND_ST_DeviceID)
	) {
		/* Turn on LD 3 */
		GPIO_SetBits(GPIOF, GPIO_Pin_9);
		return -1;
	}

	WriteReadStatus = 0;

	for (i = 0; i < BLK_TST_CNT; i++) {

	/* NAND memory address to write to */
	WriteReadAddr.Zone = 0x00;
	WriteReadAddr.Block = i;
	WriteReadAddr.Page = 0;



#if F_ERASE_BLK
	/* Erase the NAND first Block */
	status = FSMC_NAND_EraseBlock(WriteReadAddr);
	if (status != NAND_READY) {
		printf("\r\nEraseBlock %d Result 0x%X", i, status);
	}
#endif



	for (j = 0; j < NAND_BLOCK_SIZE; j++) {
		WriteReadAddr.Page = j;



#if F_PAGE_WR
		/* Write data to FSMC NAND memory */
		/* Fill the buffer to send */
		Fill_Buffer(TxBuffer, BUFFER_SIZE, j);

		//printf("Write Nandflash ...");
#if (defined(SPARE_DBG) && (SPARE_DBG))
		status = FSMC_NAND_WriteSpareArea(TxBuffer, WriteReadAddr, PageNumber);
#else
		status = FSMC_NAND_WriteSmallPage(TxBuffer, WriteReadAddr, PageNumber);
#endif
		//printf(" 0x%X\r\n", status);
#endif //F_PAGE_WR



#if F_PAGE_RD
		//printf("Read Nandflash ...");
		/* Read back the written data */
#if (defined(SPARE_DBG) && (SPARE_DBG))
		status = FSMC_NAND_ReadSpareArea(RxBuffer, WriteReadAddr, PageNumber);
#else
		status = FSMC_NAND_ReadSmallPage(RxBuffer, WriteReadAddr, PageNumber);
#endif
		//printf(" 0x%X\r\n", status);
		if (memcmp(RxBuffer, "\xEB\x3C\x90", 3) == 0) {
			debug_buf("RxBuffer", (char*)RxBuffer, BUFFER_SIZE);
			printf("\r\nblk = %d page = %d", i, j);
		}
#endif //F_PAGE_RD



#if F_CMP_RW
		/* Verify the written data */
		if (memcmp(TxBuffer, RxBuffer, BUFFER_SIZE) != 0) {
			WriteReadStatus++;
		}

		// debug_buf("TxBuffer", TxBuffer, BUFFER_SIZE);
		if (WriteReadStatus == 0) {	/* OK */
			/* Turn on LD1 */
			GPIO_SetBits(GPIOF, GPIO_Pin_8);
			printf("\r\nBlock = %.4d Page = %.2d Write OK", i, j);
		} else {	/* KO */
			/* Turn on LD2 */
			GPIO_SetBits(GPIOF, GPIO_Pin_9);
			printf("\r\nBlock = %.4d Page = %.2d Write Err", i, j);
			debug_buf("RxBuffer", RxBuffer, BUFFER_SIZE);
			break;
		}
#endif //F_CMP_RW



		}

	}

	printf("\r\nAll Operations Complete");
	//while (1) {
	//}
	return 0;
}

/*******************************************************************************
* Function name : Fill_Buffer
* Description   : Fill the buffer
* Input         : - pBuffer: pointer on the Buffer to fill
*                 - BufferSize: size of the buffer to fill
*                 - Offset: first value to fill on the Buffer
* Output param  : None
*******************************************************************************/
void Fill_Buffer(u8 *pBuffer, u16 BufferLenght, u32 Offset)
{
	u16 IndexTmp = 0;

	/* Put in global buffer same values */
	for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ ) {
		pBuffer[IndexTmp] = IndexTmp + Offset;
	}
	return;
}

/************************************END OF FILE******************************/
