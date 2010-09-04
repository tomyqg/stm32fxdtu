/******************************* (C) Embest ***********************************
* File Name          : fm1702.c
* Author             : tary
* Date               : 2009-06-25
* Version            : 0.4u
* Description        : fm1702 device low level operations 
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "fm1702_inf.h"
#include "fm1702.h"
#include "iso14443.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if FM1702_CW_DEBUG
int CW_VAL = 0x3F;
int MOD_W_VAL = 0x13;
#endif
unsigned long stm;

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//FM1702��FIFO����
__inline int fifo_len(void);
static int clear_fifo(void);
static int write_fifo(char* buf, int len);
static int read_fifo(char* buf);

int err_chk(const char* file, int lineno);

static int trans_uid(int collision, char* uid, char* buf, int len);

/* Private functions ---------------------------------------------------------*/
//�ú���ʵ�ֶ�fm1702�ӿڵĳ�ʼ��
int fm1702_init(void) {
	char dummy = 0xFF;
	//char rsvbyte;
	
	fm1702_inf_init();

	FM1702_RST_HIGH();
	sleep(10);
	FM1702_RST_LOW();
	sleep(10);

	//ѡ��SPI�ӿڷ�ʽ,�ز�����,
	//�������ErrorFlag��ʹ״̬����
	FM1702_SET_REG(Page_Reg, 0x80);	
	//�ȴ���λ���
	fm1702_timeout(&stm, 0);
	do {
		if ((dummy = FM1702_GET_REG(Command_Reg)) == 0) {
			FM1702_SET_REG(Page_Reg, 0x00);
			break;
		}
	} while (!fm1702_timeout(&stm, 100));
	if (dummy != 0) {
		return FM1702_ERR_INIT;
	}
#if 0
	printf("\r\n");
	err_chk(__FUNCTION__, __LINE__);
#endif

	FM1702_SET_REG(TimerClock_Reg, 0x0B);		//151us/per
	FM1702_SET_REG(TimerControl_Reg, 0x02);	//���ͽ�������ʱ��,���տ�ʼ�ض�ʱ��
	FM1702_SET_REG(TimerReload_Reg, 0x42);		// 10ms��ʱ
	FM1702_SET_REG(InterruptEn_Reg, 0x7F);		//���������ж�
	FM1702_SET_REG(InterruptRq_Reg, 0x7F);		//�������жϱ�־
	//0
	//10	Modulator Source, ʹ���ڲ�������
	//1
	//1	TX2Inv, TX2��������������ز�
	//0	TX2Cw, TX2�����������13.56MHz���������ز�
	//1	TX2RFEn, TX2�����13.56MHz���������ݵ��ƺ�������ز�
	//1	TX1RFEn, TX1�����13.56MHz���������ݵ��ƺ�������ز�
	FM1702_SET_REG(TxControl_Reg, 0x5B);
	//0	RcvClkSelI, 0��ʾʹ��Q-clock(I-clock��Q-clock��90����λ��).
	//0	RxAutoPD, 0���ǽ���
	//0000	
	//01	DecoderSource, ʹ���ڲ�������
	//FM1702_SET_REG(RxControl2_Reg, 0x41);
	
	//00000111 ���ݴ����, ��7���ȴ�λʱ�Ӻ󼤻������
	FM1702_SET_REG(RxWait_Reg, 0x07);

	//����������ƿ��
	FM1702_SET_REG(ModWidth_Reg, MOD_W_VAL);

	//���ý����ź�ǿ����ֵ
	//FM1702_SET_REG(RxThreshold_Reg, 0xFF);
	return FM1702_OK;
}


int err_chk(const char* file, int lineno) {
	char dummy;

	if ((dummy = FM1702_GET_REG(ErrorFlag_Reg)) == 0)
		return FM1702_OK;
#if FM1702_DEBUG
	printf("\r\n%s():%d errs: ", file, lineno);
	if (dummy & 0x01) {
		printf("Colli ");
	}
	if (dummy & 0x02) {
		printf("Parity ");
	}
	if (dummy & 0x04) {
		printf("Frame ");
	}
	if (dummy & 0x08) {
		printf("CRC ");
	}
	if (dummy & 0x10) {
		printf("FIFOOvfl ");
	}
	if (dummy & 0x20) {
		printf("Access ");
	}
	if (dummy & 0x40) {
		printf("Key ");
	}
#endif	// FM1702_DEBUG
	return dummy + FM1702_E_BASE;
}


