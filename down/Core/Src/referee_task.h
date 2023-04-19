#ifndef __REFEREE_TASK_H_
#define __REFEREE_TASK_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>
#include  "judge_protocol.h"
/* Exported macro ------------------------------------------------------------*/
#define JUDGE_FRAME_HEADER 0xA5           //帧头
#define Power_priority 1
#define Health_priority 0

#define outburst_priority 1
#define speed_priority 0
/* Exported types ------------------------------------------------------------*/

typedef struct
{
	uint8_t char_send_times :4;
}UI_char_t;

/* 
	学生机器人间通信 cmd_id 0x0301，内容 data_ID:0x0200~0x02FF
	交互数据 机器人间通信：0x0301。
	发送频率：数据上下行合计带宽不超过 5000 Byte。 上下行发送频率分别不超过30Hz。
 * +------+------+-------------+------------------------------------+
 * | byte | size |    breif    |            note                    |
 * |offset|      |             |                                    |
 * +------+------+-------------+------------------------------------+
 * |  0   |  2   | 	 data_ID   | 0x0200~0x02FF,可以在这些 ID 段选取 |
 * |      |      |             | 具体ID含义由参赛队自定义           |
 * +------|------|-------------|------------------------------------|
 * |  2   |  2   | 	sender_ID  | 需要校验发送者的 ID 正确性					|
 * +------|------|-------------|------------------------------------|
 * |  4   |  2   | receiver_ID | 需要校验接收者的 ID 正确性					|
 * |      |      |             | 例如不能发送到敌对机器人的ID				| 
 * +------|------|-------------|------------------------------------|
 * |  6   |  n   | 		Data     | n 需要小于 113 										|
 * +------+------+-------------+------------------------------------+
*/

/******************************客户端交互数据**************************************/
enum
{
	//0x200-0x02ff 	队伍自定义命令 格式  INTERACT_ID_XXXX
	INTERACT_ID_delete_graphic 			= 0x0100,	/*客户端删除图形*/
	INTERACT_ID_draw_one_graphic 		= 0x0101,	/*客户端绘制一个图形*/
	INTERACT_ID_draw_two_graphic 		= 0x0102,	/*客户端绘制2个图形*/
	INTERACT_ID_draw_five_graphic 	= 0x0103,	/*客户端绘制5个图形*/
	INTERACT_ID_draw_seven_graphic 	= 0x0104,	/*客户端绘制7个图形*/
	INTERACT_ID_draw_char_graphic 	= 0x0110,	/*客户端绘制字符图形*/
	INTERACT_ID_bigbome_num					= 0x02ff
};
//单位（字节）
enum
{
	LEN_INTERACT_delete_graphic     = 8,  //删除图层 2(数据内容ID)+2(发送者ID)+2（接收者ID）+2（数据内容）  
	LEN_INTERACT_draw_one_graphic   = 21, // 以上2+2+2+15
	LEN_INTERACT_draw_two_graphic   = 36, //6+15*2
	LEN_INTERACT_draw_five_graphic  = 81, //6+15*5
	LEN_INTERACT_draw_seven_graphic = 111,//6+15*7
	LEN_INTERACT_draw_char_graphic  = 51, //6+15+30（字符串内容）
};
//****************************绘图的数据段内容****************************/

typedef __packed struct//图形
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; //直线  矩形  正圆  椭圆  圆弧  浮点  整型  字符
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  //空    空    空    空    角度  大小  大小  大小
	uint32_t end_angle:9;    //空    空    空    空          位数  空    长度
	uint32_t width:10;       
	uint32_t start_x:11;     //起点  起点  圆心  圆心  圆心  起点  起点  起点
	uint32_t start_y:11;     //
	uint32_t radius:10;      //空    空    半径  空    空    、    、    空
	uint32_t end_x:11;       //终点  对顶  空    半轴  半轴  、    、    空
	uint32_t end_y:11;       //                              数    数    空
} graphic_data_struct_t;

typedef __packed struct//Char型
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
} Char_data_struct_t;
typedef __packed struct//Char型
{
	graphic_data_struct_t grapic_data_struct;
	uint8_t data[30];
} ext_client_string_t;

//bit 0-2
typedef enum
{
	NONE   = 0,/*空操作*/
	ADD    = 1,/*增加图层*/
	MODIFY = 2,/*修改图层*/
	DELETE = 3,/*删除图层*/
}Graphic_Operate;

/*图层操作*/
//bit3-5
typedef enum
{
	LINE      = 0,//直线
	RECTANGLE = 1,//矩形
	CIRCLE    = 2,//正圆
	OVAL      = 3,//椭圆
	ARC       = 4,//圆弧
	FLOAT     = 5,//浮点数
	INT       = 6,//整型数
	CHAR      = 7 //字符
}Graphic_Type;

/*图层类型*/
//bit 6-9图层数 最大为9，最小0
//bit 10-13颜色
typedef enum
{
	RED_BLUE  = 0,//红蓝主色	
	YELLOW    = 1,
	GREEN     = 2,
	ORANGE    = 3,
	FUCHSIA   = 4,	/*紫红色*/
	PINK      = 5,
	CYAN_BLUE = 6,	/*青色*/
	BLACK     = 7,
	WHITE     = 8
}Graphic_Color;

/*图层颜色类型*/
//bit 14-31 角度 [0,360]
/**********************************客户端绘图************************************************/

//绘制字符串
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			//帧头
	uint16_t  CmdID;										//命令码
	ext_aerial_data_t   dataFrameHeader;//数据段头结构
	ext_client_string_t clientData;//数据段
	uint16_t	FrameTail;								//帧尾
}ext_charstring_data_t;

//绘制线形
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_aerial_data_t   dataFrameHeader;
	graphic_data_struct_t Line_data_struct;	
	uint16_t	FrameTail;								
}ext_line_one_data_t;

/* Exported functions --------------------------------------------------------*/
void Client_Line_update(void);
void Client_Char_update(void);
void Client_int_update(void);
void acknowledge_client_id(void);
void  UI_Ctrl(void);
void chassis_mode_judge(void);
void super_cap_buffer(void);
int hero_refree_shoot_number(void);
#endif
