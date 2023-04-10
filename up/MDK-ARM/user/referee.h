#ifndef __REFEREE_H__
#define __REFEREE_H__

#include "crc.h"


#define JUDGE_LENGTH 2048
#define JUDGE_HEAD_LENGTH 5
#define JUDGE_CMD_LENGTH 2
#define JUDGE_CRC16_LENGTH 2

#define empty 0

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
float chassis_power;
uint16_t chassis_power_buffer;
uint16_t shooter_id1_17mm_cooling_heat;
uint16_t shooter_id2_17mm_cooling_heat;
uint16_t shooter_id1_42mm_cooling_heat;
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
uint16_t shooter_id1_42mm_cooling_rate;
uint16_t shooter_id1_42mm_cooling_limit;
uint16_t shooter_id1_42mm_speed_limit;
uint16_t chassis_power_limit;
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

typedef __packed struct 
{ 
	uint16_t data_cmd_id;    
	uint16_t send_ID;    
	uint16_t receiver_ID; 
} ext_student_interactive_header_data_t; 

typedef __packed struct
{
uint8_t graphic_name[3];
uint32_t operate_tpye:3;
uint32_t graphic_tpye:3;
uint32_t layer:4;
uint32_t color:4;
uint32_t start_angle:9;
uint32_t end_angle:9;
uint32_t width:10;
uint32_t start_x:11;
uint32_t start_y:11;
uint32_t radius:10;
uint32_t end_x:11;
uint32_t end_y:11;
} graphic_data_struct_t;

//帧头  命令码   数据段头结构  数据段   帧尾
//上传客户端
typedef __packed struct
{
	xFrameHeader   							txFrameHeader;//帧头
	uint16_t		 						CmdID;//命令码
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	graphic_data_struct_t  					clientData[7];//数据段
	uint16_t		 						FrameTail;//帧尾
}ext_SendClientData_t;

//绘字符串
typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct;
	uint8_t data[30];
} ext_client_string_t;

typedef __packed struct
{
	xFrameHeader txFrameHeader;								//帧头
	uint16_t  CmdID;										//命令码
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	ext_client_string_t 					clientData;//数据段
	uint16_t	FrameTail;								//帧尾
}ext_charstring_data_t;





extern uint16_t 								HP_percen;
extern frame_header_t 					head_data;	//Ö¡Í·
extern ext_game_robot_HP_t 			HP;					//ÑªÁ¿
extern ext_bullet_remaining_t  	remain;			//Ê£Óàµ¯Á¿
extern ext_game_robot_status_t 	My_status;	//×ÔÉí×´Ì¬
extern ext_power_heat_data_t   	power;			//¹¦ÂÊ
extern ext_event_data_t					Event_data;//³¡µØÊÂ¼þ

void referee_task(void);
extern  int determine_shoot(void);
extern void judge_rcdata(unsigned char *data,int lenth);
void determine_super_target(void);
#endif
