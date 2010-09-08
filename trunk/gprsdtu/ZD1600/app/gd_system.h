
#ifndef GD_SYSTEM_MANAGE_H
#define GD_SYSTEM_MANAGE_H

#include "gd_config.h"
#include "gd_suart.h"
#include "gd_guart.h"
#include "gd_network.h"

#define GD_CONFIG_MODE		1
#define GD_TRANS_MODE		2

enum GD_TASK_ID
{
	GD_TASK_INIT_ID = 0,
	GD_TASK_CONFIG_ID,
	GD_TASK_SUART_ID,
	GD_TASK_GUART_ID,
	GD_TASK_NETWORK_ID,
	GD_TASK_LED_ID,
	APP_TASK_TEST_ID
};

typedef struct _GD_SYSTEM_
{
	int					work_mode;
	
	gd_config_task_t	config_task;

  	gd_suart_task_t	 	suart_task;
	
	gd_guart_task_t	 	guart_task;

	gd_network_task_t	network_task; 

}gd_system_t;


int gd_judge_work_mode(void);
int  gd_system_init(void);
void gd_start_tasks(void);

















#endif

