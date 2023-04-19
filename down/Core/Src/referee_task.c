/* Includes ------------------------------------------------------------------*/
#include "referee_task.h"
#include "cmsis_os.h"
#include "chassis_mode.h"
#include  "judge_protocol.h"
#include <string.h>
#include "can.h"
/* Private variables ---------------------------------------------------------*/

uint8_t CliendTxBuffer[100];
char referee_supcap[11]= {"super cap:"};//超级电容剩余量
char empty_line[20] = {"                    "};
char super_cap1[2],super_cap2[2],percent_sign[2]={'%'},follow_mode[7]={"FOLLOW"},gyro_mode[5]={"GYRO"},aimbot_mode[12]={"AIMBOTMODE:"},
      aimbot_status_on[3]={"ON"},aimbot_status_off[4]={"OFF"}, continuouds_shoot[18]={"CONTINUOUDS_SHOOT"},single_shoot[13]={"SINGLE_SHOOT"};
char shoot_number_char[2];
/**************************字符串坑中坑：像百分号它只有1个，但是得设置2个内存的数组，其他的也一样，因为数组末尾默认有一个占位符 ****************************/
extern int Chassis_mode;
extern int Shooting_mode;
extern uint8_t judge_tag;
extern uint8_t Judge_Buffer[128];
/* Private typedef -----------------------------------------------------------*/
UI_char_t UI_char=
{
	.char_send_times=0,

};
Graphic_Color Cap_Color=GREEN;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart5;
extern judge_info_t judge_infop;

extern struct supercap supercap;
/* Private functions ---------------------------------------------------------*/
void acknowledge_client_id(void);
void Client_Char_update(void);
void Client_Line_update(void);
/**
 * @brief UI控制
 * @param 
 */

int cnt66=0;
void UI_Ctrl(void)
{

	if(judge_tag==1){
	judge_update(Judge_Buffer);
	}
	acknowledge_client_id();
	cnt66++;
	Client_Char_update();
	Client_Line_update();
}
#define SUPERCAP_VOLTAGE_MAX 2150
void Get_supercap_(void)
{
	super_cap1[0]=(((((uint16_t)supercap.cap_V)*100)/SUPERCAP_VOLTAGE_MAX)/10)+0x30;/*0x30是ASCll表对应的数字0*/
  super_cap2 [0]=(((((uint16_t)supercap.cap_V)*100)/SUPERCAP_VOLTAGE_MAX)%10)+0x30;	
}

/**
 *	@brief	字符串图层配置
 */
ext_charstring_data_t tx_client_char;
void Char_Graphic(ext_client_string_t* graphic,//最终要发出去的数组中的数据段内容
									const char* name,
									uint32_t operate_tpye,									
									uint32_t layer,
									uint32_t color,
									uint32_t size,
									uint32_t length,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,																	
									const char *character)//外部放入的数组
{
	for(char i=0;i<3;i++){
	graphic->grapic_data_struct.graphic_name[i] = name[i];	//字符索引
	}
	graphic->grapic_data_struct.operate_tpye = operate_tpye; 
	graphic->grapic_data_struct.graphic_tpye = CHAR;         
	graphic->grapic_data_struct.layer = layer;
	graphic->grapic_data_struct.color = color;
	graphic->grapic_data_struct.start_angle = size;
	graphic->grapic_data_struct.end_angle = length;	
	graphic->grapic_data_struct.width = width;
	graphic->grapic_data_struct.start_x = start_x;
	graphic->grapic_data_struct.start_y = start_y;	
	memcpy(graphic->data,empty_line,19);
  memcpy(graphic->data,character,length);
}

/**
 *	@brief	字符串初始化
 */
