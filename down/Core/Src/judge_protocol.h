#ifndef __JUDGE_PROTOCOL_H_
#define __JUDGE_PROTOCOL_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
/* Exported macro ------------------------------------------------------------*/
#define INTERACT_DATA_LEN	10 /* �����˼佻�����ݶγ���*/
#define Robot_Hurt_ID0 (judge_info.RobotHurt.armor_id==1) /*����װ��ID1*/
#define Robot_Hurt_ID1 (judge_info.RobotHurt.armor_id==3) /*����װ��ID3*/
/* Exported types ------------------------------------------------------------*/

//ң����
/** ��ң������Ϣͼ��
 * ����������������������������������������������������������������������������������������
 * �� ����������1  660                   660   ����������1 ��
 * �� ��S 1��3   ��                      ��   ��S 2��3 ��
 * �� ����������2   ��                      ��   ����������2 ��
 * ��          ��                      ��          ��
 * �� CH2<�����������੤��������>660    CH0<�����������੤��������>660 ��
 * ��          ��                      ��          ��
 * ��          ��                      ��          ��
 * ��          V                      V            
 * ��         CH3                    CH1           
 * ����������������������������������������������������������������������������������������
 */
 
/* ң����ģʽ */
typedef enum
{
 RC_SENSOR_MODE=1,
 RC_STOP=3,
 KEYMOUSE_MODE=2,

}rc_keymouse_mode;

/* ң�����ṹ�� */
typedef  struct
{
	struct
{
 int16_t ch0;
 int16_t ch1;
 int16_t ch2;
 int16_t ch3; 
 uint8_t s1;
 uint8_t s2;
}rc;
struct
{
  int16_t x;
	int16_t y;
	int16_t z;
	uint8_t press_1;
	uint8_t press_r;
}mouse;

struct
{
  uint16_t v;
}key;
}rc_sensor_info_t;

/* ֡�ֽ�ƫ�� */
typedef enum {
    FRAME_HEADER1	= 0,
    CMD_ID			= 5,
    DATA_SEG		= 7
} Judge_Frame_Offset_t;

/* ֡ͷ�ֽ�ƫ�� */
typedef enum {
    J_SOF			= 0,
    DATA_LENGTH	= 1,
} Judge_Frame_Header_Offset_t;

typedef enum {  
	  ID_GAME_STATUS 					    = 0x0001,	// ����״̬
	  ID_GAME_RESULT 					    = 0x0002,	// �������
    ID_GAME_ROBOT_HP 				      = 0x0003,	// ������Ѫ������
    ID_EVENT_DATA 					      = 0x0101,	// �����¼�����
	  ID_SUPPLY_PROJECTILE_ACTION 	= 0x0102,	// ����վ������ʶ
    ID_GAME_ROBOT_STATUS 			    = 0x0201,	// ����������״̬
    ID_POWER_HEAT_DATA 				    = 0x0202,	// ʵʱ������������
	  ID_GAME_ROBOT_POS				    = 0x0203,	// ������λ��
    ID_BUFF							        = 0x0204,	// ����������
    ID_ROBOT_HURT					      = 0x0206,	// �������˺�״̬
    ID_SHOOT_DATA					      = 0x0207,	// ʵʱ�����Ϣ
    ID_BULLET_REMAINING				    = 0x0208,	// �ӵ�ʣ�෢����
	  ID_RFID_STATUS					      = 0x0209,	// ������RFID״̬
	  ID_COMMUNICATION				      = 0x0301,	// �����˼佻������(���ͷ���������)
    ID_COMMAND						        = 0x0303,	// �����˼佻������(���ͷ���������)	
} Judge_Cmd_ID_t;
typedef enum {
    /* Std */
    LEN_FRAME_HEAD 	= 5,	// ֡ͷ����
    LEN_CMD_ID 		= 2,	// �����볤��
    LEN_FRAME_TAIL 	= 2,	// ֡βCRC16
    /* Ext */
    // 0x000x
    LEN_GAME_STATUS 				= 11,
    LEN_GAME_RESULT 				= 1,
    LEN_GAME_ROBOT_HP 				= 32,
    LEN_DART_STATUS					= 3,
    LEN_ICRA_BUFF_DEBUFF_ZONE_STATUS= 11,

    // 0x010x
    LEN_EVENT_DATA					= 4,
    LEN_SUPPLY_PROJECTILE_ACTION	= 4,
    LEN_REFEREE_WARNING				= 2,
    LEN_DART_REMAINING_TIME			= 1,

    // 0x020x
    LEN_AERAL_DATA			        = 8,
    LEN_GAME_ROBOT_STATUS			= 27,
    LEN_POWER_HEAT_DATA 			= 16,
    LEN_GAME_ROBOT_POS				= 16,
    LEN_BUFF		 				= 1,
    LEN_AERIAL_ROBOT_ENERGY 		= 1,
    LEN_ROBOT_HURT					= 1,
    LEN_SHOOT_DATA					= 7,
    LEN_BULLET_REMAINING	 		= 6,
    LEN_RFID_STATUS					= 4,
} Judge_Data_Length_t;
/* Exported types ------------------------------------------------------------*/
/* �Զ���֡ͷ Byte: 5 */
typedef __packed struct
{
    uint8_t  sof;
    uint16_t data_length;
    uint8_t  seq;
    uint8_t  crc8;
} std_frame_header_t;


