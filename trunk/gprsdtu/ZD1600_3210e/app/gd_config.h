
#ifndef GD_CONFIG_H
#define GD_CONFIG_H

#include <ucos_ii.h>
#include "gd_mib.h"

#define GD_DEV_ID_LEN 			8
#define GD_SIM_ID_LEN			12
#define GD_WORK_MODE_LEN		4
#define GD_ACTI_TYPE_LEN		5
#define GD_SERIAL_MODE_LEN		3
#define GD_BAUD_RATE_LEN		6
#define GD_RETURN_MAIN_LEN		1
#define GD_DATA_INT_LEN			4
#define GD_SVR_CNT_LEN			1
#define GD_SVR_IP_LEN			16
#define GD_SVR_PORT_LEN			5
#define GD_BKP_SVR_IP_LEN		16
#define GD_BKP_SVR_PORT_LEN		5
#define GD_SVR1_IP_LEN			16
#define GD_SVR1_PORT_LEN		5
#define GD_SVR2_IP_LEN			16
#define GD_SVR2_PORT_LEN		5
#define GD_DNS_SVR_LEN			17
#define GD_BKP_DNS_SVR_LEN		16
#define GD_APN_LEN				6
#define GD_APN_USER_LEN			11
#define GD_APN_PWD_LEN			11
#define GD_APN_CENTER_LEN		11
#define GD_SMSC_LEN				15
#define GD_POLL_INT_LEN			2
#define GD_WAKE_PHONE_LEN		9
#define GD_WAKE_SMS_LEN			11

typedef struct	_GD_DEV_ID_
{
	const char *oid;
	char value[GD_DEV_ID_LEN];
}gd_dev_id_t;

typedef struct	_GD_SIM_ID_
{
	const char *oid;
	char value[GD_SIM_ID_LEN];
}gd_sim_id_t;

enum gd_work_mode_value_t
{
	GD_WORK_MODE_PROT = 0,
	GD_WORK_MODE_TTRN ,
	GD_WORK_MODE_TUDP ,
};
typedef struct	_GD_WORK_MODE_
{
	const char *oid;
	char value;
}gd_work_mode_t;

enum gd_acti_type_value_t
{
	GD_ACTI_TYPE_AUTO = 0,
	GD_ACTI_TYPE_SMS ,
	GD_ACTI_TYPE_PHONE,
	GD_ACTI_TYPE_DATA,
	GD_ACTI_TYPE_MIX,
};
typedef struct	_GD_ACTI_TYPE_
{
	const char *oid;
	char value;
}gd_acti_type_t;

enum gd_serial_mode_value_t
{
	GD_SERIAL_MODE_8N1 = 0,
	GD_SERIAL_MODE_8O1 = 1,
	GD_SERIAL_MODE_8E1 = 2,

};
typedef struct	_GD_SERIAL_MODE_
{
	const char *oid;
	char value;
}gd_serial_mode_t;

typedef struct	_GD_BAUD_RATE_
{
	const char *oid;
	int value;
}gd_baud_rate_t;

enum gd_return_main_value_t
{
	GD_RETURN_MAIN_NO  = 0,
	GD_RETURN_MAIN_YES  ,
};
typedef struct _GD_RETURN_MAIN_
{
	const char *oid;
	char value; 
}gd_return_main_t;

typedef struct _GD_DATA_INT_
{
	const char *oid;
	int value; 
}gd_data_int_t;

typedef struct _GD_SVR_CNT_
{
	const char *oid;
	char value; 
}gd_svr_cnt_t;

typedef struct _GD_SVR_IP_
{
	const char *oid;
	char value[GD_SVR_IP_LEN]; 
}gd_svr_ip_t;

typedef struct _GD_SVR_PORT_
{
	const char *oid;
	int value; 
}gd_svr_port_t;

typedef struct _GD_BKP_SVR_IP_
{
	const char *oid;
	char value[GD_BKP_SVR_IP_LEN]; 	
}gd_bkp_svr_ip_t;

