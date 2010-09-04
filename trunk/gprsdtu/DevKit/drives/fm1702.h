/******************************* (C) Embest ***********************************
* File Name          : fm1702.h
* Author             : tary
* Date               : 2009-06-25
* Version            : 0.4u
* Description        : fm1702 device low level operations 
******************************************************************************/

#ifndef __FM1702_H__
#define __FM1702_H__

//;==============================================
//;FM1702控制寄存器定义
//;==============================================
#define		Page_Reg		0x00
#define		Command_Reg		0x01
#define		FIFOData_Reg		0x02
#define		FIFOLength_Reg		0x04
#define		SecondaryStatus_Reg  	0x05
#define		InterruptEn_Reg		0x06
#define		InterruptRq_Reg		0x07
#define		Control_Reg		0x09
#define		ErrorFlag_Reg		0x0A
#define		CollPos_Reg		0x0B
#define		TimerValue_Reg		0x0C
#define		BitFraming_Reg	     	0x0F
#define		TxControl_Reg		0x11
//定义输出电导，用来调整输出能量，电流消耗，及操作距离
#define		CWConductance_Reg	0x12
#define		ModConductance_Reg	0x13
#define		ModWidth_Reg		0x15
#define		RxThreshold_Reg		0x1C
#define		RxControl2_Reg		0x1E
#define  	RxWait_Reg          	0x21
#define		ChannelRedundancy_Reg	0x22
#define		CRCPresetLSB		0x23
#define		CRCPresetMSB		0x24
#define  	MFOUTSelect_Reg        	0x26
#define  	FIFOLevel_Reg          	0x29
#define		TimerClock_Reg		0x2A
#define		TimerControl_Reg	0x2B
#define		TimerReload_Reg		0x2C



//;==============================================
//;FM1702发送命令代码
//;==============================================
//读FM1702 EEPROM命令, 并写入FIFO
#define 	ReadE2			0x03
//写FM1702 EEPROM命令, 数据来自FIFO
#define 	WriteE2			0x01
//将密钥从EEPROM复制到KEY缓存
#define		LoadKeyE2		0x0B
//将密钥从FIFO复制到KEY缓存
#define		LoadKey			0x19
//发送FIFO缓存数据
#define		Transmit                0x1A
//发送FIFO缓存数据并自动激活接收
#define		Transceive		0x1E
//证认第一步
#define		Authent1		0x0C
//证认第二步
#define		Authent2		0x14


int fm1702_init(void);
int fm1702_reg_initial(int fix);
int fm1702_mif_key_e2p(int mif_sec);
int fm1702_mif_key(int mif_sec, char* key);
int fm1702_mif_no_energy(int usec);
int fm1702_mif_halt(void);
int fm1702_mif_req(int f_all);
int fm1702_mif_anticol(char* uid);
int fm1702_mif_sel(char* uid);
int fm1702_mif_auth(char* uid, int mif_blk);
int fm1702_mif_read(int mif_blk, char* buf);
int fm1702_mif_write(int mif_blk, char* buf);

//FM1702的EEPROM操作
int fm1702_read_e2p(int addr, char* buf, int len);
int fm1702_write_e2p(int addr, char* buf, int len);

//#define REQ_TEST	1

#if defined(REQ_TEST)
//int iso14443_cmd(char* buf, int len);
int iso14443_test(void);
#endif


/* FM1702函数的返回值 */
#define FM1702_OK			0
#define FM1702_ERR_NOTAG		(-1)
#define FM1702_ERR_CRC			(-2)
#define FM1702_ERR_EMPTY		(-3)
#define FM1702_ERR_AUTH			(-4)
#define FM1702_ERR_PARITY		(-5)
#define FM1702_ERR_CODE			(-6)
#define FM1702_ERR_SERNR		(-7)
#define FM1702_ERR_SELECT		(-8)
#define FM1702_ERR_BIT			(-11)
#define FM1702_ERR_BYTE			(-12)
#define FM1702_ERR_WRITE		(-15)
#define FM1702_ERR_READ			(-18)
#define FM1702_ERR_LOADKEY		(-19)
#define FM1702_ERR_FRAME		(-20)
#define FM1702_ERR_REQ			(-21)
#define FM1702_ERR_SEL			(-22)
#define FM1702_ERR_ANTICOL		(-23)
#define FM1702_ERR_INIT			(-24)
#define FM1702_ERR_DESELECT		(-26)
#define FM1702_ERR_CMD			(-30)
#define FM1702_ERR_FIFO			(-31)
#define FM1702_ERR_E2P			(-32)
#define FM1702_ERR_REG			(-33)

// return = ErrorFlag + FM1702_E_BASE
#define FM1702_E_BASE			(-256)

//是否输出错误寄存器信息
#define FM1702_DEBUG			0

//是否调试CWConductance寄存器
#define FM1702_CW_DEBUG			0

#if FM1702_CW_DEBUG
extern int CW_VAL;
extern int MOD_W_VAL;
extern int fm1702_cw_debug(void);
#else
#define CW_VAL				(0x3F)
#define MOD_W_VAL			(0x13)
#endif

#endif //__FM1702_H__

/************************************END OF FILE******************************/
