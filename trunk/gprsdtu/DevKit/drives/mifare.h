/******************************* (C) Embest ***********************************
* File Name          : mifare.h
* Author             : tary
* Date               : 2009-02-26
* Version            : 0.3
* Description        : MIFARE control functions 
******************************************************************************/

#ifndef	__MIFARE_H__
#define	__MIFARE_H__

#include "stm32f10x_lib.h"
#include "aux_lib.h"
#include "fm1702.h"
#include <stdio.h>

//REQ操作是否等待回复
#define MIFARE_ID_WAIT		0	// 获取ID时是否等待回复
#define MIFARE_DETECT_WAIT	0	// 检测卡片是否等待回复
#define MIFARE_DBG		0	// 是否单步调试MIFARE

// 等待卡片状态机的变迁过程
#define FM1702_HALT2REQ_T	(1000)	// us
#define FM1702_DETECT_TOUT	(10)	// ms
#define FM1702_ID_TOUT		(10)	// ms
#define FM1702_RD2RD_T		(1)	// us
#define FM1702_WR2WR_T		(1)	// us

int mifare_detect(int wait_req);
int mifare_id(char* uid);
int mifare_dbg(void);
int mifare_active(int mif_blk);
int mifare_write(int mif_blk, char* buf, char* key);
int mifare_read(int mif_blk, char* buf, char* key);
int mifare_key(int mif_sec, char* key);
int mifare_key2(void);
int get_keybuf_e2p(void);
int set_keybuf_e2p(void);
int get_fm1702_info(char* buf, int len);

#endif	//__MIFARE_H__

/************************************END OF FILE******************************/
