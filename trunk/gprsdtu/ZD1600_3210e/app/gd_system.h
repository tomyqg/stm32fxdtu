
#ifndef GD_SYSTEM_MANAGE_H
#define GD_SYSTEM_MANAGE_H

#include "gd_config.h"
#include "gd_suart.h"
#include "gd_guart.h"
#include "gd_network.h"
#include "gd_mem.h"

#define SUART		COM1
#define GUART 		COM2

#define GD_CONFIG_MODE		1
#define GD_TRANS_MODE		2

#define GD_MSG_COUNT		64
#define GD_MSG_SIZE			8

#define GD_DEVID	"gprsdtu"

extern const INT8U GD_DEVMAC[];

enum GD_TASK_ID
{
	GD_TASK_INIT_ID = 0,
	GD_TASK_CONFIG_ID,
	GD_TASK_GUART_RX_ID,
	GD_TASK_SUART_ID,
	GD_TASK_GUART_ID,
	GD_TASK_NETWORK_ID,
	GD_TASK_LED_ID,
	APP_TASK_TEST_ID,
};

enum _GD_MSG_TYPE_
{
	GD_MSG_CONNECTION_READY = 0,
	GD_MSG_FRAME_READY,
	GD_MSG_RES_FRAME_READY,
	GD_MSG_GM_RECV_DATA,
	GD_MSG_GM_TCP_STATE_CHANGE,
};

typedef struct _GD_MSG_
{
	int  		type;
	void    	*data;
}gd_msg_t;

typedef struct _GD_SYSTEM_
{
	int					work_mode;
	
	OS_MEM 				*gd_msg_PartitionPtr;

	OS_MEM 				*sp2gm_buf_PartitionPtr;
	OS_MEM 				*gm2sp_buf_PartitionPtr;

	OS_EVENT			*gm_operate_sem;

	gd_frame_list_t 	sp2gm_frame_list;
	gd_frame_list_t 	gm2sp_frame_list;
	
	gd_config_task_t	config_task;

  	gd_suart_task_t	 	suart_task;
	
	gd_guart_task_t	 	guart_task;

	gd_network_task_t	network_task; 

}gd_system_t;

extern gd_system_t gd_system; 


int gd_judge_work_mode(void);
int  gd_system_init(void);
int gd_start_init_task(void);
















#endif

