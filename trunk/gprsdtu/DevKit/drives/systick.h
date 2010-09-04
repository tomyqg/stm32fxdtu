/******************************* (C) Embest ***********************************
* File Name          : systick.h
* Author             : tary 
* Date               : 2009-06-25
* Version            : 0.2u
* Description        : generate the system clock and sleep time
******************************************************************************/

#ifndef	__SYSTICK_H__
#define	__SYSTICK_H__

#include "ucos_ii.h"

__inline int systick_get_freq(void) {
	return OS_TICKS_PER_SEC;
}

//unit : milliseconds
__inline unsigned long sys_elapse_time(void) {
	return OSTimeGet() * 1000L / OS_TICKS_PER_SEC;
}

#define sleep(x)	sleep_us((x) * 1000)

#endif	//__SYSTICK_H__

/************************************END OF FILE******************************/
