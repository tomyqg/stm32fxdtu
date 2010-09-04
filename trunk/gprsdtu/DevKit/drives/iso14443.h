/******************************* (C) Embest ***********************************
* File Name          : iso14443.h
* Author             : tary 
* Date               : 2009-02-26
* Version            : 0.1
* Description        : iso14443.h
******************************************************************************/

#ifndef	__ISO14443_H__
#define	__ISO14443_H__

#define CMD_REQA	0x26	// 7bit
#define CMD_WAKEUPA	0x52	// 7bit
#define CMD_ANTICOL	0x93
#define CMD_SELECT	0x93	// (1001xxxx)b
#define CMD_AUTHA	0x60
#define CMD_READ	0x30
#define CMD_WRITE	0xA0
#define CMD_HALTA	0x50	// 8bit

#endif	//__ISO14443_H__

/************************************END OF FILE******************************/
