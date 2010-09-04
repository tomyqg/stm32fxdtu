/******************************* (C) Embest ***********************************
* File Name          : timer.c
* Author             : Wang Shaowei
*                      tary 
* Date               : 2009-02-26
* Version            : 3.2
* Description        : virtual timer
******************************************************************************/
#include <stdio.h>
#include "vtimer.h"
#include "systick.h"
#include "aux_lib.h"


#define LOCAL_DBG		0

typedef struct {
	int interval;
	int delay;
	Function pCallBack;
}MSTIMER_T;

static volatile int MSTimerID = 0;
static volatile int MSTimerMode = 0;
static volatile MSTIMER_T MSTimerArray[MSTIMER_NUMBER];

/*=============================================================================
* Function	: MSTimerService
* Description	: will be called in systick Interrupt Service
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int MSTimerService(int ticks, int systick_period) {
	int i = 0;
	int MSTimerID_Map;
	MSTimerID_Map = MSTimerID;
	
	for (; MSTimerID_Map; MSTimerID_Map >>= 1, i++) {
		if((MSTimerID_Map & 0x01) == 0) {
			continue;
		}

		MSTimerArray[i].delay -= systick_period;
		if(MSTimerArray[i].delay > 0) {
			continue;
		}

		if(GETBIT(MSTimerMode, i)) {
			MSTimerArray[i].delay = MSTimerArray[i].interval;
		} else {
			RESETBIT(MSTimerID, i);
		}

		(*(MSTimerArray[i].pCallBack))(i);
	}
	return 0;
}

/*=============================================================================
* Function	: MSTimerStart
* Description	: start a virtual timer base on system timer
*		make sure there is enough timer, that is MSTIMER_MAX is big enough
* Input Para	: delay --- delay time, in the unit of system timer (normally it is 20ms).
*			so the max delay time is 65535 * 20 ms
*		 pCallBack --- callback function when MSTimer arrives
* Output Para	: 
* Return Value  : >=0 success, -1 failure
=============================================================================*/
int MSTimerStart(int delay, Function pCallBack) {
	int i = -1;

	if (delay <= 0) {
		return -1;
	}

	DISABLE_IRQ();	
	for (i = 0; i < MSTIMER_NUMBER; i++) {
		if(! GETBIT(MSTimerID, i)) {
			break;
		}
	}

	if (i < MSTIMER_NUMBER) {
		MSTimerArray[i].interval = 0;
		MSTimerArray[i].delay = delay;
		MSTimerArray[i].pCallBack = pCallBack;
		SETBIT(MSTimerID, i);
		RESETBIT(MSTimerMode, i);
	} else {
		i = -1;
		DBG_PRINT(" fail");
	}
	ENABLE_IRQ();

	return i;
}

/*=============================================================================
* Function	: MSTimerInterval
* Description	: start a virtual interval timer base on system timer
*		make sure there is enough timer, that is MSTIMER_MAX is big enough
* Input Para	: interval --- interval time, in the unit of system timer (normally it is 20ms).
*			so the max delay time is 65535 * 20 ms
*		 pCallBack --- callback function when MSTimer arrives
* Output Para	: 
* Return Value  : >=0 success, -1 failure
=============================================================================*/
int MSTimerInterval(int interval, Function pCallBack) {
	int i = -1;

	if (interval <= 0) {
		return -1;
	}

	DISABLE_IRQ();	
	for (i = 0; i < MSTIMER_NUMBER; i++) {
		if(! GETBIT(MSTimerID, i)) {
			break;
		}
	}

	if (i < MSTIMER_NUMBER) {
		MSTimerArray[i].interval = interval;
		MSTimerArray[i].delay = 0;
		MSTimerArray[i].pCallBack = pCallBack;
		SETBIT(MSTimerID, i);
		SETBIT(MSTimerMode, i);
	} else {
		i = -1;
		DBG_PRINT(" fail");
	}
	ENABLE_IRQ();

	return i;
}

/*=============================================================================
* Function	: MSTimerStop
* Description	: stop MSTimer
* Input Para	: id --- timer ID, should be less than MSTIMER_MAX
* Output Para	: 
* Return Value  : >=0 success, -1 failure
=============================================================================*/
int MSTimerStop(int id) {
	if (0 <= id && id < MSTIMER_NUMBER) {
		DISABLE_IRQ();
		RESETBIT(MSTimerID, id);
		ENABLE_IRQ();
		return 0;
	}
	DBG_PRINT(" failure");
	return -1;
}


/*=============================================================================
* Function	: timer_dbg
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int timer_dbg(int id) {
	static int power_on_secs = 0;

	/* Set Clock1s to 1 every 1 second    */
	DBG_PRINT(": **************** Power On %lu Seconds  ****************", power_on_secs++);
	MSTimerStart(1000, timer_dbg);
	return 0;
}

/************************************END OF FILE******************************/