int ui_x_st=1020;
int ui_y_st=450;
int ui_x_end=890;
int ui_y_end=450;
extern int hero_shoot_number;
static void Char_Init()
{
	if(UI_char.char_send_times==0){     /*添加 超级电容字符串*/
	Char_Graphic(&tx_client_char.clientData,"CL1",ADD,0,YELLOW,25,strlen(referee_supcap),5,(1420),(1080*9/12),referee_supcap);
	UI_char.char_send_times=1;
	}
	else if(UI_char.char_send_times==1){/*添加 超级电容剩余量百分十位*/
	Char_Graphic(&tx_client_char.clientData,"CL2",ADD,0,GREEN,25,strlen(super_cap1),5,(1420+250+30),(1080*9/12),super_cap1);
	UI_char.char_send_times=2;
	}
	else if(UI_char.char_send_times==2){/*添加 超级电容余量百分个位*/
	Char_Graphic(&tx_client_char.clientData,"CL3",ADD,0,GREEN,25,strlen(super_cap2),5,(1420+275+30),(1080*9/12),super_cap2);
	UI_char.char_send_times=3;
	}
	else if(UI_char.char_send_times==3){/*添加 百分号*/
	Char_Graphic(&tx_client_char.clientData,"CL4",ADD,0,YELLOW,25,strlen(percent_sign),5,(1420+300+30),(1080*9/12),percent_sign);
	UI_char.char_send_times=4;
	}
	else if(UI_char.char_send_times==4){/*更新 超级电容剩余量百分十位*/
	Get_supercap_();
	if(super_cap1[0]<=0x36){            /*电容危险信号*/
	Cap_Color=RED_BLUE;
	}
	else{
	Cap_Color=GREEN;
	}
	Char_Graphic(&tx_client_char.clientData,"CL2",MODIFY,0,Cap_Color,25,strlen(super_cap1),5,(1420+250+30),(1080*9/12),super_cap1);
	UI_char.char_send_times=5;
	}
	else if(UI_char.char_send_times==5){/*更新 超级电容余量百分个位*/
	Char_Graphic(&tx_client_char.clientData,"CL3",MODIFY,0,Cap_Color,25,strlen(super_cap2),5,(1420+275+30),(1080*9/12),super_cap2);
	UI_char.char_send_times=6;
	}
	else if(UI_char.char_send_times==6){/*添加 底盘跟随模式*/	
	Char_Graphic(&tx_client_char.clientData,"CL5",ADD,0,YELLOW,25,strlen(follow_mode),5,(50),(1080*9/12),follow_mode);
	UI_char.char_send_times=7;	
	}
	else if(UI_char.char_send_times==7){/*添加 底盘小陀螺模式*/
	Char_Graphic(&tx_client_char.clientData,"CL6",ADD,0,YELLOW,25,strlen(gyro_mode),5,(60),(1080*8/12),gyro_mode);
		UI_char.char_send_times=8;
	}
	else if(UI_char.char_send_times==8){/*添加 发射量*/
	shoot_number_char[0]=((uint16_t)hero_shoot_number)+0x30;
	Char_Graphic(&tx_client_char.clientData,"CL7",ADD,0,GREEN,25,strlen(shoot_number_char),5,(1045),(450),shoot_number_char);
	UI_char.char_send_times=9;
	}
	else if(UI_char.char_send_times==9){/*添加 发射量*/
	shoot_number_char[0]=((uint16_t)hero_shoot_number)+0x30;
		Char_Graphic(&tx_client_char.clientData,"CL7",MODIFY,0,GREEN,25,strlen(shoot_number_char),5,(1045),(450),shoot_number_char);
	UI_char.char_send_times=0;
	}
}

/**
 *	@brief	字符串图层发送
 */
void Client_Char_update(void)
{
		//帧头
		tx_client_char.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_client_char.txFrameHeader.data_length = sizeof(ext_aerial_data_t) + sizeof(ext_client_string_t);
		tx_client_char.txFrameHeader.seq = 0;//包序号
		memcpy(CliendTxBuffer,&tx_client_char.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//头校验
	
		//命令码
		tx_client_char.CmdID = ID_COMMUNICATION;
		
		//数据段头结构
		tx_client_char.dataFrameHeader.cmd_id = INTERACT_ID_draw_char_graphic;
		tx_client_char.dataFrameHeader.send_id     = judge_infop.GameRobotStatus.robot_id;
		tx_client_char.dataFrameHeader.receive_id = judge_infop.self_client;
		
		//数据段
		Char_Init();
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_char.CmdID, LEN_CMD_ID+tx_client_char.txFrameHeader.data_length);//加上命令码长度2
		
		//帧尾
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_client_char));
		
		HAL_UART_Transmit(&huart5,CliendTxBuffer,sizeof(tx_client_char),0x50);
}

