#include "main.h"
#include "referee.h"
#include "usart.h"
#include "string.h"
#include "chassis_mode.h"
extern uint8_t judge_tag;
uint16_t HP_percen = 100;


uint8_t CliendTxBuffer[100];
char referee_supcap[16]= {"supercapacitor:"};//超级电容剩余量
char empty_line[20] = {"                    "};
char super_cap1[2],super_cap2[2],percent_sign[2]={'%'},follow_mode[7]={"FOLLOW"},gyro_mode[5]={"GYRO"},aimbot_mode[12]={"AIMBOTMODE:"},
      aimbot_status_on[3]={"ON"},aimbot_status_off[4]={"OFF"}, continuouds_shoot[18]={"CONTINUOUDS_SHOOT"},single_shoot[13]={"SINGLE_SHOOT"};
/**************************字符串坑中坑：像百分号它只有1个，但是得设置2个内存的数组，其他的也一样，因为数组末尾默认有一个占位符 ****************************/
extern int Chassis_mode;
extern int Shooting_mode;
extern uint8_t judge_tag;
extern uint8_t Judge_Buffer[128];
/* Private typedef -----------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart4;
UI_char_t UI_char=
{
	.char_send_times=0,

};
Graphic_Color Cap_Color=GREEN;

			

frame_header_t 					shead_data;//帧头
ext_game_robot_HP_t			HP;//血量
ext_bullet_remaining_t  remain;//剩余弹量
ext_game_robot_status_t My_status;//自身状态
ext_power_heat_data_t   power;
ext_shoot_data_t 				shoot_data;
ext_robot_hurt_t        hurt_data;
ext_event_data_t				Event_data;
unsigned char judge_buffer[JUDGE_LENGTH];
extern int chassis_mode;
int data_length = 0;
int data_fulllength = 0;
JUDGE_HEADER judge_header;
#define BLUE  0
#define RED   1
uint8_t Judge_Self_ID;//当前机器人的ID
uint16_t Judge_SelfClient_ID;//发送者机器人对应的客户端ID


extern uint8_t Judge_Buffer[128];

static union{//联合体传输浮点数

float value;

uint8_t recivebuf[4];

}chassis_remain_power,shoot_speed;

void judge_rcdata(unsigned char *data,int lenth)
{
	int i = 0; 
	for(i = 0; i < lenth - 3; i++)  //-1 防止越界
	{
		if(*data == 0xA5)
		{
			if(ref_verify_crc8(data,JUDGE_HEAD_LENGTH))
			{
				judge_header.data_length = *(data + 2);
				judge_header.data_length = (unsigned short)((judge_header.data_length << 8) | *(data + 1));
				judge_header.cmd = *(data + 6);
				judge_header.cmd = (unsigned short)((judge_header.cmd << 8) | *(data + 5));
				data_fulllength = JUDGE_HEAD_LENGTH + JUDGE_CMD_LENGTH + judge_header.data_length + JUDGE_CRC16_LENGTH;
				//printf("length : %d\n",data_fulllength);
				if(ref_verify_crc16(data,data_fulllength))
				{
					*data += 1;
					//数据处理
					//printf("cmd: %0#x\n",judge_header.cmd);
					//printf("length: %d\n",judge_header.data_length);
					switch(judge_header.cmd)
					{
						case 0x0003: //血量读取
						HP.red_1_robot_HP=data[7+11]<<8 | data[7+10];
						HP.blue_1_robot_HP=data[7+27]<<8 | data[7+26];
					break;
					case 0x0101:
						Event_data.event_type=data[7+3]<<24 | data[7+2]<<16 | data[7+1]<<8 | data[7];
					break;
					case 0x0201://英雄状态
						My_status.robot_id=data[7+0];
						My_status.robot_level=data[7+1];
						My_status.remain_HP=data[7+2+1]<<8 | data[7+2];
						My_status.max_HP=data[7+4+1]<<8 | data[7+4];
						My_status.shooter_id1_17mm_cooling_rate=data[7+6+1]<<8 | data[7+6]<<8;
						My_status.shooter_id1_17mm_cooling_limit=data[7+8+1]<<8 | data[7+8];
						My_status.shooter_id1_17mm_speed_limit=data[7+10+1]<<8 | data[7+10];
						My_status.shooter_id2_17mm_cooling_rate=data[7+12+1]<<8 | data[7+12]<<8;
						My_status.shooter_id2_17mm_cooling_limit=data[7+14+1]<<8 | data[7+14];
						My_status.shooter_id2_17mm_speed_limit=data[7+16+1]<<8 | data[7+16];
						My_status.shooter_id1_42mm_cooling_rate=data[7+18+1]<<8 | data[7+18];
						My_status.shooter_id1_42mm_cooling_limit=data[7+20+1]<<8 | data[7+20];
						My_status.shooter_id1_42mm_speed_limit=data[7+22+1]<<8 | data[7+22];
						My_status.chassis_power_limit=data[7+24+1]<<8 | data[7+24];
						My_status.mains_power_gimbal_output=data[7+26]&0x01;
						My_status.mains_power_chassis_output=(data[7+26]>>1)&0x01;
						My_status.mains_power_shooter_output=(data[7+26]>>2)&0x01;
					break;
					case 0x0202:
						power.chassis_volt   =data[7+1]<<8 | data[7];
						power.chassis_current=data[7+2+1]<<8 | data[7+2];
						for(i=0;i<4;i++)
						{
							chassis_remain_power.recivebuf[i]=(uint8_t)data[7+4+i];
						};
						power.chassis_power=chassis_remain_power.value;
						power.chassis_power_buffer=data[7+8+1]<<8 | data[7+8];
						power.shooter_id1_17mm_cooling_heat=data[7+10+1]<<8 | data[7+10];
						power.shooter_id2_17mm_cooling_heat=data[7+12+1]<<8 | data[7+12];
						power.shooter_id1_42mm_cooling_heat=data[7+14+1]<<8 | data[7+14];
					break;
					case 0x0206:
						hurt_data.armor_id=data[7]&0x0f;
						hurt_data.hurt_type=(data[7]>>4)&0x0f;
					break;
					case 0x0207:
						shoot_data.bullet_type=data[7];
						shoot_data.shooter_id=data[7+1];
						shoot_data.bullet_freq=data[7+2];
						for(i=0;i<4;i++)
						{
							shoot_speed.recivebuf[i]=data[7+3+i];
						};
						shoot_data.bullet_speed=shoot_speed.value;
					break;
					case 0x0208://剩余弹量获取
						remain.bullet_remaining_num_17mm=data[7+1] << 8 | data[7];
					break;
					}
					data++;//data += data_fulllength
				}
				else
				{
					*data += 1;
					data++;
				}
			}
			else
			{
				*data += 1;
				data++;//++ 或者 += JUDGE_HEAD_LENGTH
			}
		}
		else
		{
			data++;
		};
	}
}

/**
  * @brief  判断自己红蓝方
  * @param  void
  * @retval RED   BLUE
  * @attention  数据打包,打包完成后通过串口发送到裁判系统
  */
