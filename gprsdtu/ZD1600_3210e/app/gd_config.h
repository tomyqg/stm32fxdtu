
#ifndef GD_CONFIG_H
#define GD_CONFIG_H

#include <ucos_ii.h>

typedef struct GD_CONFIG_INFO
{
	int		type;
}gd_config_info_t;

typedef struct GD_CONFIG_TASK
{	 
	INT8U 		task_id;
	INT8U 		prio;

}gd_config_task_t;


int gd_get_config(void);







#endif

