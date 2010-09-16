#include <string.h>
#include "stm32f10x_type.h"

#include "gd_mem.h"
#include "gd_system.h"

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


/************************* Encapsulate Memery Function *****************************/

extern gd_system_t gd_system;

INT8U sp2gm_buf_partition[SP2GM_FRAME_NODE_COUNT][GD_FRAME_NODE_SIZE];
INT8U sp2gm_buf[SP2GM_BUF_LEN];
int   sp2gm_start_pos = 0;
int	  sp2gm_free_pos = 0;

INT8U gm2sp_buf_partition[GM2SP_FRAME_NODE_COUNT][GD_FRAME_NODE_SIZE];
INT8U gm2sp_buf[GM2SP_BUF_LEN];
int   gm2sp_start_pos = 0;
int	  gm2sp_free_pos = 0;


//////////////////////////////// SP2GM Memery Function////////////////////////////

// If buf is full, overwrite the frames from beginning
int sp2gm_buf_full_process()
{
	frame_node_t 	*node = NULL;

	sp2gm_free_pos = sp2gm_start_pos = 0;
	
	if(gd_system.sp2gm_frame_list.head == NULL)
		return 0;
	
	while(1)
	{		
		node = gd_system.sp2gm_frame_list.head;
		
		if(node->next == NULL)
		{
			OSMemPut(gd_system.sp2gm_buf_PartitionPtr, (void*)node);
			gd_system.sp2gm_frame_list.head = NULL;
			return 0;
		}
	
		gd_system.sp2gm_frame_list.head = node->next;
		OSMemPut(gd_system.sp2gm_buf_PartitionPtr, (void*)node);
	}	

	
	return 0;
}

int sp2gm_adjust_buf_pos(int frame_len)
{
	if(sp2gm_start_pos == 0 && sp2gm_free_pos == 0)
	{
		return 0;
	}
	else if(sp2gm_free_pos > sp2gm_start_pos)
	{
		if((sp2gm_free_pos+frame_len) < SP2GM_BUF_LEN)
		{
			return 0;
		}
		else 
		{
			if(frame_len < sp2gm_start_pos)
			{
				sp2gm_free_pos = 0;
			}
			else // Buf is full
			{
				sp2gm_buf_full_process();
			}
		}
	}
	else if(sp2gm_free_pos < sp2gm_start_pos)
	{
		if((sp2gm_start_pos-sp2gm_free_pos) > frame_len)
			return 0;
		else // Buf is full
		{
			sp2gm_buf_full_process();
		}
	}
		
	return 0;
}

int sp2gm_cache_frame(char *frame, int frame_len)
{
	INT8U 			err;
	frame_node_t 	*node = NULL;
	frame_node_t    *next_node = NULL;

	// Adjust the start_pos and free_pos
	sp2gm_adjust_buf_pos(frame_len);

	node = gd_system.sp2gm_frame_list.head;
	
	if(gd_system.sp2gm_frame_list.head == NULL)
	{
		gd_system.sp2gm_frame_list.head = (frame_node_t *)OSMemGet(gd_system.sp2gm_buf_PartitionPtr, &err);
		gd_system.sp2gm_frame_list.head->pFrame = &sp2gm_buf[sp2gm_free_pos];
		gd_system.sp2gm_frame_list.head->len	= frame_len;
		gd_system.sp2gm_frame_list.head->next = NULL;
		memcpy(gd_system.sp2gm_frame_list.head->pFrame, frame, frame_len);
		sp2gm_free_pos += frame_len; 
	}
	else
	{
		while(1)
		{
			next_node = node->next;
			if(next_node == NULL)
				break;

			node = next_node;
		}

		next_node = (frame_node_t *)OSMemGet(gd_system.sp2gm_buf_PartitionPtr, &err);
		if(next_node == NULL)
			return -1;

		next_node->pFrame = &sp2gm_buf[sp2gm_free_pos];
		next_node->len	= frame_len;
		next_node->next = NULL;
		node->next = next_node;
		memcpy(next_node->pFrame, frame, frame_len);
		sp2gm_free_pos += frame_len; 

	}

	gd_system.sp2gm_frame_list.frame_count++;

	return 0;
}

int sp2gm_remove_frame(frame_node_t *frame_node)
{
	frame_node_t 	*node = NULL;

	if(gd_system.sp2gm_frame_list.head == NULL)
		return -1;

	node = gd_system.sp2gm_frame_list.head;

	if(node->pFrame == frame_node->pFrame)
	{
		if(node->next == NULL)
		{
			OSMemPut(gd_system.sp2gm_buf_PartitionPtr, (void*)node);
			gd_system.sp2gm_frame_list.head = NULL;
			gd_system.sp2gm_frame_list.frame_count = 0;
			sp2gm_start_pos = 0;
			sp2gm_free_pos = 0;
		}
		else
		{
			gd_system.sp2gm_frame_list.head = node->next;
			sp2gm_start_pos = (int)(node->pFrame - &sp2gm_buf[0]);
			OSMemPut(gd_system.sp2gm_buf_PartitionPtr, (void*)node);
			gd_system.sp2gm_frame_list.frame_count--;
		}
	}
	else // Error
		return -1;
}


