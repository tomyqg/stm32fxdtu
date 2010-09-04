/******************************* (C) Embest ***********************************
* File Name          : timer.h
* Author             : Wang Shaowei
*                      tary 
* Date               : 2009-02-26
* Version            : 3.1
* Description        : virtual timer
******************************************************************************/


#ifndef __TIMER_H__
#define __TIMER_H__

#define MSTIMER_NUMBER		0x1F				/*max: 32虚拟定时器的数量*/
#define SETBIT(v, off)    	(v |= (1 << off))
#define RESETBIT(v, off)  	(v &= ~(1 << off))
#define GETBIT(v, off)	   	((v >> off) & 0x01)

typedef int (*Function)(int id);

int timer_dbg(int id);
int MSTimerStop(int id);
int MSTimerStart(int delay, Function pCallBack);
int MSTimerService(int ticks, int systick_period);
int MSTimerInterval(int interval, Function pCallBack);

#endif	//__TIMER_H__

/************************************END OF FILE******************************/
