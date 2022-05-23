#ifndef __REFEREE_H__
#define __REFEREE_H__

#include "crc.h"
#include "core_cm4.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#define JUDGE_LENGTH 2048
#define JUDGE_HEAD_LENGTH 5
#define JUDGE_CMD_LENGTH 2
#define JUDGE_CRC16_LENGTH 2

typedef uint8_t  u8;

typedef __packed struct//帧头结构体
{
  uint8_t  sof;
  uint16_t data_length;
  uint8_t  seq;
  uint8_t  crc8;
} frame_header_t;

typedef __packed struct
{
uint8_t bullet_type;
uint8_t shooter_id;
uint8_t bullet_freq;
float bullet_speed;
} ext_shoot_data_t;

typedef __packed struct
{
uint16_t red_1_robot_HP;
uint16_t red_2_robot_HP;
uint16_t red_3_robot_HP;
uint16_t red_4_robot_HP;
uint16_t red_5_robot_HP;
uint16_t red_7_robot_HP;
uint16_t red_outpost_HP;
uint16_t red_base_HP;
uint16_t blue_1_robot_HP;
uint16_t blue_2_robot_HP;
uint16_t blue_3_robot_HP;
uint16_t blue_4_robot_HP;
uint16_t blue_5_robot_HP;
uint16_t blue_7_robot_HP;
uint16_t blue_outpost_HP;
uint16_t blue_base_HP;
} ext_game_robot_HP_t;

typedef __packed struct
{
uint16_t chassis_volt;
uint16_t chassis_current;
float chassis_power;//底盘输出功率 单位 W 瓦
uint16_t chassis_power_buffer;
uint16_t shooter_id1_17mm_cooling_heat;
uint16_t shooter_id2_17mm_cooling_heat;
uint16_t shooter_id1_42mm_cooling_heat;//42mm 枪口热量
} ext_power_heat_data_t;

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
uint16_t shooter_id1_42mm_cooling_rate;//机器人 42mm 枪口每秒冷却值
uint16_t shooter_id1_42mm_cooling_limit;//机器人 42mm 枪口热量上限
uint16_t shooter_id1_42mm_speed_limit;//机器人 42mm 枪口上限速度 单位 m/s
uint16_t chassis_power_limit;//机器人底盘功率限制上限
uint8_t mains_power_gimbal_output : 1;
uint8_t mains_power_chassis_output : 1;
uint8_t mains_power_shooter_output : 1;
} ext_game_robot_status_t;

typedef __packed struct
{
uint8_t armor_id : 4;
uint8_t hurt_type : 4;
} ext_robot_hurt_t;

typedef __packed struct
{
uint32_t event_type;
} ext_event_data_t;

typedef __packed struct
{
uint16_t bullet_remaining_num_17mm;
uint16_t bullet_remaining_num_42mm;
uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

typedef struct
{
	int data_length;
	int seq;
	int cmd;
}JUDGE_HEADER;

/* 自定义帧头 */
typedef __packed struct
{
	uint8_t  SOF;
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
} xFrameHeader;

/* 	
	交互数据，包括一个统一的数据段头结构，
	包含了内容 ID，发送者以及接受者的 ID 和内容数据段，
	整个交互数据的包总共长最大为 128 个字节，
	减去 frame_header,cmd_id,frame_tail 以及数据段头结构的 6 个字节，
	故而发送的内容数据段最大为 113。
	整个交互数据 0x0301 的包上行频率为 10Hz。

	机器人 ID：
	1，英雄(红)；
	2，工程(红)；
	3/4/5，步兵(红)；
	6，空中(红)；
	7，哨兵(红)；
	11，英雄(蓝)；
	12，工程(蓝)；
	13/14/15，步兵(蓝)；
	16，空中(蓝)；
	17，哨兵(蓝)。 
	客户端 ID： 
	0x0101 为英雄操作手客户端( 红) ；
	0x0102 ，工程操作手客户端 ((红 )；
	0x0103/0x0104/0x0105，步兵操作手客户端(红)；
	0x0106，空中操作手客户端((红)； 
	0x0111，英雄操作手客户端(蓝)；
	0x0112，工程操作手客户端(蓝)；
	0x0113/0x0114/0x0115，操作手客户端步兵(蓝)；
	0x0116，空中操作手客户端(蓝)。 
*/
/* 交互数据接收信息：0x0301  */
typedef __packed struct 
{ 
	uint16_t data_cmd_id;    
	uint16_t send_ID;    
	uint16_t receiver_ID; 
} ext_student_interactive_header_data_t; 


/* 
	客户端 客户端自定义数据：cmd_id:0x0301。内容 ID:0xD180
	发送频率：上限 10Hz


	1.	客户端 客户端自定义数据：cmd_id:0x0301。内容 ID:0xD180。发送频率：上限 10Hz 
	字节偏移量 	大小 	说明 				备注 
	0 			2 		数据的内容 ID 		0xD180 
	2 			2 		送者的 ID 			需要校验发送者机器人的 ID 正确性 
	4 			2 		客户端的 ID 		只能为发送者机器人对应的客户端 
	6 			4 		自定义浮点数据 1 	 
	10 			4 		自定义浮点数据 2 	 
	14 			4 		自定义浮点数据 3 	 
	18 			1 		自定义 8 位数据 4 	 

*/ 
typedef __packed struct 
{ 
	float data1; 
	float data2; 
	float data3; 
	uint8_t masks; 
} client_custom_data_t;
//帧头  命令码   数据段头结构  数据段   帧尾
//上传客户端
typedef __packed struct
{
	xFrameHeader   							txFrameHeader;//帧头
	uint16_t		 						CmdID;//命令码
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	client_custom_data_t  					clientData;//数据段
	uint16_t		 						FrameTail;//帧尾
}ext_SendClientData_t;







extern uint16_t 								HP_percen;
extern frame_header_t 					head_data;	//帧头
extern ext_game_robot_HP_t 			HP;					//血量
extern ext_bullet_remaining_t  	remain;			//剩余弹量
extern ext_game_robot_status_t 	My_status;	//自身状态
extern ext_power_heat_data_t   	power;			//功率
extern ext_event_data_t					Event_data;//场地事件


extern void judge_rcdata(unsigned char *data,int lenth);
#endif