//////////////////////////////// GM2SP Memery Function////////////////////////////

// If buf is full, overwrite the frames from beginning
int gm2sp_buf_full_process()
{
	frame_node_t 	*node = NULL;

	gm2sp_free_pos = gm2sp_start_pos = 0;
	
	if(gd_system.gm2sp_frame_list.head == NULL)
		return 0;
	
	while(1)
	{		
		node = gd_system.gm2sp_frame_list.head;
		
		if(node->next == NULL)
		{
			OSMemPut(gd_system.gm2sp_buf_PartitionPtr, (void*)node);
			gd_system.gm2sp_frame_list.head = NULL;
			return 0;
		}
	
		gd_system.gm2sp_frame_list.head = node->next;
		OSMemPut(gd_system.gm2sp_buf_PartitionPtr, (void*)node);
	}	

	
	return 0;
}

int gm2sp_adjust_buf_pos(int frame_len)
{
	if(gm2sp_start_pos == 0 && gm2sp_free_pos == 0)
	{
		return 0;
	}
	else if(gm2sp_free_pos > gm2sp_start_pos)
	{
		if((gm2sp_free_pos+frame_len) < GM2SP_BUF_LEN)
		{
			return 0;
		}
		else 
		{
			if(frame_len < gm2sp_start_pos)
			{
				gm2sp_free_pos = 0;
			}
			else // Buf is full
			{
				gm2sp_buf_full_process();
			}
		}
	}
	else if(gm2sp_free_pos < gm2sp_start_pos)
	{
		if((gm2sp_start_pos-gm2sp_free_pos) > frame_len)
			return 0;
		else // Buf is full
		{
			gm2sp_buf_full_process();
		}
	}
		
	return 0;
}

int gm2sp_cache_frame(char *frame, int frame_len)
{
	INT8U 			err;
	frame_node_t 	*node = NULL;
	frame_node_t    *next_node = NULL;

	// Adjust the start_pos and free_pos
	gm2sp_adjust_buf_pos(frame_len);

	node = gd_system.gm2sp_frame_list.head;
	
	if(gd_system.gm2sp_frame_list.head == NULL)
	{
		gd_system.gm2sp_frame_list.head = (frame_node_t *)OSMemGet(gd_system.gm2sp_buf_PartitionPtr, &err);
		gd_system.gm2sp_frame_list.head->pFrame = &gm2sp_buf[gm2sp_free_pos];
		gd_system.gm2sp_frame_list.head->len	= frame_len;
		gd_system.gm2sp_frame_list.head->next = NULL;
		memcpy(gd_system.gm2sp_frame_list.head->pFrame, frame, frame_len);
		gm2sp_free_pos += frame_len; 
	}
	else
	{
		while(1)
		{
			next_node = node->next;
			if(next_node == NULL)
				break;

			node = next_node;
		}

		next_node = (frame_node_t *)OSMemGet(gd_system.gm2sp_buf_PartitionPtr, &err);
		if(next_node == NULL)
			return -1;

		next_node->pFrame = &gm2sp_buf[gm2sp_free_pos];
		next_node->len	= frame_len;
		next_node->next = NULL;
		node->next = next_node;
		memcpy(next_node->pFrame, frame, frame_len);
		gm2sp_free_pos += frame_len; 

	}

	gd_system.gm2sp_frame_list.frame_count++;

	return 0;
}

int gm2sp_remove_frame(frame_node_t *frame_node)
{
	frame_node_t 	*node = NULL;

	if(gd_system.gm2sp_frame_list.head == NULL)
		return -1;

	node = gd_system.gm2sp_frame_list.head;

	if(node->pFrame == frame_node->pFrame)
	{
		if(node->next == NULL)
		{
			OSMemPut(gd_system.gm2sp_buf_PartitionPtr, (void*)node);
			gd_system.gm2sp_frame_list.head = NULL;
			gd_system.gm2sp_frame_list.frame_count = 0;
			gm2sp_start_pos = 0;
			gm2sp_free_pos = 0;
		}
		else
		{
			gd_system.gm2sp_frame_list.head = node->next;
			gm2sp_start_pos = (int)(node->pFrame - &gm2sp_buf[0]);
			OSMemPut(gd_system.gm2sp_buf_PartitionPtr, (void*)node);
			gd_system.gm2sp_frame_list.frame_count--;
		}
	}
	else // Error
		return -1;
}





