//FM1702 EEPROM 0x10-0x2F��Ϣ������ȷ,
//����FM1702�Ĵ����᲻�����ع���
int fm1702_reg_initial(int fix) {
	static const char reg_inits[] = {
		0x00, 0x58, 0x3F, 0x3F,
		0x19, 0x13, 0x00, 0x00,
		0x00, 0x73, 0x08, 0xAD,
		0xFF, 0x00, 0x41, 0x00,
		0x00, 0x06, 0x03, 0x63,
		0x63, 0x00, 0x00, 0x00,
		0x00, 0x08, 0x07, 0x06,
		0x0A, 0x02, 0x00, 0x00
	};
	char reg_buf[40];
	int rlen;

	rlen = fm1702_read_e2p(0x10, reg_buf, 0x10);
	if (rlen != 0x10) {
		return rlen;
	}
	rlen = fm1702_read_e2p(0x20, reg_buf + 0x10, 0x10);
	if (rlen != 0x10) {
		return rlen;
	}

	if (memcmp(reg_buf, reg_inits, 32) == 0) {
		return FM1702_OK;
	}

	if (fix == 0) {
		return FM1702_ERR_REG;
	}
	
	rlen = fm1702_write_e2p(0x10, (char*)reg_inits, 0x10);
	if (rlen != FM1702_OK) {
		return rlen;
	}
	rlen = fm1702_write_e2p(0x20, (char*)reg_inits + 0x10, 0x10);
	if (rlen != FM1702_OK) {
		return rlen;
	}
	return FM1702_OK;
}

//ȡ��FIFO����
__inline static int fifo_len(void) {
	return FM1702_GET_REG(FIFOLength_Reg);
}

//���FM1715��FIFO������
static int clear_fifo(void) {
	char dummy, i;
	
	// bit0, Flush FIFO
	FM1702_SET_REG(Control_Reg, FM1702_GET_REG(Control_Reg) | 0x01);

	for (i = 10; i != 0; i--) {
		if ((dummy = FM1702_GET_REG(FIFOLength_Reg)) == 0)
			break;
		//sleep_us(500);
	}
	if (dummy != 0) {
		err_chk(__FUNCTION__, __LINE__);
		return FM1702_ERR_FIFO;
	}
	return FM1702_OK;
}

//��FM1715��FIFO�ж�������
//���ض�ȡ�ֽ��� >= 0
static int read_fifo(char* buf) {
	int len = 0;
	
	if ((len = fifo_len()) < 0) {
		err_chk(__FUNCTION__, __LINE__);
		return FM1702_ERR_FIFO;
	} else if (len == 0) {
		return FM1702_OK;
	} else if (len >= 24) {
		len = 24;
	}

	//printf("\r\n%s(): ", __FUNCTION__);
	//printf("fifo len = %d", len);
	//sleep_us(1000);
	
	fm1702_inf_read(FIFOData_Reg, buf, len);
	return len;
}

//FM1715��FIFO��д��len�ֽ�����
static int write_fifo(char* buf, int len) {
	fm1702_inf_write(FIFOData_Reg, buf, len);
	return FM1702_OK;
}

//��FM1715�������
static int cmd_send(int cmd, char* buf, int len) {
	int rlen;
	char dummy;

	FM1702_SET_REG(Command_Reg, 0x00);

	if (clear_fifo() != FM1702_OK) {
		err_chk(__FUNCTION__, __LINE__);
		return FM1702_ERR_FIFO;
	}

	if (write_fifo(buf, len) != FM1702_OK) {
		err_chk(__FUNCTION__, __LINE__);
		return FM1702_ERR_FIFO;
	}

	FM1702_SET_REG(Command_Reg, cmd);

	fm1702_timeout(&stm, 0);
	while (!fm1702_timeout(&stm, 20)) {
		if ((dummy = FM1702_GET_REG(Command_Reg)) == 0x00) {
			//printf("%02X", dummy);
			return FM1702_OK;
		}		
		dummy = FM1702_GET_REG(InterruptRq_Reg);
		if ((dummy & 0x80) == 0x80) {
			//printf("%02X", dummy);
			return FM1702_OK;
		}
	}
#if 1
	if ((rlen = err_chk(__FUNCTION__, __LINE__)) != 0) {
		return rlen;
	} 
	return FM1702_ERR_CMD;
#endif
	//return FM1702_OK;
}