/* ID: 0x0001	Byte: 	11	����״̬���� */
typedef __packed struct
{
    uint8_t game_type : 4;			// ��������
    uint8_t game_progress : 4;		// �����׶�
    uint16_t stage_remain_time;		// ��ǰ�׶�ʣ��ʱ��(��λ:s)
    uint64_t SyncTimeStamp;       //�����˽��յ���ָ��ľ�ȷ Unix ʱ�䣬�����ض��յ���Ч�� NTP ��������ʱ����Ч
} ext_game_status_t;


/* ID: 0x0002	Byte:	1	����������� */
typedef __packed struct
{
    uint8_t winner;
} ext_game_result_t;


/* ID: 0x0003	Byte:	36	������Ѫ���������� */
typedef __packed struct
{
    uint16_t red_1_robot_HP;	// ��1Ӣ�ۻ�����Ѫ��(δ�ϳ�������Ѫ��Ϊ0)
    uint16_t red_2_robot_HP;	// ��2���̻�����Ѫ��
    uint16_t red_3_robot_HP;	// ��3����������Ѫ��
    uint16_t red_4_robot_HP;	// ��4����������Ѫ��
    uint16_t red_5_robot_HP;	// ��5����������Ѫ��
    uint16_t red_7_robot_HP;	// ��7�ڱ�������Ѫ��
    uint16_t red_outpost_HP;	// �췽ǰ��վѪ��
    uint16_t red_base_HP;		// �췽����Ѫ��
    uint16_t blue_1_robot_HP;	// ��1Ӣ�ۻ�����Ѫ��
    uint16_t blue_2_robot_HP;	// ��2���̻�����Ѫ��
    uint16_t blue_3_robot_HP;	// ��3����������Ѫ��
    uint16_t blue_4_robot_HP;	// ��4����������Ѫ��
    uint16_t blue_5_robot_HP;	// ��5����������Ѫ��
    uint16_t blue_7_robot_HP;	// ��7�ڱ�������Ѫ��
    uint16_t blue_outpost_HP;	// ����ǰ��վѪ��
    uint16_t blue_base_HP;		// ��������Ѫ��
} ext_game_robot_HP_t;



/* ID: 0x0101  Byte:  4    �����¼����� */
typedef __packed struct
{
   uint32_t Event_data;
} ext_event_data_t;


