/******************************* (C) Embest ***********************************
* File Name          : fm1702_inf.c
* Author             : tary
* Date               : 2009-06-25
* Version            : 0.4u
* Description        : fm1702 device low level operations 
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "fm1702_inf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

// 检测超时
int fm1702_timeout(unsigned long *start_ms, int m_secs) {
	unsigned long tm_now, ticks;

	ticks = OSTimeGet();
	tm_now = (ticks * 1000) / OS_TICKS_PER_SEC;

	if (start_ms == NULL) {
		return tm_now > m_secs;
	}

	if (m_secs == 0) {
		*start_ms = tm_now;
		return 0;
	}
	return (tm_now - *start_ms) > m_secs;
}

// 设置寄存器,单值操作
int FM1702_SET_REG (int reg, int val) {
	FM1702_NSS_LOW();
	DISABLE_IRQ();
	fm1702_inf_rwbyte(reg << 1);
	fm1702_inf_rwbyte(val);
	ENABLE_IRQ();
	FM1702_NSS_HIGH();
	return val;
}

// 读取寄存器,单值操作
int FM1702_GET_REG(int reg) {
	FM1702_NSS_LOW();
	DISABLE_IRQ();
	fm1702_inf_rwbyte((reg << 1) | 0x80);
	reg = fm1702_inf_rwbyte(0);
	ENABLE_IRQ();
	FM1702_NSS_HIGH();
	return reg;
}

//reg		要写到FM1702SL内的寄存器地址[0x01~0x3f] \n 
//buf		要写入的数据在RAM中的首地址 \n 
//len		要写入的字节数
int fm1702_inf_write(int reg, char* buf, int len) {
	int i;

	if ((reg & 0xC0) != 0) {
		return -1;
	}
	
	FM1702_NSS_LOW();
	DISABLE_IRQ();
	//写操作(最高位为0，最低位为0，1~6表示地址)
	reg = ((reg << 1) & 0x7E);
	fm1702_inf_rwbyte(reg);
	
	for (i = 0; i < len; i++) {
		fm1702_inf_rwbyte(buf[i]);
	}
	
	ENABLE_IRQ();
	FM1702_NSS_HIGH();
	return 0;
}

//reg		要读的FM1702SL内的寄存器地址[0x01~0x3f] \n 
//buf		要读出的数据在Ram中的首地址 \n 
//len		要读出的字节数
int fm1702_inf_read(int reg, char* buf, int len) {
	int i = 0;

	if ((reg & 0xC0) != 0) {
		return -1;
	}

	FM1702_NSS_LOW();
	DISABLE_IRQ();

	//读操作(最高位为1，最低位为0，1~6表示地址)
	reg = ((reg << 1) | 0x80);
	fm1702_inf_rwbyte(reg);
	
	for (i = 0; i < len - 1; i++) {
		buf[i] = fm1702_inf_rwbyte(reg);
	}
	//最后一个字节要求发送0
	buf[i] = fm1702_inf_rwbyte(0);

	ENABLE_IRQ();
	FM1702_NSS_HIGH();
	return 0;
}

/*******************************************************************************
* Description    : Write a byte on the SPI, then return the byte recevied.
*******************************************************************************/
u8 fm1702_inf_rwbyte(u8 bSnd) {
	u8 bRsv = 0;

	/* Wait until the transmit buffer is empty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Send the byte */
	SPI_I2S_SendData(SPI2, bSnd);

	/* Wait until a data is received */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	/* Get the received data */
	bRsv = SPI_I2S_ReceiveData(SPI2);
	/* Return the shifted data */
	// printf("\r\n0x%02X 0x%02X", bSnd, bRsv);
	return bRsv;
}


/*******************************************************************************
* Description    : Initializes the SPI2 and RST/NSS pins.
*******************************************************************************/
int fm1702_inf_init(void) {
	SPI_InitTypeDef SPI_InitStructure;

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* Configure SPI2 pins: SCK, MISO and MOSI */
	gpio_init(GPIOB,
	  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15
	  , GPIO_Speed_50MHz, GPIO_Mode_AF_PP,
	  RCC_APB2Periph_GPIOB
	);

	/* Configure NSS pin */
	/* NSS and RST pin clock enable */
	gpio_init(FM1702_NSS_PORT,
	  FM1702_NSS_PIN, GPIO_Speed_50MHz, GPIO_Mode_Out_PP,
	  RCC_APB2Periph_NSS
	);

	/* Configure RST pin */
	gpio_init(FM1702_RST_PORT,
	  FM1702_RST_PIN, GPIO_Speed_50MHz, GPIO_Mode_Out_PP,
	  RCC_APB2Periph_RST
	);

	/* SPI2 Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	/* SPI2 enable */
	SPI_Cmd(SPI2, ENABLE);
	return 0;
}

/************************************END OF FILE******************************/