//��FM1715��EEPROM�ж�������
//addr	��ȡ��ַ
//len	���������ݵ��ֽ���
//buf	ָ����������ݵ�ָ��
int fm1702_read_e2p(int addr, char* buf, int len) {
	char bsnd[3];
	
	// �ɶ�ȡ��Ϊ0x0 - 0x7F
	bsnd[0] = (addr & 0x7F);
	bsnd[1] = 0;
	bsnd[2] = (char)len;
	//debug_buf("R ADDR", bsnd, 3);
	if (cmd_send(ReadE2, bsnd, 3) != FM1702_OK) {
		return err_chk(__FUNCTION__, __LINE__);
	}
	return read_fifo(buf);
}

//��FM1715��EEPROM��д������
//addr	д���ַ
//len	��д�����ݵ��ֽ���
//buf	ָ���д�����ݵ�ָ��
int fm1702_write_e2p(int addr, char* buf, int len) {
	char bsnd[2 + 18];
	char dummy;
	//int rlen;

	if (addr < 0x10) {
		return FM1702_ERR_E2P; 
	}
	bsnd[0] = (addr & 0xFF);
	bsnd[1] = (addr & 0x0100) >> 8;
	len = (len > 16)? 16: len;
	memcpy(bsnd + 2, buf, len);

	//debug_buf("W ADDR", bsnd, len + 2);
	if ((//rlen = 
	cmd_send(WriteE2, bsnd, len + 2)) != FM1702_OK) {
		//return rlen;
	}
	fm1702_timeout(&stm, 0);
	do {
		dummy = FM1702_GET_REG(SecondaryStatus_Reg);
		if ((dummy & 0x40) == 0x40)
			break;
		//bit6, E2Ready, 1 = EEPROM��������
	} while (!fm1702_timeout(&stm, 10));
#if 0
	//���Command_RegΪ0
	fm1702_timeout(&stm, 0);
	do {
		FM1702_SET_REG(Command_Reg, 0x00);
		if (FM1702_GET_REG(Command_Reg) == 0)
			break;
	} while (!fm1702_timeout(&stm, 10));
#endif
	if ((dummy & 0x40) == 0x40) {
	//fm1702_read_e2p(addr, bsnd, len);
	//if (memcmp(bsnd, buf, len) == 0) {
		return FM1702_OK;
	}
	//printf("\r\n%s():", __FUNCTION__);
	//printf("line = %d", __LINE__);
	return FM1702_ERR_E2P;
}

// ��Ƭ��λӦ����ж�
int detect_req(char* buf) {
	if (buf[1] == 0) {
		//�Ϻ���׼1KB��
		if (buf[0] == 0x53) {
			return 1;
		}

		//�Ϻ���׼TOKEN��
		if (buf[0] == 0x03) {
			return 1;
		}

		//MIFARE��׼TOKEN��
		if (buf[0] == 0x05) {
			return 1;
		}

		//MIFARE ULTRALIGHT
		if (buf[0] == 0x44) {
			return 1;
		}

		//MIFARE 1K
		if (buf[0] == 0x04) {
			return 1;
		}

		//MIFARE 4K
		if (buf[0] == 0x02) {
			return 1;
		}
	}
	
	if (buf[1] == 0x03) {
		//MIFARE DESFire
		if (buf[0] == 0x04) {
			return 1;
		}
	}
	
	if ((buf[1] & 0xF8) == 0) {
		//MIFARE ProX��SmartMX xD(T)
		if (
		  buf[0] == 0x08 ||
		  buf[0] == 0x04 ||
		  buf[0] == 0x02 ||
		  buf[0] == 0x48 ||
		  buf[0] == 0x44 ||
		  buf[0] == 0x42
		) {
			return 1;
		}
	}
	return 0;
}

// �жϿ�Ƭ�յ������к�
int check_uid(char* buf) {
	char chk = 0;
	int i;

	for (i = 0; i < 5; i++) {
		chk ^= buf[i];
	}
	return chk == 0x00;
}

