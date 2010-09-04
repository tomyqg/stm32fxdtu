/******************************* (C) Embest ***********************************
* File Name          : rtc.c
* Author             : liren
*                      tary
* Date               : 2009-02-26
* Version            : 0.2
* Description        : rtc control functions 
******************************************************************************/

/* Includes ------------------------------------------------------------------*/


#include <stdio.h>
#include "stm32f10x_lib.h"
#include "rtc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

#define RTC_DEBUG		0
#define RTC_SHOW_TIME		0


/* Private function prototypes -----------------------------------------------*/
static void RTC_Configuration(void);
//u32 Time_Regulate(void);
static void Time_Adjust(unsigned int);


/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int rtc_init(void) {
  int ret = RTC_CONFIGED;

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */
#if RTC_DEBUG
    printf("\r\n\n RTC not yet configured....");

    printf("\r\n RTC configured....");
#endif
    /* Adjust time by values entred by the user on the hyperterminal */
    Time_Adjust(0);
    ret = RTC_NOCONFIGED;
    
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
#if RTC_DEBUG
      printf("\r\n\n Power On Reset occurred....");
#endif
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
#if RTC_DEBUG
      printf("\r\n\n External Reset occurred....");
#endif
    }
#if RTC_DEBUG
    printf("\r\n No need to configure RTC....");
#endif
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

  }

#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

  /* Clear reset flags */
  RCC_ClearFlag();

  return ret;
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : Time_Adjust
* Description    : Adjusts time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Time_Adjust(unsigned int cnt)
{
  /* RTC Configuration */
  RTC_Configuration();
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(cnt);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  
}

time_t time(time_t * timer)
{
    time_t tm;
    tm = (time_t)RTC_GetCounter();
    if (timer != NULL)
        *timer = tm;
    return tm;
}

time_t rtc_SetTime(struct tm * tm)
{
    time_t time;
    time = mktime(tm);
    if (time != 0xffffffff)
        Time_Adjust(time);
            
    printf("time cnt %d", time);
    return time;
}
/************************************END OF FILE******************************/
