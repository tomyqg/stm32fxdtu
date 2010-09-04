/******************************* (C) Embest ***********************************
* File Name          : rtc.c
* Author             : liren
*                      tary
* Date               : 2009-02-26
* Version            : 0.2
* Description        : rtc control functions 
******************************************************************************/

#ifndef	__RTC_H__
#define	__RTC_H__

#include <time.h>

#define RTC_CONFIGED        1
#define RTC_NOCONFIGED      2

int rtc_init(void);
time_t rtc_SetTime(struct tm * tm);
#endif	//__RTC_H__

/************************************END OF FILE******************************/