// ���濨Ƭ�յ������к�
static int trans_uid(int collision, char* uid, char* buf, int len) {
	char row, col, chbuf, chid;

	//debug_buf(__FUNCTION__, buf, len);
	if ((collision & 0xFFFF) == 0) {
		memcpy(uid, buf, len);
		return 0;
	}

	row = (collision & 0xFF00) >> 8;
	col = collision;
	chbuf = buf[0];
	chid = uid[row - 1];
	if (col == 0x00) {
		chid = 0x00;
		row++;
	} else if (col <= 7){
		chbuf &= (0xFF << col);
		chid &= ~(0xFF << col);
	}
	buf[0] = chbuf;
	uid[row - 1] = chbuf | chid;
	memcpy(&uid[row], &buf[1], len - 1);
	return 1;
}

//���ô��������ݵ��ֽ���
int set_bitframe(int collision, char* buf) {
	int bf = 0;
	char row, col;

	row = (collision & 0xFF00) >> 8;
	col = collision;
	if (row <= 4) {
		buf[1] = (2 + col) * 0x10;
	}

	if (col != 0x00 && col <= 7) {
		bf = col * 0x11;
		buf[1] |= col;
	}
	FM1702_SET_REG(BitFraming_Reg, bf);
	return bf;	
}

// ����key��FM1702��KEY BUFFER��
int fm1702_mif_key(int mif_sec, char* key) {
	if (cmd_send(LoadKey, key, 12) != FM1702_OK) {
		return err_chk(__FUNCTION__, __LINE__);
	}
	return FM1702_OK;
}

// ��E2PROM�������FM1702��KEY BUFFER��
int fm1702_mif_key_e2p(int mif_sec) {
	int addr;
	char buf[4];

	// 0x80��ʼ����Կ�����, ����12�ֽ��������
	addr = 0x80 + 12 * mif_sec;
	buf[0] = addr & 0xFF;
	buf[1] = (addr & 0x0100) >> 8;
	if (cmd_send(LoadKeyE2, buf, 2) != FM1702_OK) {
 		return err_chk(__FUNCTION__, __LINE__);
	}
	return FM1702_OK;
}

// �رյ�Ų��ز�����
int fm1702_mif_no_energy(int usec) {
	// ����ĳЩ��Ƭ, ֻ�е�һ���ϵ�ʱREQ�����Ż��лظ�
	// ����, ���´���ر���������, ���ڶԸ����ֿ�Ƭ.
	// ����������Ƭû��Ӱ��
	FM1702_SET_REG(TxControl_Reg, FM1702_GET_REG(TxControl_Reg) & ~0x03);
	sleep_us(usec);
	FM1702_SET_REG(TxControl_Reg, FM1702_GET_REG(TxControl_Reg) | 0x03);
	return FM1702_OK;
}

// ��MIFARE������HALT����
int fm1702_mif_halt(void) {
	//int i;
	int rlen;
	char buf[3];
	
	//FM1702_SET_REG(BitFraming_Reg, 0x07);		//���һ���ֽڷ���7bit	
	FM1702_SET_REG(CRCPresetLSB, 0x63);
	//0
	//0	CRCMSBFirst, 0 = ��������λ�Ƚ���CRC����
	//0	CRC3309, 0 = ʹ��ISO14443A CRC�㷨
	//0	CRC8, 0 = ����16bit CRC
	//0	RxCRCEn, 0 = �رս���CRC                 *****
	//0	TxCRCEn, 0 = �رշ���CRC
	//1	ParityOdd, 1 = ��У��
	//1	ParityEn, 1 = ÿ�����ֽں����У��λ
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x03);	// �رս���CRC
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);


	buf[0] = CMD_HALTA;
	buf[1] = 0x0;
	if (cmd_send(Transmit, buf, 2) != FM1702_OK) {
 		//return err_chk(__FUNCTION__, __LINE__);
		rlen = err_chk(__FUNCTION__, __LINE__);
		if ((rlen & 0x3F) != 0) {
			return rlen;
		}
	}
#if 0
	// wait command send ok
	for(i = 10; i > 0; i--) {
		if (fifo_len() == 0) break;
		sleep_us(1000);
	}
#endif
	return FM1702_OK;	
}

