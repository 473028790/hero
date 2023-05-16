#include "vision.h"
#include "usart.h"
extern UART_HandleTypeDef huart1;
uint8_t UART1_Rx_Buf[6];
extern float last_store[3];


typedef struct
{
	uint8_t Yaw_Low;
	uint8_t Yaw_High;
	uint8_t Pitch_Low;
	uint8_t Pitch_High;
	float Vision_Yaw;
	float Vision_Pitch;
}Vision_Data_Struct;

typedef union
{
	float f_data;
	uint8_t byte[4];
}float_data;


Vision_Data_Struct From_Vision_Data;

union
{
    float value;
    uint8_t sendbuf[4];
}vision_data_send[2];

Vision_Data_S Vision_Data;
Vision_Data_R Vision_Data_r;

float Vision_Coff=0.5;
uint16_t distance[3]={0};
void Vision_decode(uint8_t *data)
{
//	#if(ADJUST_MODE==4)
	if(*data==0x80 && (*(data+11))==0x7F)
	{
		float_data pitch_receive;
		float_data yaw_receive;
		for(int i=0;i<4;i++)
		{
			yaw_receive.byte[i]=*(data+1+i);
		}
			for(int i=0;i<4;i++)
		{
			pitch_receive.byte[i]=*(data+5+i);
		}
		distance[0] = *(data+9);
		distance[1] = *(data+10);
		distance[2] = distance[0] *100+distance[1];
		From_Vision_Data.Vision_Pitch =pitch_receive.f_data;
		From_Vision_Data.Vision_Yaw = yaw_receive.f_data;
	}
}
