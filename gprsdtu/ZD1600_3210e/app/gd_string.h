#ifndef	__MYSTRING_H__
#define	__MYSTRING_H__
#include "stm32f10x_type.h"


#define GD_FRAME_HEAD_FLG	"WS-120M"
/*ws-120Э����������*/
#define WS_120M_HEARTBEAT	0
#define WS_120M_FRAME_DATA	1


/*�ַ�������*/
u32 char_to_int(u8 *cdata, u32 len);
u8* check_string(u8 *str, u8 *flg, u32 len);
u32 ascii_to_hex(u8 *O_data, u8 *N_data, u32 len);
u32 hex_to_ascii(u8 *data, u8 *buffer, u32 len);








//�������ͣ�0���� 1��ҵ����
typedef struct __gd_frame_type
{
	u8 *dev_id;			//	dev_id	
	u8 packet_sum;		//	packet_sum  ������
	u8 packet_index;		//	packet_index 	�����
	u8 *packet_data;		//	packet_data	 ������
	u16 packet_len;		//	packet_len	�����ݳ���
	u16 frame_index;	//�frame_index  ֡���

	u8 *data;				//	ws120mЭ�����ݰ�
	u32 len;				//	ws120mЭ�����ݰ�����
}gd_frame_t;

u16 check_sum_16(u8 *addr, u16 count);
u32	gd_heart_beat_init(uc8 *dev_id, uc8 *dev_mac, u8 *extra_data, u8 extra_len, u8 wifi_signal, u8 *buf);
u32	gd_data_init(uc8 *dev_id, u8 data_type, u8 *data, u16 data_len, u8 *buf);
//u32	gd_gm_frame_data_init(uc8 *dev_id, u8 packet_sum, u8 packet_index, u8 *packet_data, u16 packet_len, u16 frame_index,u8 *buf);
void	gd_frame_data_init(gd_frame_t *frame);
u8	gd_frame_data_resolve(gd_frame_t *frame);

#endif