// �Է���FM1702������Χ�ڵĿ�Ƭrequest����
int fm1702_mif_req(int f_all) {
	char buf[4];
	int rlen;

	FM1702_SET_REG(CRCPresetLSB, 0x63);
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);
	FM1702_SET_REG(BitFraming_Reg, 0x07);		//���һ���ֽڷ���7bit
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x03);	// �رս���CRC
	//bit3, CryptolOn,  0 = �رռ���, �������Զ���
	FM1702_SET_REG(Control_Reg, FM1702_GET_REG(Control_Reg) & (~0x08));

	buf[0] = f_all? CMD_WAKEUPA: CMD_REQA;
	if (cmd_send(Transceive, buf, 1) != FM1702_OK) {
		//return err_chk(__FUNCTION__, __LINE__);
		rlen = err_chk(__FUNCTION__, __LINE__);
		if ((rlen & 0x3F) != 0) {
			return rlen;
		}
	}
#if 0
	fm1702_timeout(&stm, 0);
	while (fifo_len() < 2) {
		if (fm1702_timeout(&stm, 10)) break;
	}
#endif
	if ((
	//rlen = 
	read_fifo(buf)) > 0) {
#if 0
		debug_buf("mif_req(): RECV", buf, rlen);
#endif
		if (detect_req(buf)) {
			return FM1702_OK;
		}		
	}
	// printf("fm1702_mif_req(): error req");
	return FM1702_ERR_REQ;
}

// �Է���FM1702������Χ�ڵĿ�Ƭ������ͻ���
int fm1702_mif_anticol(char* uid) {
	char p_row = 0;
	char row = 0;
	char col = 0;
	char buf[8];
	int r, rlen;

	FM1702_SET_REG(CRCPresetLSB, 0x63);
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);
	FM1702_SET_REG(ModConductance_Reg, 0x3F);
	//FM1702_SET_REG(BitFraming_Reg, 0x07);	//���һ���ֽڷ���7bit
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x03);	// �رս���CRC

	buf[0] = CMD_ANTICOL;
	buf[1] = 0x20;
	r = cmd_send(Transceive, buf, 2);
	
	while (1) {
		if (r != FM1702_OK) {
			//return err_chk(__FUNCTION__, __LINE__);
			rlen = err_chk(__FUNCTION__, __LINE__);
			if ((rlen & 0x3F) != 0) {
				return rlen;
			}
		}
		if ((r = read_fifo(buf)) <= 0) {
			return err_chk(__FUNCTION__, __LINE__);
		}
		trans_uid(((int)row << 8) + col, uid, buf, r);

		r = FM1702_GET_REG(ErrorFlag_Reg) & 0x01;
		if (r == 0x00) {
			if (!check_uid(buf)) {
				return FM1702_ERR_SERNR;
			}
			break;
		}

		r = FM1702_GET_REG(CollPos_Reg);
		row = r >> 3;
		col = r & 0x07;
		p_row += row;
		
		set_bitframe(((int)p_row << 8) + col, buf);

		buf[0] = CMD_ANTICOL;
		memcpy(&buf[2], &uid[0], p_row + 1);

		row = (col != 0) ? p_row + 1: p_row;
		r = cmd_send(Transceive, buf, row + 2);
	}
	return FM1702_OK;
}

//��FM1702������Χ�ڵ�ĳ�ſ�����ѡ��
int fm1702_mif_sel(char* uid) {
	int rlen;
	char dummy;
	char buf[8];

	FM1702_SET_REG(CRCPresetLSB, 0x63);
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);

	buf[0] = CMD_SELECT;
	buf[1] = 0x70;
	memcpy(&buf[2], uid, 5);

	FM1702_SET_REG(ChannelRedundancy_Reg, 0x0F);	// �򿪷��ͼ�����CRC
	if (cmd_send(Transceive, buf, 7) != FM1702_OK) {
		return FM1702_ERR_NOTAG;
	}
	dummy = FM1702_GET_REG(ErrorFlag_Reg);
	if ((dummy & 0x0E) != 0) {
		//return err_chk(__FUNCTION__, __LINE__);
		rlen = err_chk(__FUNCTION__, __LINE__);
		if ((rlen & 0x3F) != 0) {
			return rlen;
		}
	}

	if (read_fifo(buf) < 1) {
		//return err_chk(__FUNCTION__, __LINE__);
		rlen = err_chk(__FUNCTION__, __LINE__);
		if ((rlen & 0x3F) != 0) {
			return rlen;
		}
	}
	if (buf[0] == 0x08 ||
	  buf[0] == 0x88 ||
	  buf[0] == 0x53) {
	  	return FM1702_OK;
	}
	//return err_chk(__FUNCTION__, __LINE__);
	rlen = err_chk(__FUNCTION__, __LINE__);
	if ((rlen & 0x3F) != 0) {
		return rlen;
	}
	return FM1702_OK;
}

