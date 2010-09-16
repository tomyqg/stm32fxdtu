/******************************* (C) Embest ***********************************
* File Name          : mifare.c
* Author             : tary
* Date               : 2009-02-26
* Version            : 0.3
* Description        : MIFARE control functions 
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "mifare.h"

#define LOCAL_DBG		(MIFARE_DBG)
#define F_MIFARE_TST		(1)

static char card_uid[8];	//保存的卡物理ID
static char key_buf[16];	//用于读写操作的key缓冲,12字节格式
static char key_old[16];	//用于mifare_key2()快速恢复key值


static int mifare_req(void) {
	int r;

	//检测卡时将卡置为IDLE(HALT)状态
	if ((r = fm1702_mif_halt()) != FM1702_OK) {
		//DBG_LINE(2, " halt", r);
		return r;
	}

#if MIFARE_DETECT_WAIT
	if (1) {
		int us_wait = 0;
		unsigned long tmr;

		sys_timeout(&tmr, 0);
		while (! sys_timeout(&tmr, FM1702_DETECT_TOUT)) {
			if ((r = fm1702_mif_req(1)) == FM1702_OK) {
				break;
			}
			sleep_us(us_wait++);
		}

		if (r != FM1702_OK) {
			//DBG_LINE(2, " req", r);
			return r;
		}
	} else
#endif
	{
		sleep_us(FM1702_HALT2REQ_T);

		if ((r = fm1702_mif_req(1)) != FM1702_OK) {
			//DBG_LINE(2, " req", r);
			return r;
		}
	}
	return r;
}

//检则天线范围内是否存在MIFARE卡
//wait_req	为0时为检测卡片,其它值表示读写数据等
int mifare_detect(int wait_req) {
	int r;

	if ((r = mifare_req()) == FM1702_OK) {
		return r;
	}
	// 10us用于关闭电磁波的发射, 不能太短(如,只有1us)
	// 2009-01-13
	fm1702_mif_no_energy(10);
	return mifare_req();
}

// 获取卡片UID
int mifare_id(char* uid) {
	int r;

	if ((r = mifare_detect(1)) != FM1702_OK) {
		DBG_LINE(2, " detect", r);
		return r;
	}

	if ((r = fm1702_mif_anticol(card_uid)) != FM1702_OK) {
		DBG_LINE(2, " anticol", r);
		memset(card_uid, '\0', 5);
		return r;
	}

	if ((r = fm1702_mif_sel(card_uid)) != FM1702_OK) {
		DBG_LINE(2, " sel", r);
		memset(card_uid, '\0', 5);
		return r;
	}

	if (uid != NULL) {
		memcpy(uid, card_uid, 5);
	}
	return r;
}

// 激活MIFARE卡命令,应用程序接口
int mifare_active(int mif_blk) {
	char uid[8];
	int r;

	if ((r = mifare_id(uid)) != FM1702_OK) {
		DBG_LINE(2, " id", r);
		return r;
	}

	//fm1702_mif_key_e2p(mif_blk >> 2);
	if ((r = fm1702_mif_key(mif_blk >> 2, key_buf)) != FM1702_OK) {
		DBG_LINE(2, " key", r);
		return r;
	}

	if ((r = fm1702_mif_auth(card_uid, mif_blk)) != FM1702_OK) {
		DBG_BUF("mifare_active(): key_buf", key_buf, 12);
		DBG_LINE(2, " auth", r);
		return r;
	}
		DBG_LINE(2, " ok", r);
	return FM1702_OK;
}

//MIF读块命令,应用程序接口
//key		不为NULL时, 则先加载key值(6字节格式)
//return	>= 0	读取的字节数
//		<0	错误码
int mifare_read(int mif_blk, char* buf, char* key) {
	int rlen;
	//static 
	int l_sec = -1;

	if (key != NULL) {
		mifare_key(mif_blk >> 2, key);
	}

	if (l_sec != (mif_blk >> 2)) {
		l_sec = (mif_blk >> 2);
		rlen = mifare_active(mif_blk);
		if (rlen < 0) {
			DBG_LINE(2, " active err", rlen);
			return rlen;
		}
	}
	//DBG_LINE(1, " active ok");
	return fm1702_mif_read(mif_blk, buf);
}

//MIF写块命令,应用程序接口
//key		不为NULL时, 则先加载key值(6字节格式)
//return	FM1702_OK	成功
//		<0		错误码
int mifare_write(int mif_blk, char* buf, char* key) {
	int rlen;
	//static 
	int l_sec = -1;

	if (key != NULL) {
		mifare_key(mif_blk >> 2, key);
	}
	
	if (l_sec != (mif_blk >> 2)) {
		l_sec = (mif_blk >> 2);
		rlen = mifare_active(mif_blk);
		if (rlen != FM1702_OK) {
			DBG_LINE(2, " active err", rlen);
			return rlen;
		}
	}
	//DBG_LINE(1, " active ok");
	return fm1702_mif_write(mif_blk, buf);
}

//由EEPROM读key到key_buf
int get_keybuf_e2p(void) {
	int rlen;
	char kbuf[0x10];
	
	rlen = fm1702_read_e2p(0x30, kbuf, 12);
	if (rlen < 0) {
		return FM1702_ERR_E2P;
	}
	memcpy(key_buf, kbuf, rlen);
	debug_buf(__FUNCTION__, kbuf, rlen);
	return FM1702_OK;
}

//写keybuf到EEPROM
int set_keybuf_e2p(void) {
	char kbuf[0x10];

	debug_buf(__FUNCTION__, key_buf, 12);
	if (FM1702_OK != fm1702_write_e2p(0x30, key_buf, 12)) {
		DBG_LINE(1, " write");
		return FM1702_ERR_E2P;
	}
	memcpy(kbuf, key_buf, 12);
	if (fm1702_read_e2p(0x30, kbuf, 12) < 0) {
		DBG_LINE(1, " read");
		return FM1702_ERR_E2P;
	}
	if (memcmp(kbuf, key_buf, 12) != 0) {
		DBG_LINE(1, " cmp");
		return FM1702_ERR_E2P;
	}
	return FM1702_OK;
}

//加载key[6]到key_buf, 用于mifare_active时载入FM1702的KEY BUFFER
int mifare_key(int mif_sec, char* key) {
	int i;
	char kbuf[0x10];
	unsigned char ch_l, ch_h;

#if 1
	for (i = 0; i < 6; i++) {
		ch_h = key[i] & 0xF0;
		ch_l = (key[i] & 0x0F);
		kbuf[2*i + 0] = (~ch_h & 0xF0) | (ch_h >> 4);
		kbuf[2*i + 1] = ch_l | (~ch_l << 4);
	}
#else
	memcpy(key_buf, key, 6);
#endif
	memcpy(key_old, key_buf, 12);
	memcpy(key_buf, kbuf, 12);
	//return set_keybuf_e2p();

	DBG_BUF("mifare_key(): key_buf", key_buf, 12);
	return FM1702_OK;
	//写key指向的KEY到EEPROM,
	//0x80开始是密钥存放区
	//并按12字节每MIFARE扇区连续存放
	//fm1702_write_e2p(mif_sec * 12 + 0x80, kbuf, 12);
}

//恢复上一次调用mifare_key的key值
int mifare_key2(void) {
	memcpy(key_buf, key_old, 12);
	return 0;
}

//FM1702 EEPROM 0地址开始16字节为厂商信息
int get_fm1702_info(char* buf, int len) {
	int rlen;

	rlen = fm1702_read_e2p(0x00, buf, len);
	if (rlen > 0)
		return rlen;
	return FM1702_ERR_E2P;
}




/////////////////////////////////////TEST CODE//////////////////////////////////

#if F_MIFARE_TST

// 测试RFID或1702内EEPROM
#define F_TEST_RFID		1
// 是否测试写操作
#define F_TEST_WR		0

char key_a[] = {
#if 1
// 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
 0x11, 0x22, 0x33, 0x44, 0x55, 0x66
#else
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
#endif
};

char key_b[] = {
#if 1
 0x11, 0x22, 0x33, 0x44, 0x55, 0x66
#else
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
#endif
};


int mifare_dbg(void) {
	extern char key_a[];
#if F_TEST_WR
	int j;
	char data[20] = 
	  {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
	  0x21, 0x43, 0x65, 0x87, 0x09, 0xBA, 0xDC, 0xFE };
#endif
	char readbuf[20];
	int rlen;
	int i;
	int mif_blk
#if F_TEST_RFID
	 = 0x01;
#else
	 = 0x30;
#endif

#if 0
		
	printf("\r\nblock #?");
	ascanf("%d", &mif_blk);
#endif


#if F_TEST_RFID
	mifare_key(mif_blk >> 2, key_a);

	if ((rlen = mifare_active(mif_blk)) == FM1702_OK) {
		printf("\r\nCard Active: OK");
	} else {
		printf("\r\nCard Active: ERR %d", rlen);
	}
#endif
	i = 0;
	while (1) {
		mif_blk++;
#if F_TEST_RFID
		if (mif_blk % 4 == 3) {
			mif_blk++;
		}
		if (mif_blk >= 64) mif_blk = 0;

		while (mifare_id(NULL) != FM1702_OK) {
			printf("\r\nNO CARD");
			sleep(500);
		}
#else
		if (mif_blk + 0x10 >= 0x80) mif_blk = 0x30;
#endif
		printf("\r\n************* blk = %d *************", mif_blk);
#if F_TEST_WR
		for (j = 0; j < 16; j++)
			data[j] = j + mif_blk + i;
		debug_buf("DATA", data, 0x10);

		// sample time 5.3ms
		if ((rlen = 
#if F_TEST_RFID
		mifare_write(mif_blk, data, key_a)
#else
		fm1702_write_e2p(mif_blk, data, 0x10)
#endif
		) == FM1702_OK) {
			printf("\r\nCard Write: OK");
		} else {
			printf("\r\nCard Write: ERR %d", rlen);
		}
#endif
		sleep(100);
		memset(readbuf, '\xFF', 20);

		// sample time 2.5ms
		if ((rlen = 
#if F_TEST_RFID
		  mifare_read(mif_blk, readbuf, key_a)

#else
		  fm1702_read_e2p(mif_blk, readbuf, 0x10)
#endif
		) >= 0) {
			//printf("\r\nCard Read: OK");
			debug_buf("DATA", readbuf, rlen);
		} else {
			printf("\r\nCard Read: ERR %d", rlen);
		}

#if F_TEST_WR
		if (memcmp(readbuf, data, 0x10) != 0) {
			printf("\r\nWrite Err");
		}
#endif
		//sleep(1000);
		if (i++ % 10 == 0) {
			printf("\r\n%d", i);
			printf(" times %d ms", sys_elapse_time());
		}
	}
	//return 0;
}

#else
int mifare_dbg(void) {
	return 0;
}

#endif

/************************************END OF FILE******************************/
