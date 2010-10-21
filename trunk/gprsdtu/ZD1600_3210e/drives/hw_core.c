
#include "hw_core.h"
#include "stm32f10x_nvic.h"

	vu32 stacked_r0;  
	vu32 stacked_r1;  
	vu32 stacked_r2;  
	vu32 stacked_r3;  
	vu32 stacked_r12;  
	vu32 stacked_lr;  
	vu32 stacked_pc;  
	vu32 stacked_psr;



void system_reset(void )
{
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
  NVIC_GenerateCoreReset();
}

void hardfault_isr(u32 *hardfault_args)
{
  
	
	stacked_r0 = ((unsigned long) hardfault_args[0]);  
	stacked_r1 = ((unsigned long) hardfault_args[1]);  
	stacked_r2 = ((unsigned long) hardfault_args[2]);  
	stacked_r3 = ((unsigned long) hardfault_args[3]);  
	
	stacked_r12 = ((unsigned long) hardfault_args[4]);  
	stacked_lr = ((unsigned long) hardfault_args[5]);  
	stacked_pc = ((unsigned long) hardfault_args[6]);  
	stacked_psr = ((unsigned long) hardfault_args[7]);
/*  
	printf ("[Hard fault handler]\n");  
	printf ("R0 = %x\n", stacked_r0);  
	printf ("R1 = %x\n", stacked_r1);  
	printf ("R2 = %x\n", stacked_r2);  
	printf ("R3 = %x\n", stacked_r3);  
	printf ("R12 = %x\n", stacked_r12);  
	printf ("LR = %x\n", stacked_lr);  
	printf ("PC = %x\n", stacked_pc);  
	printf ("PSR = %x\n", stacked_psr);  
	printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));  
	printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));  
	printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));  
	printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));  
	printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));  
*/
	
	while(1)
	{
	
	} 
NVIC_SETFAULTMASK();
NVIC_GenerateSystemReset(); 
//system_reset();
}