//������֤�Ĺ���
int fm1702_mif_auth(char* uid, int mif_blk) {
	char buf[8];

	FM1702_SET_REG(CRCPresetLSB, 0x63);
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);
	FM1702_SET_REG(ModConductance_Reg, 0x3F);
	//bit3, CryptolOn,  0 = �رռ���, �������Զ���
	FM1702_SET_REG(Control_Reg, FM1702_GET_REG(Control_Reg) & ~(0x08));
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x0F);	// �򿪷��ͼ�����CRC
	
	buf[0] = CMD_AUTHA;
	buf[1] = mif_blk;
	memcpy(&buf[2], uid, 4);
	//debug_buf("Authent1", buf, 6);

	if (cmd_send(Authent1, buf, 6) != FM1702_OK) {
		return FM1702_ERR_NOTAG;
	}
	if ((FM1702_GET_REG(ErrorFlag_Reg) & 0x0E) != 0) {
		return err_chk(__FUNCTION__, __LINE__);
	}

	if (cmd_send(Authent2, buf, 0) != FM1702_OK) {
		return FM1702_ERR_NOTAG;
	}
	if ((FM1702_GET_REG(ErrorFlag_Reg) & 0x0E) != 0) {
		return err_chk(__FUNCTION__, __LINE__);
	}

	if ((FM1702_GET_REG(Control_Reg) & 0x08) == 0x08) {
		return FM1702_OK;
	}
	return FM1702_ERR_AUTH;
}

// ��MIFARE��ĳ�������
//���ض�ȡ�ֽ���,���ߴ�����
int fm1702_mif_read(int mif_blk, char* buf) {
	char rwbuf[4];
	int rlen;

	FM1702_SET_REG(CRCPresetLSB, 0x63);
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);
	FM1702_SET_REG(ModConductance_Reg, 0x3F);
	//FM1702_SET_REG(ChannelRedundancy_Reg, 0x07);	// �رս���CRC
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x0F);	// �򿪷��ͼ�����CRC
	//FM1702_SET_REG(InterruptRq_Rg, 0x7F);

	rwbuf[0] = CMD_READ;
	rwbuf[1] = mif_blk;
	if ((rlen = cmd_send(Transceive, rwbuf, 2)) != FM1702_OK) {
		//return FM1702_ERR_NOTAG;
		return rlen;
	}

	if ((FM1702_GET_REG(ErrorFlag_Reg) & 0x0E) != 0) {
		return err_chk(__FUNCTION__, __LINE__);
	}
	//sleep_us(5000);
	rlen = read_fifo(buf);
	if (rlen == 0x10 || rlen == 0x04) {
		return rlen;
	}
	return err_chk(__FUNCTION__, __LINE__);
}

// д���ݵ�MIFARE��ĳ��
// ����FM1702_OK �������
int fm1702_mif_write(int mif_blk, char* buf) {
	char rwbuf[4];
	int rlen;

	FM1702_SET_REG(CRCPresetLSB, 0x63);
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x07);	// �رս���CRC

	rwbuf[0] = CMD_WRITE;
	rwbuf[1] = mif_blk;
	if ((rlen = cmd_send(Transceive, rwbuf, 2)) != FM1702_OK) {
		printf("\r\ncmd_send() = %d", rlen);
		//return FM1702_ERR_NOTAG;
		return rlen;
	}
#if 0
	fm1702_timeout(&stm, 0);
	while (fifo_len() != 1) {
		if (fm1702_timeout(&stm, 10))
			break;
	}
#endif
	//sleep_us(1000);
	rlen = fifo_len();
	if (read_fifo(rwbuf) != 1) {
		return err_chk(__FUNCTION__, __LINE__);
	}
	if (rwbuf[0] != 0x0A) {
		return err_chk(__FUNCTION__, __LINE__);
	}
	if (cmd_send(Transceive, buf, 16) != FM1702_OK) {
		return err_chk(__FUNCTION__, __LINE__);
	}

	//sleep(12);
#if 1
	fm1702_timeout(&stm, 0);
	while (fifo_len() < 1) {
		if (fm1702_timeout(&stm, 10)) break;
	}