/**
 *	@brief	绘制line类型
 */
ext_line_one_data_t ext_line_one_data;
void Line_Graphic(graphic_data_struct_t* graphic,//最终要发出去的数组的数据段内容
									const char* name,
									uint32_t operate_tpye,
									uint32_t graphic_tpye,//绘制什么图像
									uint32_t layer,
									uint32_t color,
									uint32_t start_angle,
									uint32_t end_angle,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,
									uint32_t radius,
                   uint32_t end_x,
									uint32_t end_y
								)													
{
	for(char i=0;i<3;i++){
	graphic->graphic_name[i] = name[i];	//字符索引
	}
	graphic->operate_tpye = operate_tpye; //图层操作
	graphic->graphic_tpye = graphic_tpye;        
	graphic->layer        = layer;//都在第一层
	graphic->color        = color;//变色
	graphic->start_angle  = start_angle;
	graphic->end_angle    = end_angle;
	graphic->width        = width;
	graphic->start_x      = start_x;
	graphic->start_y      = start_y;
  graphic->radius       = radius;	
	graphic->end_x        = end_x;
	graphic->end_y        = end_y;
}
static uint8_t line_send_times=0;
float super_per;
extern int chassis_mode;
static void Line_int()//线形初始化
{  
	//int super_per;
	if(line_send_times==0){
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI1",ADD,LINE,0,CYAN_BLUE,0,0,2,960,600,0,960,300);
	line_send_times=1;
	}
	else if(line_send_times==1){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI2",ADD,LINE,0,GREEN,0,0,6,ui_x_st,ui_y_st,0,ui_x_end,ui_y_end);
		//4m瞄准线
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI2",ADD,LINE,0,CYAN_BLUE,0,0,2,1020,450,0,890,450);
	line_send_times=2;
	}
	else if(line_send_times==2){
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI3",ADD,LINE,0,GREEN,0,0,10,1500,0,0,1500,250);
	line_send_times=3;
	}
	
	else if(line_send_times==3){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI4",ADD,CIRCLE,0,GREEN,0,0,10,ui_x_st,ui_y_st,5,ui_x_end,ui_y_end);
		//超电方框初始化;
		Line_Graphic(&ext_line_one_data.Line_data_struct,"LI4",ADD,RECTANGLE,0,WHITE,60,260,7,1510-80,760,60,1850-80,610+120);
	line_send_times=4;
	}
	
	
	else if(line_send_times==4){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI4",ADD,CIRCLE,0,GREEN,0,0,10,ui_x_st,ui_y_st,5,ui_x_end,ui_y_end)
		//底盘跟随模式圆圈初始化;
		Line_Graphic(&ext_line_one_data.Line_data_struct,"LI5",ADD,CIRCLE,0,FUCHSIA,60,260,7,250,790+5,25,1354,610);
	line_send_times=5;
	}
	
		else if(line_send_times==5){
			//超电绿条初始化
			super_per=(supercap.cap_V)/SUPERCAP_VOLTAGE_MAX;
		Line_Graphic(&ext_line_one_data.Line_data_struct,"LI6",ADD,LINE,0,GREEN,0,0,24,1432,745,1512,1432+(335*super_per),745);
	line_send_times=6;
	}
		else if(line_send_times==6){
		//超电绿条更新
			super_per=(supercap.cap_V)/SUPERCAP_VOLTAGE_MAX;
			if(super_per<0.75) Line_Graphic(&ext_line_one_data.Line_data_struct,"LI6",MODIFY,LINE,0,RED_BLUE,0,0,24,1432,745,1512,1432+(335*super_per),745);
			else
			{
			Line_Graphic(&ext_line_one_data.Line_data_struct,"LI6",MODIFY,LINE,0,GREEN,0,0,24,1432,745,1512,1432+(335*super_per),745);
			}		
	line_send_times=7;
	}
		
	else if(line_send_times==7){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI4",ADD,CIRCLE,0,GREEN,0,0,10,ui_x_st,ui_y_st,5,ui_x_end,ui_y_end)
		//小陀螺模式圆圈初始化;
		Line_Graphic(&ext_line_one_data.Line_data_struct,"LI7",ADD,CIRCLE,0,FUCHSIA,60,260,7,250,712,25,1354,610);
	line_send_times=8;
	}
	
	else if(line_send_times==8){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI4",ADD,CIRCLE,0,GREEN,0,0,10,ui_x_st,ui_y_st,5,ui_x_end,ui_y_end)
		//小陀螺模式圆圈更新;
		if(chassis_mode==Small_gyroscope)
		{
			Line_Graphic(&ext_line_one_data.Line_data_struct,"LI7",MODIFY,CIRCLE,0,GREEN,60,260,7,250,712,25,1354,610);
		}
		if(chassis_mode==cheassis_follow)
		{
			Line_Graphic(&ext_line_one_data.Line_data_struct,"LI7",MODIFY,CIRCLE,0,FUCHSIA,60,260,7,250,712,25,1354,610);
		}
	line_send_times=9;
	}
	
	
	
	
	else if(line_send_times==9){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI4",ADD,CIRCLE,0,GREEN,0,0,10,ui_x_st,ui_y_st,5,ui_x_end,ui_y_end)
		//底盘跟随模式圆圈更新;
		if(chassis_mode==Small_gyroscope)
		{
			Line_Graphic(&ext_line_one_data.Line_data_struct,"LI5",MODIFY,CIRCLE,0,FUCHSIA,60,260,7,250,790+5,25,1354,610);
		}
		if(chassis_mode==cheassis_follow)
		{
			Line_Graphic(&ext_line_one_data.Line_data_struct,"LI5",MODIFY,CIRCLE,0,GREEN,60,260,7,250,790+5,25,1354,610);
		}
	line_send_times=10;
	}
	else if(line_send_times==10){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI2",ADD,LINE,0,GREEN,0,0,6,ui_xui_y_st_st,ui_y_st,0,ui_x_end,ui_y_end);
		//5m瞄准线
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI8",ADD,LINE,0,CYAN_BLUE,0,0,2,1020,420,0,890,420);
	line_send_times=11;
	}
	else if(line_send_times==11){
	//Line_Graphic(&ext_line_one_data.Line_data_struct,"LI2",ADD,LINE,0,GREEN,0,0,6,ui_xui_y_st_st,ui_y_st,0,ui_x_end,ui_y_end);
		//3m瞄准线
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI9",ADD,LINE,0,CYAN_BLUE,0,0,2,1020,480,0,890,480);
	line_send_times=0;
	}
}

