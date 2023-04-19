#ifndef __JUDGE_PROTOCOL_H_
#define __JUDGE_PROTOCOL_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
/* Exported macro ------------------------------------------------------------*/
#define INTERACT_DATA_LEN	10 /* 机器人间交互数据段长度*/
#define Robot_Hurt_ID0 (judge_info.RobotHurt.armor_id==1) /*受伤装甲ID1*/
#define Robot_Hurt_ID1 (judge_info.RobotHurt.armor_id==3) /*受伤装甲ID3*/
/* Exported types ------------------------------------------------------------*/

//遥控器
/** 《遥控器信息图》
 * ┌──────────────────────────────────────────┐
 * │ ┌───┐1  660                   660   ┌───┐1 │
 * │ │S 1│3   Λ                      Λ   │S 2│3 │
 * │ └───┘2   │                      │   └───┘2 │
 * │          │                      │          │
 * │ CH2<─────┼─────>660    CH0<─────┼─────>660 │
 * │          │                      │          │
 * │          │                      │          │
 * │          V                      V            
 * │         CH3                    CH1           
 * └──────────────────────────────────────────┘
 */
 
/* 遥控器模式 */
typedef enum
{
 RC_SENSOR_MODE=1,
 RC_STOP=3,
 KEYMOUSE_MODE=2,

}rc_keymouse_mode;

/* 遥控器结构体 */
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

/* 帧字节偏移 */
typedef enum {
    FRAME_HEADER1	= 0,
    CMD_ID			= 5,
    DATA_SEG		= 7
} Judge_Frame_Offset_t;

/* 帧头字节偏移 */
typedef enum {
    J_SOF			= 0,
    DATA_LENGTH	= 1,
} Judge_Frame_Header_Offset_t;