#endif
	if (read_fifo(rwbuf) != 1) {
		return err_chk(__FUNCTION__, __LINE__);
	}
	if (rwbuf[0] != 0x0A) {
		return err_chk(__FUNCTION__, __LINE__);
	}
	return FM1702_OK;
}


/////////////////////////////////////TEST CODE//////////////////////////////////


#if defined(REQ_TEST)
int iso14443_cmd(char* buf, int len) {
	//int i;
	static int d_us = 1;
	int rlen;
	static char rsvbuf[65];

	printf("\r\nd_us = %d us", d_us);
	
	//FM1702_SET_REG(BitFraming_Reg, 0x07);	//���һ���ֽڷ���7bit	
	//0
	//0	CRCMSBFirst, 0 = ��������λ�Ƚ���CRC����
	//0	CRC3309, 0 = ʹ��ISO14443A CRC�㷨
	//0	CRC8, 0 = ����16bit CRC
	//0	RxCRCEn, 0 = �رս���CRC                 *****
	//0	TxCRCEn, 0 = �رշ���CRC
	//1	ParityOdd, 1 = ��У��
	//1	ParityEn, 1 = ÿ�����ֽں����У��λ
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x03);
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);

	rsvbuf[0] = CMD_HALTA;
	rsvbuf[1] = 0x0;
	if (cmd_send(Transmit, rsvbuf, 2) != 0) {
 		return err_chk(__FUNCTION__, __LINE__);
	}
#if 0
	// wait command send ok
	for(i = 10; i > 0; i--) {
		if (fifo_len() == 0) break;
		sleep_us(1000);
	}
#endif
	FM1702_SET_REG(CWConductance_Reg, CW_VAL);
	FM1702_SET_REG(BitFraming_Reg, 0x07);	//���һ���ֽڷ���7bit
	FM1702_SET_REG(ChannelRedundancy_Reg, 0x03);	//�رս���CRC

	//bit3, CryptolOn,  0 = �رռ���, �������Զ���
	FM1702_SET_REG(Control_Reg, FM1702_GET_REG(Control_Reg) & ~(0x08));

	//buf[0] = CMD_WAKEUPA;
	//len = 1;
#if 0
	debug_buf("cmd_send(): ", buf, len);
#endif
	//printf("c\r\n");
	sleep_us(d_us);
	d_us += 50;

	if (cmd_send(Transceive, buf, len) != 0) {
		return err_chk(__FUNCTION__, __LINE__);
	}

#if 0
	fm1702_timeout(&stm, 0);
	while (fifo_len() < 2) {
		if (fm1702_timeout(&stm, 10)) break;
	}
#endif
	if ((rlen = read_fifo(rsvbuf)) > 0) {
		printf("\r\n%s()", __FUNCTION__);
		debug_buf("RECV", rsvbuf, rlen);
		return rlen;
	}
	return FM1702_OK;
}


int iso14443_test(void) {

	while (1) {
		char sndbuf[64];
		int cnt = 1;
#if 1
		int i, dummy;
		printf("\r\ninput bytes?");
		ascanf("%d", &cnt);
		for (i = 0; i < cnt; i++) {
			ascanf("%X", &dummy);
			sndbuf[i] = (dummy & 0xFF);
		}
#endif

		if ((cnt = iso14443_cmd(sndbuf, cnt)) >= 0) {
			printf("\r\niso14443 cmd ok %d", cnt);
		} else {
			printf("\r\n");
		}
		//sleep(500);
	}
}

#endif

#if FM1702_CW_DEBUG
int fm1702_cw_debug(void) {
	int ch = 0;

	if ((ch = peekch()) == EOF || ch != ' ') {
		return 0;
	}

#if 0
	printf("\r\ninput CWConductance_Reg Values?");
	if (++CW_VAL > 0x3F) {
		CW_VAL = 0;
	}
	//ascanf("%d", &CW_VAL);
	printf("=%d", CW_VAL);
#endif

#if 0
	printf("\r\ninput ModWidth_Reg Values?");
	if (++MOD_W_VAL > 0xFF) {
		MOD_W_VAL = 0;
	}
	printf("=%d", MOD_W_VAL);
	//ascanf("%d", &MOD_W_VAL);
#endif
	init_fm1702();
	return 0;
}

#if FM1702_CW_DEBUG
			fm1702_cw_debug();
#endif

#endif

/************************************END OF FILE******************************/
