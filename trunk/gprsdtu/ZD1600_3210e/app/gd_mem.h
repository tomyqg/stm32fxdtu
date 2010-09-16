#ifndef GD_MEM_MANAGE_H
#define GD_MEM_MANAGE_H

#include <ucos_ii.h>

#define SP2GM_BUF_LEN		8*1024
#define GM2SP_BUF_LEN		4*1024

#define SP2GM_FRAME_NODE_COUNT	512
#define GM2SP_FRAME_NODE_COUNT	32


#define GD_FRAME_NODE_SIZE		12

typedef struct GD_FRAME_NODE
{
	char 				 *pFrame;
	int  				 len;

	struct GD_FRAME_NODE *next;
}frame_node_t;

typedef struct GD_FRAME_LIST
{
	frame_node_t *head;

	int 		 frame_count;
}gd_frame_list_t;
















#endif