bool Color;
bool is_red_or_blue(void)
{
	Judge_Self_ID = My_status.robot_id;//读取当前机器人ID
	
	if(My_status.robot_id > 10)
	{
		return BLUE;
	}
	else 
	{
		return RED;
	}
}

/**
  * @brief  判断自身ID，选择客户端ID
  * @param  void
  * @retval RED   BLUE
  * @attention  数据打包,打包完成后通过串口发送到裁判系统
  */
void determine_ID(void)
{
	Color = is_red_or_blue();
	if(Color == BLUE)
	{
		Judge_SelfClient_ID = 0x0110 + (Judge_Self_ID-10);//计算客户端ID
	}
	else if(Color == RED)
	{
		Judge_SelfClient_ID = 0x0100 + Judge_Self_ID;//计算客户端ID
	}
}





int cap_now;


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
	graphic->grapic_data_struct.graphic_tpye = 7;         
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
static void Char_Init()
{
	if(UI_char.char_send_times==0){     /*添加 超级电容字符串*/
	Char_Graphic(&tx_client_char.clientData,"CL1",ADD,0,YELLOW,25,strlen(referee_supcap),5,(50),(1080*9/12),referee_supcap);
	UI_char.char_send_times=1;
	}
	else if(UI_char.char_send_times==1){/*添加 超级电容剩余量百分十位*/
	Char_Graphic(&tx_client_char.clientData,"CL2",ADD,0,GREEN,25,strlen(super_cap1),5,(460),(1080*9/12),super_cap1);
	UI_char.char_send_times=2;
	}
	else if(UI_char.char_send_times==2){/*添加 超级电容余量百分个位*/
	Char_Graphic(&tx_client_char.clientData,"CL3",ADD,0,GREEN,25,strlen(super_cap2),5,(485),(1080*9/12),super_cap2);
	UI_char.char_send_times=3;
	}
	else if(UI_char.char_send_times==3){/*添加 百分号*/
	Char_Graphic(&tx_client_char.clientData,"CL4",ADD,0,YELLOW,25,strlen(percent_sign),5,(510),(1080*9/12),percent_sign);
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
	Char_Graphic(&tx_client_char.clientData,"CL2",MODIFY,0,Cap_Color,25,strlen(super_cap1),5,(460),(1080*9/12),super_cap1);
	UI_char.char_send_times=5;
	}
	else if(UI_char.char_send_times==5){/*更新 超级电容余量百分个位*/
	Char_Graphic(&tx_client_char.clientData,"CL3",MODIFY,0,Cap_Color,25,strlen(super_cap2),5,(485),(1080*9/12),super_cap2);
	UI_char.char_send_times=6;
	}
	else if(UI_char.char_send_times==6){/*添加 底盘跟随模式*/	
	Char_Graphic(&tx_client_char.clientData,"CL5",ADD,0,YELLOW,25,strlen(follow_mode),5,(1400),(1080*9/12),follow_mode);
	UI_char.char_send_times=7;	
	}
	else if(UI_char.char_send_times==7){/*更新 底盘小陀螺模式*/
	if(Chassis_mode==GYRO){
	Char_Graphic(&tx_client_char.clientData,"CL5",MODIFY,0,RED_BLUE,25,strlen(gyro_mode),5,(1400),(1080*9/12),gyro_mode);
	}
	UI_char.char_send_times=8;
	}
	else if(UI_char.char_send_times==8){/*更新 底盘跟随模式*/
	if(Chassis_mode==FOLLOW){
	Char_Graphic(&tx_client_char.clientData,"CL5",MODIFY,0,YELLOW,25,strlen(follow_mode),5,(1400),(1080*9/12),follow_mode);
	}
	UI_char.char_send_times=9;
  }
	else if(UI_char.char_send_times==9){/*添加 自瞄模式*/
	Char_Graphic(&tx_client_char.clientData,"CL6",ADD,0,YELLOW,25,strlen(aimbot_mode),5,(50),(1080*9/13),aimbot_mode);
	UI_char.char_send_times=10;
  }
	else if(UI_char.char_send_times==10){/*添加 自瞄状态*/
	Char_Graphic(&tx_client_char.clientData,"CL7",ADD,0,GREEN,25,strlen(aimbot_status_off),5,(350),(1080*9/13),aimbot_status_off);
	UI_char.char_send_times=11;
  }
	else if(UI_char.char_send_times==11){/*更新 自瞄状态*/
	if(vision_info.Aimbot_mode==LOCK_ENEMY){
	Char_Graphic(&tx_client_char.clientData,"CL7",MODIFY,0,RED_BLUE,25,strlen(aimbot_status_on),5,(350),(1080*9/13),aimbot_status_on);
	}
	UI_char.char_send_times=12;
  }
	else if(UI_char.char_send_times==12){/*更新 自瞄状态*/
	if(vision_info.Aimbot_mode!=LOCK_ENEMY){
	Char_Graphic(&tx_client_char.clientData,"CL7",MODIFY,0,GREEN,25,strlen(aimbot_status_off),5,(350),(1080*9/13),aimbot_status_off);
	}
	UI_char.char_send_times=13;
  }
	else if(UI_char.char_send_times==13){/*添加 连续射击模式*/	
	Char_Graphic(&tx_client_char.clientData,"CL8",ADD,0,YELLOW,25,strlen(continuouds_shoot),5,(1400),(1080*9/13),continuouds_shoot);
	UI_char.char_send_times=14;	
	}
	else if(UI_char.char_send_times==14){/*更新 单发射击螺模式*/
	if(Shooting_mode==SINGLE_SHOOT){
	Char_Graphic(&tx_client_char.clientData,"CL8",MODIFY,0,RED_BLUE,25,strlen(single_shoot),5,(1400),(1080*9/13),single_shoot);
	}
	UI_char.char_send_times=15;
  }
	else if(UI_char.char_send_times==15){/*更新 连续射击螺模式*/
	if(Shooting_mode==CONTINUOUS_SHOOT){
	Char_Graphic(&tx_client_char.clientData,"CL8",MODIFY,0,YELLOW,25,strlen(continuouds_shoot),5,(1400),(1080*9/13),continuouds_shoot);
	}
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
		
		HAL_UART_Transmit(&huart1,CliendTxBuffer,sizeof(tx_client_char),0x50);
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
static void Line_int()//线形初始化
{  
	if(line_send_times==0){
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI1",ADD,LINE,0,GREEN,0,0,10,600,0,0,650,250);
	line_send_times=1;
	}
	else if(line_send_times==1){
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI2",ADD,LINE,0,GREEN,0,0,10,650,250,0,1500,250);
	line_send_times=2;
	}
	else if(line_send_times==2){
	Line_Graphic(&ext_line_one_data.Line_data_struct,"LI3",ADD,LINE,0,GREEN,0,0,10,1500,0,0,1500,250);
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
		
		HAL_UART_Transmit(&huart1,CliendTxBuffer,sizeof(ext_line_one_data),0x50);
		
}







void referee_task(void)
{
	is_red_or_blue();
	determine_ID();
	
	
	Client_Char_update();
	Client_Line_update();

}