void Client_Line_update(void)
{ 
		//帧头
		ext_line_one_data.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		ext_line_one_data.txFrameHeader.data_length = sizeof(ext_aerial_data_t) + sizeof(graphic_data_struct_t);
		ext_line_one_data.txFrameHeader.seq = 0;//包序号
		memcpy(CliendTxBuffer,&ext_line_one_data.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//头校验

		//命令码
		ext_line_one_data.CmdID = ID_COMMUNICATION;

		//数据段头结构
		ext_line_one_data.dataFrameHeader.cmd_id = INTERACT_ID_draw_one_graphic;
		ext_line_one_data.dataFrameHeader.send_id     = judge_infop.GameRobotStatus.robot_id;
		ext_line_one_data.dataFrameHeader.receive_id = judge_infop.self_client;
	
		//数据段
		Line_int();
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&ext_line_one_data.CmdID, LEN_CMD_ID+ext_line_one_data.txFrameHeader.data_length);//加上命令码长度2

		//帧尾
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(ext_line_one_data));
		
		HAL_UART_Transmit(&huart5,CliendTxBuffer,sizeof(ext_line_one_data),0x50);
		
}

 void acknowledge_client_id(void)
{
 judge_info_t*p=&judge_infop;
 if(p->GameRobotStatus.robot_id==1){
 p->self_client=0x0101;
 }
 else if(p->GameRobotStatus.robot_id==101){
 p->self_client=0x0165;
 }
}
int chassis_referee_mode=3;
int shoot_referee_mode=3;
void chassis_mode_judge(void)
{
	if(judge_infop.GameRobotStatus.max_HP==250 && judge_infop.GameRobotStatus.robot_level==1)
	{
		chassis_referee_mode=Health_priority;
	}
	else if(judge_infop.GameRobotStatus.max_HP==200 && judge_infop.GameRobotStatus.robot_level==1)
	{
		chassis_referee_mode=Power_priority;
	}
	if(judge_infop.GameRobotStatus.shooter_id1_42mm_speed_limit==16 && judge_infop.GameRobotStatus.shooter_id1_42mm_cooling_limit==100&& judge_infop.GameRobotStatus.robot_level==1)
	{
		shoot_referee_mode=speed_priority;
	}
	else if(judge_infop.GameRobotStatus.shooter_id1_42mm_speed_limit==10 && judge_infop.GameRobotStatus.shooter_id1_42mm_cooling_limit==200&& judge_infop.GameRobotStatus.robot_level==1)
	{
		shoot_referee_mode=outburst_priority;
	}
}