typedef enum {  
	  ID_GAME_STATUS 					    = 0x0001,	// 比赛状态
	  ID_GAME_RESULT 					    = 0x0002,	// 比赛结果
    ID_GAME_ROBOT_HP 				      = 0x0003,	// 机器人血量数据
    ID_EVENT_DATA 					      = 0x0101,	// 场地事件数据
	  ID_SUPPLY_PROJECTILE_ACTION 	= 0x0102,	// 补给站动作标识
    ID_GAME_ROBOT_STATUS 			    = 0x0201,	// 比赛机器人状态
    ID_POWER_HEAT_DATA 				    = 0x0202,	// 实时功率热量数据
	  ID_GAME_ROBOT_POS				    = 0x0203,	// 机器人位置
    ID_BUFF							        = 0x0204,	// 机器人增益
    ID_ROBOT_HURT					      = 0x0206,	// 机器人伤害状态
    ID_SHOOT_DATA					      = 0x0207,	// 实时射击信息
    ID_BULLET_REMAINING				    = 0x0208,	// 子弹剩余发射数
	  ID_RFID_STATUS					      = 0x0209,	// 机器人RFID状态
	  ID_COMMUNICATION				      = 0x0301,	// 机器人间交互数据(发送方触发发送)
    ID_COMMAND						        = 0x0303,	// 机器人间交互数据(发送方触发发送)	
} Judge_Cmd_ID_t;
typedef enum {
    /* Std */
    LEN_FRAME_HEAD 	= 5,	// 帧头长度
    LEN_CMD_ID 		= 2,	// 命令码长度
    LEN_FRAME_TAIL 	= 2,	// 帧尾CRC16
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
/* 自定义帧头 Byte: 5 */
typedef __packed struct
{
    uint8_t  sof;
    uint16_t data_length;
    uint8_t  seq;
    uint8_t  crc8;
} std_frame_header_t;


/* ID: 0x0001	Byte: 	11	比赛状态数据 */
typedef __packed struct
{
    uint8_t game_type : 4;			// 比赛类型
    uint8_t game_progress : 4;		// 比赛阶段
    uint16_t stage_remain_time;		// 当前阶段剩余时间(单位:s)
    uint64_t SyncTimeStamp;       //机器人接收到该指令的精确 Unix 时间，当机载端收到有效的 NTP 服务器授时后生效
} ext_game_status_t;


/* ID: 0x0002	Byte:	1	比赛结果数据 */
typedef __packed struct
{
    uint8_t winner;
} ext_game_result_t;


/* ID: 0x0003	Byte:	36	机器人血量数据数据 */
typedef __packed struct
{
    uint16_t red_1_robot_HP;	// 红1英雄机器人血量(未上场及罚下血量为0)
    uint16_t red_2_robot_HP;	// 红2工程机器人血量
    uint16_t red_3_robot_HP;	// 红3步兵机器人血量
    uint16_t red_4_robot_HP;	// 红4步兵机器人血量
    uint16_t red_5_robot_HP;	// 红5步兵机器人血量
    uint16_t red_7_robot_HP;	// 红7哨兵机器人血量
    uint16_t red_outpost_HP;	// 红方前哨站血量
    uint16_t red_base_HP;		// 红方基地血量
    uint16_t blue_1_robot_HP;	// 蓝1英雄机器人血量
    uint16_t blue_2_robot_HP;	// 蓝2工程机器人血量
    uint16_t blue_3_robot_HP;	// 蓝3步兵机器人血量
    uint16_t blue_4_robot_HP;	// 蓝4步兵机器人血量
    uint16_t blue_5_robot_HP;	// 蓝5步兵机器人血量
    uint16_t blue_7_robot_HP;	// 蓝7哨兵机器人血量
    uint16_t blue_outpost_HP;	// 蓝方前哨站血量
    uint16_t blue_base_HP;		// 蓝方基地血量
} ext_game_robot_HP_t;



/* ID: 0x0101  Byte:  4    场地事件数据 */
typedef __packed struct
{
   uint32_t Event_data;
} ext_event_data_t;


/* ID: 0x0102  Byte:  4    补给站动作标识数据 */
typedef __packed struct
{
    uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
    uint8_t supply_projectile_step;
    uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

/* ID: 0X0201  Byte: 18    机器人状态数据 */
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


/* ID: 0X0202  Byte: 16    实时功率热量数据 */
typedef __packed struct
{
    uint16_t chassis_volt;   		// 底盘输出电压，单位：mV
    uint16_t chassis_current;		// 底盘输出电流，单位：mA
    float chassis_power;   			// 瞬时功率，单位：W
    uint16_t chassis_power_buffer;	// 底盘功率缓冲
    uint16_t  shooter_id1_17mm_cooling_heat;		// 17mm 枪口热量
    uint16_t shooter_id2_17mm_cooling_heat;	// 机动17mm 枪口热量
    uint16_t shooter_id1_42mm_cooling_heat;	//  42mm 枪口热量
} ext_power_heat_data_t;

/* ID: 0x0203  Byte: 16    机器人位置数据 */
typedef __packed struct
{
    float x;
    float y;
    float z;
    float yaw;
} ext_game_robot_pos_t;


/* ID: 0x0204  Byte:  1    机器人增益数据 */
typedef __packed struct
{
    uint8_t power_rune_buff;
} ext_buff_t;

/* ID: 0x0206  Byte:  1    伤害状态数据 */
typedef __packed struct
{
    uint8_t armor_id : 4; 	// 装甲伤害时代表装甲ID
    uint8_t hurt_type : 4; 	// 0x0装甲伤害 0x1模块掉线 0x2超射速 0x3超热量 0x4超功率 0x5撞击
} ext_robot_hurt_t;


/* ID: 0x0207  Byte:  7    实时射击数据 */
typedef __packed struct
{
    uint8_t bullet_type; 	// 子弹类型(1-17mm, 2-42mm)
    uint8_t shooter_id;
    uint8_t bullet_freq;  	// 子弹射频(Hz)
    float bullet_speed;		// 子弹射速(m/s)
} ext_shoot_data_t;


/* ID: 0x0208  Byte:  6    子弹剩余发射数数据 */
typedef __packed struct
{
    uint16_t bullet_remaining_num_17mm;
    uint16_t bullet_remaining_num_42mm;
    uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

/* ID: 0x0209  Byte:  4 	机器人RFID状态 */
typedef __packed struct
{
    uint32_t rfid_status;
} ext_rfid_status_t;

/* ID: 0x301  Byte:  12 	云台手交互 */
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
    uint16_t frame_length;	// 整帧长度(调试时使用)
    uint16_t cmd_id;		// 命令码(调试时使用)
    uint16_t err_cnt;		// 错帧数(调试时使用)
    bool	 data_valid;	// 数据有效性
    uint16_t self_client_id;// 发送者机器人对应的客户端ID
    bool	power_heat_update;	// 功率热量数据更新
    bool	shoot_update;		// 射击数据更新
    bool	 hurt_data_update;	// 伤害数据更新
    bool	dart_data_update;	// 飞镖数据更新
    bool	supply_data_update;	// 补给站数据更新
	bool    command_data_update;//小地图数据更新
	bool	communication_data_update;//云台手数据更新
    std_frame_header_t				      FrameHeader;				      // 帧头信息
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
		uint16_t                        self_client;        //本机客户端
    int16_t		offline_cnt;
    int16_t		offline_max_cnt;

} judge_info_t;
/* Exported functions --------------------------------------------------------*/
void judge_update(uint8_t *rxBuf);

#endif
