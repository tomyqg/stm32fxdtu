/* 
********************************************************************************************************* 
*                                              EXAMPLE CODE 
* 
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL 
* 
*               All rights reserved.  Protected by international copyright laws. 
*               Knowledge of the source code may NOT be used to develop a similar product. 
*               Please help us continue to provide the Embedded community with the finest 
*               software available.  Your honesty is greatly appreciated. 
********************************************************************************************************* 
*/ 
 
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
* Programmer(s) : Brian Nagel 
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
#define APP_TASK_MONITOR_PRIO                    3
#define SYS_TASK_KEY_SRV_PRIO                    4

#define SYS_FATFS_MUTEX_PRIO                     1

/*
    pppMain                 7
    tcpip                   8
 */
#define SYS_TASK_DIAL_SRV_PRIO                   10


#define APP_TASK_TEST_PRIO                       11
 
#define APP_TASK_MP3_PRIO			             12

#define  APP_TASK_LED_PRIO                       (20) 
 
//#define  APP_TASK_START_PRIO                   3 
//#define  APP_TASK_SHELL_PRIO				     10
//#define  APP_TASK_KBD_PRIO                     4 
//#define  APP_TASK_USER_IF_PRIO                 5 
//#define  APP_TASK_PROBE_STR_PRIO               6 
//#define  OS_PROBE_TASK_PRIO                    8 
//#define  OS_PROBE_TASK_ID                      8 
 
#define  OS_TASK_TMR_PRIO              (OS_LOWEST_PRIO - 2) 
 
/* 
********************************************************************************************************* 
*                                            TASK STACK SIZES 
********************************************************************************************************* 
*/ 

#define  APP_TASK_MONITOR_STK_SIZE             512 
#define SYS_TASK_KEY_SRV_STK_SIZE              128
#define SYS_TASK_DIAL_SRV_STK_SIZE             256 

#define APP_TASK_TEST_STK_SIZE                 512
#define  APP_TASK_LED_STK_SIZE                 128

#define APP_TASK_MP3_STK_SIZE			       256

//#define  APP_TASK_START_STK_SIZE             256 
//#define  APP_TASK_SHELL_STK_SIZE			   256
//#define  APP_TASK_USER_IF_STK_SIZE           256 
//#define  APP_TASK_KBD_STK_SIZE               256 
//#define  APP_TASK_PROBE_STR_STK_SIZE         256 
 
//#define  OS_PROBE_TASK_STK_SIZE              256 
 
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
