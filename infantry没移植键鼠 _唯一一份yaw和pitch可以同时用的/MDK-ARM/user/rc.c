#include "rc.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "usart.h"
#include "main.h"
//#include "packet.h"
#include "can.h"
#include "pid.h"
#include "tim.h"
extern int dial_mode;

float ch1,ch2,ch3,ch4,s1,s2;
uint8_t hi229_data;
float degree_k=0.01;   

RC_Ctl_t RC_CtrlData;

void RemoteDataProcess(uint8_t *pData);
void (*dma_M0_rx_callback)(void);
void (*dma_M1_rx_callback)(void);

uint8_t STOP;//s1
uint8_t MODE;//s2
int16_t L_X;//?????
int16_t L_Y;//?????
int16_t R_Y;//?????
int16_t R_X;//?????
int16_t x,v;
int i;
/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)
/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01<<5)
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01<<6)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01<<7)
#define RC_FRAME_LENGTH 18u  

//?????????????????
#define RC_CHANNAL_ERROR_VALUE 8000
//????
static int16_t RC_abs(int16_t value);

unsigned char sbus_rx_buffer[2][RC_FRAME_LENGTH]; //double sbus rx buffer to save data


uint8_t RC_data_is_error(void)
{   
    if (RC_abs(RC_CtrlData.rc.ch0) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_abs(RC_CtrlData.rc.ch1) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_abs(RC_CtrlData.rc.ch2) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_abs(RC_CtrlData.rc.ch3) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_CtrlData.rc.s1 == 0)
    {
        goto error;
    }
    if (RC_CtrlData.rc.s2 == 0)
    {
        goto error;
    }
    return 0;

error:
    RC_CtrlData.rc.ch0 = 0;
    RC_CtrlData.rc.ch1 = 0;
    RC_CtrlData.rc.ch2 = 0;
    RC_CtrlData.rc.ch3 = 0;
    RC_CtrlData.rc.s1 = RC_SW_UP;
    RC_CtrlData.rc.s2 = RC_SW_MID;
    RC_CtrlData.mouse.x = 0;
    RC_CtrlData.mouse.y = 0;
    RC_CtrlData.mouse.z = 0;
    RC_CtrlData.mouse.press_l = 0;
    RC_CtrlData.mouse.press_r = 0;
    RC_CtrlData.key.v = 0;
    return 1;
}
void RC_data_init(void)
{
    RC_CtrlData.rc.ch0 = 0;
    RC_CtrlData.rc.ch1 = 0;
    RC_CtrlData.rc.ch2 = 0;
    RC_CtrlData.rc.ch3 = 0;
    RC_CtrlData.rc.s1 = RC_SW_UP;
    RC_CtrlData.rc.s2 = RC_SW_MID;
    RC_CtrlData.mouse.x = 0;
    RC_CtrlData.mouse.y = 0;
    RC_CtrlData.mouse.z = 0;
    RC_CtrlData.mouse.press_l = 0;
    RC_CtrlData.mouse.press_r = 0;
    RC_CtrlData.key.v = 0;
}
void slove_RC_offline(void)
{
	RC_data_init();
}
void slove_RC_lost(void)
{
    RC_restart(SBUS_RX_BUF_NUM);
}
void slove_data_error(void)
{
    RC_restart(SBUS_RX_BUF_NUM);
}

//????
static int16_t RC_abs(int16_t value)
{
    if (value > 0)
    {
        return value;
    }
    else
    {
        return -value;
    }
}

