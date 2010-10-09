#ifndef	__HW_CORE_H__
#define	__HW_CORE_H__

#include "stm32f10x_type.h"





void system_reset(void );

void hardfault_isr(u32 *hardfault_args);




#endif

