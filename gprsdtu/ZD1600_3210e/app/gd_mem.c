
#include "gd_mem.h"
#include <string.h>
#include "stm32f10x_type.h"

#define PA 0x40010800	
#define PB 0x40010C00
#define PC 0x40011000
#define PD 0x40011400

#define CRL  0x00	
#define CRH  0x04
#define IDR  0x08
#define ODR  0x0C
#define BSRR 0x10
#define BRR  0x14
#define LCKR 0x18

#define		ADDRESS16_1			*(u32*)(PB + ODR) =
#define		ADDRESS20_17		*(u32*)(PA + ODR) = (*(u32*)(PA + ODR) & 0xFFFFFF0F) |
#define		ADDR_0				*(u32*)(PA + ODR) &= ~(1 << 0);	*(u32*)(PA + ODR) |=  (1 << 1)
#define		ADDR_1				*(u32*)(PA + ODR) |=  (1 << 0);	*(u32*)(PA + ODR) &= ~(1 << 1)

#define		RD_L				*(u32*)(PC + ODR) &= ~(1 << 13)
#define		RD_H				*(u32*)(PC + ODR) |=  (1 << 13)

#define		WR_L				*(u32*)(PC + ODR) &= ~(1 << 14)
#define		WR_H				*(u32*)(PC + ODR) |=  (1 << 14)

#define		CS_L				*(u32*)(PC + ODR) &= ~(1 << 15)
#define		CS_H				*(u32*)(PC + ODR) |=  (1 << 15)

#define		DAT_IN				*(u32*)(PC + IDR) & 0x00FF
#define		DAT_OUT				*(u32*)(PC + ODR) =  (*(u32*)(PC + ODR) & 0xFFFFFF00) |

#define		MODE_IN()			*(u32*)(PC + CRL) = 0x77777777
#define		MODE_OUT()			*(u32*)(PC + CRL) = 0x33333333



////////////////////////////////////////////////////////////////////////////////

void InitPSRAM(void);
void PutPSRAM(u32 addr, u8 dat);
u16 GetPSRAM(u32 addr);
void PutAddress(u32 addr);


////////////////////////////////////////////////////////////////////////////////
void InitPSRAM(void)
{
	CS_H;
	RD_H;
	WR_H;
	
	*(u32*)(PB + CRL) = (*(u32*)(PB + CRL) & 0x00000000) | 0x33333333;			//ADDRESS 8:1
	*(u32*)(PB + CRH) = (*(u32*)(PB + CRH) & 0x00000000) | 0x33333333;			//ADDRESS 16:9
	*(u32*)(PA + CRL) = (*(u32*)(PA + CRH) & 0x0000FF00) | 0x33330033;			//ADDRESS 20:17  ADDRESS 0(UB/LB)
	*(u32*)(PC + CRH) = (*(u32*)(PC + CRH) & 0x000FFF00) | 0x33300033;			//CS/WR/OE

	MODE_IN();
}

////////////////////////////////////////////////////////////////////////////////
void PutAddress(u32 addr)
{
	if (addr & 0x00000001)	{ADDR_1;}
	else 					{ADDR_0;}
	ADDRESS16_1(addr >> 1);
	ADDRESS20_17(addr >> (16 - 4));
	
}

////////////////////////////////////////////////////////////////////////////////
void PutPSRAM(u32 addr, u8 dat)
{
	MODE_OUT();
	PutAddress(addr);
	
	CS_L;
	DAT_OUT(dat);
	WR_L;	WR_H;
	CS_H;
	
	MODE_IN();
}

////////////////////////////////////////////////////////////////////////////////
u16 GetPSRAM(u32 addr)
{
	u16 dat;
	CS_L;
	PutAddress(addr);
	RD_L;	
	dat = DAT_IN;	
	RD_H;
	CS_H;
	return dat;	
}