void RemoteDataProcess(uint8_t *pData)
{
	float ch_middle[2];
	if(pData == NULL)
	{
		return;
	}
	RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
	RC_CtrlData.rc.ch0 -=1024;
	if(RC_CtrlData.rc.ch0<12&&RC_CtrlData.rc.ch0>-12)
	{
		RC_CtrlData.rc.ch0=0;
	}
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
	RC_CtrlData.rc.ch1 -=1024;
	if(RC_CtrlData.rc.ch1<12&&RC_CtrlData.rc.ch1>-12)
	{
		RC_CtrlData.rc.ch1=0;
	}
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch2 -=1024;
	if(RC_CtrlData.rc.ch2<12&&RC_CtrlData.rc.ch2>-12)
	{
		RC_CtrlData.rc.ch2=0;
	}
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
	RC_CtrlData.rc.ch3 -=1024;
	if(RC_CtrlData.rc.ch3<12&&RC_CtrlData.rc.ch3>-12)
	{
		RC_CtrlData.rc.ch3=0;
	}

	RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);
	RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
	RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
	RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8); 
	RC_CtrlData.mouse.press_l = pData[12];
	RC_CtrlData.mouse.press_r = pData[13];
	RC_CtrlData.key.v = ((int16_t)pData[14]);// | ((int16_t)pData[15] << 8);
	//your control code бн.

	STOP=RC_CtrlData.rc.s1;
	MODE=RC_CtrlData.rc.s2;
	L_Y = (int16_t)RC_CtrlData.rc.ch3;
	L_X = (int16_t)RC_CtrlData.rc.ch2;
	R_Y = RC_CtrlData.rc.ch1;
	ch_middle[1]=(((float)R_Y)/660)*Chassis_speed_proportion;
	R_Y=(int16_t)ch_middle[1];


	R_X = RC_CtrlData.rc.ch0;
	ch_middle[0]=(((float)R_X)/660)*Chassis_speed_proportion;
	R_X=(int16_t)ch_middle[0];

	x=RC_CtrlData.mouse.x;
	v=RC_CtrlData.key.v;
}
void RC_restart(uint16_t dma_buf_num)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}


void ReadRc_Chassis(void)
{
	
	int16_t right_X=R_X;
	int16_t right_Y=R_Y;

	/*
	if((right_X>=-200&&right_X<=200)||right_X>8000||right_X<-8000)    			right_X=0;
	if((right_Y>=-200&&right_Y<=200)||right_Y>8000||right_Y<-8000)    			right_Y=0;
	*/

	wheel_moter[0].target_speed=right_X+right_Y;
	wheel_moter[1].target_speed=-right_X+right_Y;
	wheel_moter[2].target_speed=right_X-right_Y;
	wheel_moter[3].target_speed=-right_X-right_Y;
	
	if(RC_CtrlData.rc.s2==1) 
	{
		wheel_moter[3].target_speed=3000;
	}
	if(RC_CtrlData.rc.s2==3)
	{
		wheel_moter[3].target_speed=0;

	}

	
}
void ReadRc_Gimbal(void)
{
	
	int16_t left_X=L_X;
	int16_t left_Y=L_Y;
	if((left_X>=-100&&left_X<=100)||left_X>700||left_X<-700)    left_X=0;
	if((left_Y>=-100&&left_Y<=100)||left_Y>700||left_Y<-700)   	left_Y=0;
	left_X*=0.02;
	left_Y*=0.006;
	yaw_outer_pid.target+=(degree_k*(-left_X));
	pitch_outer_pid.target+=(degree_k*left_Y);
	if(yaw_outer_pid.target>179.9f)   yaw_outer_pid.target-=360.0f;
	else if (yaw_outer_pid.target<-179.9f)  yaw_outer_pid.target+=360.0f;	
	/*
	if(RC_CtrlData.rc.s2==1) 
	{
		pitch_outer_pid.target=-10;

	}
	if(RC_CtrlData.rc.s2==3) 
	{
		pitch_outer_pid.target=0;

	}
	if(RC_CtrlData.rc.s2==2) 
	{
		pitch_outer_pid.target=10;

	}
	*/
	
}
int dial_number=0;
void ReadRc_dial(void)
{
	dial_mode=dial_mode_choose;//????

	if(dial_mode==hero)
	{
		if(MODE==1&&dial_data.state==0)									
		{
			dial_number++;
			dial_data.state=1;
			if(dial_number==1)
			{
				get_moto_offset(&dial_data);		
			}
			if(dial_number>3) dial_number=3;
		}

		if(dial_data.state==1)						
		{
			get_total_angle(&dial_data);
		}
		if(MODE==3&&dial_data.state==0)
		{
			dial_number=0;
			dial_data.total_angle=0;
			dial_data.angle_first=0;
			dial_data.angle_set=0;
			dial_data.last_angle=0;
			dial_data.llast_angle=0;
			dial_pid_init();
		}
	}
	if(dial_mode==infantry)
	{
      if(MODE==2)
      {
        dial_motor.target_speed=4000;
      }
	  if(MODE==3)
      {
        dial_motor.target_speed=0;
      }
	}


}

