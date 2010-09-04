#ifndef __MONITOR_H__
#define __MONITOR_H__

extern OS_STK App_TaskMonitorStk[APP_TASK_MONITOR_STK_SIZE];
void App_TaskMonitor(void *parg);

#endif
