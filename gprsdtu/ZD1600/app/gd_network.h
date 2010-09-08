#ifndef GD_NETWORK_MANAGE_H
#define GD_NETWORK_MANAGE_H

#include "ucos_ii.h"

#define LINK_TYPE_TCP		0
#define LINK_TYPE_UDP   	1

#define GD_MAX_LINK			3

#define NETWORK_QMSG_COUNT 	3

typedef struct GD_LINK
{		
	int 	link_num;
	int 	link_type; //0:TCP 1:UDP
  	int     link_state;	
	char	svr_ip[16];
	int		svr_port;
}gd_link_t;

typedef struct GD_NETWORK_TASK
{
	INT8U 		task_id;
	INT8U 		prio;
	OS_EVENT 	*q_network;
	void 		*QMsgTbl[NETWORK_QMSG_COUNT];
	int			link_count;
	int 		link_index;
	gd_link_t	link_info[GD_MAX_LINK];

}gd_network_task_t;







#endif

