#include "main.h"
#include "referee.h"
#include "usart.h"
#include "string.h"


uint16_t HP_percen = 100;

frame_header_t 					shead_data;//帧头
ext_game_robot_HP_t			HP;//血量
ext_bullet_remaining_t  remain;//剩余弹量
ext_game_robot_status_t My_status;//自身状态
ext_power_heat_data_t   power;
ext_shoot_data_t 				shoot_data;
ext_robot_hurt_t        hurt_data;
ext_event_data_t				Event_data;
unsigned char judge_buffer[JUDGE_LENGTH];

int data_length = 0;
int data_fulllength = 0;
JUDGE_HEADER judge_header;
#define BLUE  0
#define RED   1
uint8_t Judge_Self_ID;//当前机器人的ID
uint16_t Judge_SelfClient_ID;//发送者机器人对应的客户端ID
ext_SendClientData_t      ShowData;			//客户端信息

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

/**
  * @brief  判断当前热量剩余能否发子弹
  * @param  int
  * @retval 
  * @attention  数据打包,打包完成后通过串口发送到裁判系统
  */
 int determine_shoot(void)
 {
	int number=0;
	number=((My_status.shooter_id1_42mm_cooling_limit)-(power.shooter_id1_42mm_cooling_heat))/100;
	return number;
 }


int cnt22=0;
#define send_max_len     200
int UI_start_X=0;
int UI_start_Y=0;
 
int UI_finish_X=1000;
int UI_finish_Y=1000;
uint8_t CliendTxBuffer[send_max_len];
 
 
void JUDGE_Show_Data(void)
{
	cnt22++;
	static uint8_t datalength,i;
	
	determine_ID();//判断发送者ID和其对应的客户端ID
	
	ShowData.txFrameHeader.SOF = 0xA5;
	ShowData.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t);
	ShowData.txFrameHeader.Seq = 0;
	memcpy(CliendTxBuffer, &ShowData.txFrameHeader, sizeof(xFrameHeader));//写入帧头数据
	ref_append_crc8(CliendTxBuffer, sizeof(xFrameHeader));//写入帧头CRC8校验码
	
	ShowData.CmdID = 0x0301;
	
	ShowData.dataFrameHeader.data_cmd_id = 0x0101;//发给客户端的cmd,官方固定
	//ID已经是自动读取的了
	ShowData.dataFrameHeader.send_ID 	 = Judge_Self_ID;//发送者的ID
	ShowData.dataFrameHeader.receiver_ID = Judge_SelfClient_ID;//客户端的ID，只能为发送者机器人对应的客户端
	
	/*- 自定义内容 -*/
	ShowData.clientData.graphic_name[0] = 0;
	ShowData.clientData.graphic_name[1] = 0;
	ShowData.clientData.graphic_name[2] = 1;

	ShowData.clientData.operate_tpye=1;//图形操作
	ShowData.clientData.graphic_tpye=0;//图形类型
	ShowData.clientData.layer=0	  	  ;//图层数
	ShowData.clientData.color=6	  	  ;//颜色
	ShowData.clientData.start_angle=60;//起始角度
	ShowData.clientData.end_angle=260 ;//结束角度
	ShowData.clientData.width=6 	  ;//宽度
	ShowData.clientData.start_x=UI_start_X 	  ;//起点 x 坐标；
	ShowData.clientData.start_y=UI_start_Y 	  ;//起点 y 坐标；
	ShowData.clientData.radius=60     ;//字体大小或者半径；
	ShowData.clientData.end_x=UI_finish_X 	  ;//终点 x 坐标；
	ShowData.clientData.end_y=UI_finish_Y 	  ;//终点 y 坐标；

	//打包写入数据段
	memcpy(	
			CliendTxBuffer + 5, 
			(uint8_t*)&ShowData.CmdID, 
			(sizeof(ShowData.CmdID)+ sizeof(ShowData.dataFrameHeader)+ sizeof(ShowData.clientData))
		  );			
			
	ref_append_crc16(CliendTxBuffer,sizeof(ShowData));//写入数据段CRC16校验码	

	datalength = sizeof(ShowData); 
	for(i = 0;i < datalength;i++)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)&CliendTxBuffer[i],200,0x10);
		while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET);
	}	 
}

void referee_task(void)
{
	is_red_or_blue();
	determine_ID();
	JUDGE_Show_Data();
}