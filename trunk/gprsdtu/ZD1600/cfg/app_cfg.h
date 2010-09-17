/* 
********************************************************************************************************* 
* 
*                                      APPLICATION CONFIGURATION 
* 
*                                     ST Microelectronics STM32 
*                                              with the 
*                                   STM3210B-EVAL Evaluation Board 
* 
* Filename      : app_cfg.h 
* Version       : V1.00 
* Programmer(s) : Felix Tang
********************************************************************************************************* 
*/ 
 
#ifndef  __APP_CFG_H__ 
#define  __APP_CFG_H__ 
 
/* 
********************************************************************************************************* 
*                                       ADDITIONAL uC/MODULE ENABLES 
********************************************************************************************************* 
*/ 
 
#define  uC_PROBE_OS_PLUGIN              DEF_ENABLED            /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */ 
#define  uC_PROBE_COM_MODULE             DEF_ENABLED 

/* 
********************************************************************************************************* 
*                                            TASK PRIORITIES 
********************************************************************************************************* 
*/ 
#define  GD_TASK_CONFIG_PRIO					 1
#define  GD_TASK_GUART_RX_PRIO					 	 2
#define  GD_TASK_SUART_PRIO					 	 3
#define  GD_TASK_GUART_PRIO					 	 4

#define  GD_TASK_NETWORK_PRIO					 5
#define  GD_TASK_LED_PRIO                        6

#define  GD_TASK_INIT_PRIO						 7
#define  APP_TASK_TEST_PRIO						 8
 
#define  OS_TASK_TMR_PRIO              (OS_LOWEST_PRIO - 2) 
 
/* 
********************************************************************************************************* 
*                                            TASK STACK SIZES 
********************************************************************************************************* 
*/ 
#define  GD_TASK_INIT_STK_SIZE				   64
#define  GD_TASK_CONFIG_STK_SIZE			   512
#define  GD_TASK_SUART_STK_SIZE			   	   512
#define  GD_TASK_GUART_STK_SIZE			       512
#define  GD_TASK_GUART_RX_STK_SIZE			   64
#define  GD_TASK_NETWORK_STK_SIZE			   512
#define  GD_TASK_LED_STK_SIZE                  64
#define	 APP_TASK_TEST_STK_SIZE				   64

/* 
********************************************************************************************************* 
*                               uC/Probe plug-in for uC/OS-II CONFIGURATION 
********************************************************************************************************* 
*/ 
 
//#define  OS_PROBE_TASK                         1                /* Task will be created for uC/Probe OS Plug-In             */ 
//#define  OS_PROBE_TMR_32_BITS                  0                /* uC/Probe OS Plug-In timer is a 16-bit timer              */ 
//#define  OS_PROBE_TIMER_SEL                    2 
//#define  OS_PROBE_HOOKS_EN                     0 
 
 
 
 
#endif 