void super_cap_buffer(void)
{
	if(judge_infop.PowerHeatData.chassis_power_buffer>30)
	{
		switch(chassis_referee_mode)
		{
			case Power_priority:
			{
					if(judge_infop.GameRobotStatus.robot_level==1)
					{
						CAN1_0x333_TX(80);
					}
					else if(judge_infop.GameRobotStatus.robot_level==2)
					{
						CAN1_0x333_TX(100);
					}
					else if(judge_infop.GameRobotStatus.robot_level==3)
					{
						CAN1_0x333_TX(130);
					}
			}
			break;
			case Health_priority:
			{
					if(judge_infop.GameRobotStatus.robot_level==1)
					{
						CAN1_0x333_TX(60);
					}
					else if(judge_infop.GameRobotStatus.robot_level==2)
					{
						CAN1_0x333_TX(70);
					}
					else if(judge_infop.GameRobotStatus.robot_level==3)
					{
						CAN1_0x333_TX(75);
					}
			}
			break;
			default: {break;}
		}
	}
	else if(judge_infop.PowerHeatData.chassis_power_buffer<5)
	{
		switch(chassis_referee_mode)
		{
			case Power_priority:
			{
					if(judge_infop.GameRobotStatus.robot_level==1)
					{
						CAN1_0x333_TX(63);
					}
					else if(judge_infop.GameRobotStatus.robot_level==2)
					{
						CAN1_0x333_TX(83);
					}
					else if(judge_infop.GameRobotStatus.robot_level==3)
					{
						CAN1_0x333_TX(113);
					}
			}
			break;
			case Health_priority:
			{
					if(judge_infop.GameRobotStatus.robot_level==1)
					{
						CAN1_0x333_TX(50);
					}
					else if(judge_infop.GameRobotStatus.robot_level==2)
					{
						CAN1_0x333_TX(55);
					}
					else if(judge_infop.GameRobotStatus.robot_level==3)
					{
						CAN1_0x333_TX(60);
					}
			}
			break;
			default: {break;}
		}
	}
}
int hero_refree_shoot_number(void)
{
	static int number=0;
	number=(judge_infop.GameRobotStatus.shooter_id1_42mm_cooling_limit-judge_infop.PowerHeatData.shooter_id1_42mm_cooling_heat)/100;
}