typedef struct _GD_BKP_SVR_PORT_
{
	const char *oid;
	int value; 	
}gd_bkp_svr_port_t;

typedef struct _GD_SVR1_IP_
{
	const char *oid;
	char value[GD_SVR1_IP_LEN]; 
}gd_svr1_ip_t;

typedef struct _GD_SVR1_PORT_
{
	const char *oid;
	int value; 
}gd_svr1_port_t;

typedef struct _GD_SVR2_IP_
{
	const char *oid;
	char value[GD_SVR2_IP_LEN]; 
}gd_svr2_ip_t;

typedef struct _GD_SVR2_PORT_
{
	const char *oid;
	int value; 
}gd_svr2_port_t;

typedef struct _GD_DNS_SVR_
{
	const char *oid;
	char value[GD_DNS_SVR_LEN]; 
}gd_dns_svr_t;

typedef struct _GD_BKP_DNS_SVR_
{
	const char *oid;
	char value[GD_BKP_DNS_SVR_LEN]; 
}gd_bkp_dns_svr_t;

typedef struct _GD_APN_
{
	const char *oid;
	char value[GD_APN_LEN]; 
}gd_apn_t;

typedef struct _GD_APN_USER_
{
	const char *oid;
	char value[GD_APN_USER_LEN]; 
}gd_apn_user_t;

typedef struct _GD_APN_PWD_
{
	const char *oid;
	char value[GD_APN_PWD_LEN]; 
}gd_apn_pwd_t;

typedef struct _GD_APN_CENTER_
{
	const char *oid;
	char value[GD_APN_CENTER_LEN]; 
}gd_apn_center_t;

typedef struct _GD_SMSC_
{
	const char *oid;
	char value[GD_SMSC_LEN]; 
}gd_smsc_t;

typedef struct _GD_POLL_INT_
{
	const char *oid;
	int value; 
}gd_poll_int_t;

typedef struct _GD_WAKE_PHONE_
{
	const char *oid;
	char value[GD_WAKE_PHONE_LEN]; 
}gd_wake_phone_t;

typedef struct _GD_WAKE_SMS_
{
	const char *oid;
	char value[GD_WAKE_SMS_LEN]; 
}gd_wake_sms_t;

typedef struct GD_CONFIG_INFO
{
	gd_dev_id_t			gd_dev_id;
	gd_sim_id_t			gd_sim_id;
	gd_work_mode_t		gd_work_mode;
	gd_acti_type_t		gd_acti_type;
	gd_serial_mode_t 	gd_serial_mode;
	gd_baud_rate_t		gd_baud_rate;
	gd_return_main_t	gd_return_main;
	gd_data_int_t		gd_data_int;
	gd_svr_cnt_t		gd_svr_cnt;
	gd_svr_ip_t			gd_svr_ip;
	gd_svr_port_t		gd_svr_port;
	gd_bkp_svr_ip_t		gd_bkp_svr_ip;
	gd_bkp_svr_port_t	gd_bkp_svr_port;
	gd_svr1_ip_t		gd_svr1_ip;
	gd_svr1_port_t		gd_svr1_port;
	gd_svr2_ip_t		gd_svr2_ip;
	gd_svr2_port_t		gd_svr2_port;
	gd_dns_svr_t		gd_dns_svr;
	gd_bkp_dns_svr_t	gd_bkp_dns_svr;
	gd_apn_t			gd_apn;
	gd_apn_user_t		gd_apn_user;
	gd_apn_pwd_t		gd_apn_pwd;
	gd_apn_center_t		gd_apn_center;
	gd_smsc_t			gd_smsc;
	gd_poll_int_t		gd_poll_int;
    gd_wake_phone_t		gd_wake_phone;
	gd_wake_sms_t		gd_wake_sms;

}gd_config_info_t;

typedef struct GD_CONFIG_TASK
{	 
	INT8U 		task_id;
	INT8U 		prio;

}gd_config_task_t;


INT32U gd_get_config(void);







#endif