/* ID: 0x0102  Byte:  4    ����վ������ʶ���� */
typedef __packed struct
{
    uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
    uint8_t supply_projectile_step;
    uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

/* ID: 0X0201  Byte: 18    ������״̬���� */
typedef __packed struct
{
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t remain_HP;
    uint16_t max_HP;
    uint16_t shooter_id1_17mm_cooling_rate;
    uint16_t shooter_id1_17mm_cooling_limit;
    uint16_t shooter_id1_17mm_speed_limit;
    uint16_t shooter_id2_17mm_cooling_rate;
    uint16_t shooter_id2_17mm_cooling_limit;
    uint16_t shooter_id2_17mm_speed_limit;
    uint16_t shooter_id1_42mm_cooling_rate;
    uint16_t shooter_id1_42mm_cooling_limit;
    uint16_t shooter_id1_42mm_speed_limit;
    uint16_t chassis_power_limit;
    uint8_t mains_power_gimbal_output : 1;
    uint8_t mains_power_chassis_output : 1;
    uint8_t mains_power_shooter_output : 1;
} ext_game_robot_status_t;


/* ID: 0X0202  Byte: 16    ʵʱ������������ */
typedef __packed struct
{
    uint16_t chassis_volt;   		// ���������ѹ����λ��mV
    uint16_t chassis_current;		// ���������������λ��mA
    float chassis_power;   			// ˲ʱ���ʣ���λ��W
    uint16_t chassis_power_buffer;	// ���̹��ʻ���
    uint16_t  shooter_id1_17mm_cooling_heat;		// 17mm ǹ������
    uint16_t shooter_id2_17mm_cooling_heat;	// ����17mm ǹ������
    uint16_t shooter_id1_42mm_cooling_heat;	//  42mm ǹ������
} ext_power_heat_data_t;

/* ID: 0x0203  Byte: 16    ������λ������ */
typedef __packed struct
{
    float x;
    float y;
    float z;
    float yaw;
} ext_game_robot_pos_t;


/* ID: 0x0204  Byte:  1    �������������� */
typedef __packed struct
{
    uint8_t power_rune_buff;
} ext_buff_t;

/* ID: 0x0206  Byte:  1    �˺�״̬���� */
typedef __packed struct
{
    uint8_t armor_id : 4; 	// װ���˺�ʱ����װ��ID
    uint8_t hurt_type : 4; 	// 0x0װ���˺� 0x1ģ����� 0x2������ 0x3������ 0x4������ 0x5ײ��
} ext_robot_hurt_t;


/* ID: 0x0207  Byte:  7    ʵʱ������� */
typedef __packed struct
{
    uint8_t bullet_type; 	// �ӵ�����(1-17mm, 2-42mm)
    uint8_t shooter_id;
    uint8_t bullet_freq;  	// �ӵ���Ƶ(Hz)
    float bullet_speed;		// �ӵ�����(m/s)
} ext_shoot_data_t;


/* ID: 0x0208  Byte:  6    �ӵ�ʣ�෢�������� */
typedef __packed struct
{
    uint16_t bullet_remaining_num_17mm;
    uint16_t bullet_remaining_num_42mm;
    uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

/* ID: 0x0209  Byte:  4 	������RFID״̬ */
typedef __packed struct
{
    uint32_t rfid_status;
} ext_rfid_status_t;

/* ID: 0x301  Byte:  12 	��̨�ֽ��� */
typedef __packed struct
{
    uint16_t cmd_id;
    uint16_t send_id;
    uint16_t receive_id;
} ext_aerial_data_t;

/* ID: 0x0303  Byte:  15 	 */
typedef __packed struct
{
    float target_x;
    float target_y;
    float target_z;
    uint8_t commd_keyboard;
    uint16_t target_ID;
} ext_robot_command_t;

typedef struct
{
    uint16_t frame_length;	// ��֡����(����ʱʹ��)
    uint16_t cmd_id;		// ������(����ʱʹ��)
    uint16_t err_cnt;		// ��֡��(����ʱʹ��)
    bool	 data_valid;	// ������Ч��
    uint16_t self_client_id;// �����߻����˶�Ӧ�Ŀͻ���ID
    bool	power_heat_update;	// �����������ݸ���
    bool	shoot_update;		// ������ݸ���
    bool	 hurt_data_update;	// �˺����ݸ���
    bool	dart_data_update;	// �������ݸ���
    bool	supply_data_update;	// ����վ���ݸ���
	bool    command_data_update;//С��ͼ���ݸ���
	bool	communication_data_update;//��̨�����ݸ���
    std_frame_header_t				      FrameHeader;				      // ֡ͷ��Ϣ
    ext_game_status_t 				      GameStatus;					    // 0x0001
    ext_game_result_t 				      GameResult;				      // 0x0002
    ext_game_robot_HP_t 			      GameRobotHP;		 		      // 0x0003
    ext_event_data_t				        EventData;					      // 0x0101
	  ext_supply_projectile_action_t	SupplyProjectileAction;		// 0x0102
    ext_game_robot_status_t			    GameRobotStatus;			    // 0x0201
    ext_power_heat_data_t			      PowerHeatData;				    // 0x0202
	  ext_game_robot_pos_t			      GameRobotPos;				    // 0x0203
		ext_buff_t						          Buff;						        // 0x0204
    ext_robot_hurt_t				        RobotHurt;					      // 0x0206
    ext_shoot_data_t				        ShootData;					      // 0x0207
    ext_bullet_remaining_t			    BulletRemaining;			    // 0x0208
		ext_rfid_status_t				      RfidStatus;					    // 0x0209
		ext_aerial_data_t             AerialData;             // 0x301	
    ext_robot_command_t             command;                // 0x0303
		uint16_t                        self_client;        //�����ͻ���
    int16_t		offline_cnt;
    int16_t		offline_max_cnt;

} judge_info_t;
/* Exported functions --------------------------------------------------------*/
void judge_update(uint8_t *rxBuf);

#endif
