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
#include "kalman.h"
#include "referee.h"
#include "Ranging.h"
extern int dial_mode;
extern ext_power_heat_data_t   power;
extern int infra_red_GPIO;
extern float last_store[3];
extern int16_t store[3];
float ch1,ch2,ch3,ch4,s1,s2;
uint8_t hi229_data;
float degree_k=0.01;   
int dial_red=0;

RC_Ctl_t RC_CtrlData;
KEY	KEY_Date;

void RemoteDataProcess(uint8_t *pData);
void (*dma_M0_rx_callback)(void);
void (*dma_M1_rx_callback)(void);
void KEY_chassis_max(void);

uint8_t STOP=3;//s1,1--control   2--KEY
uint8_t MODE=3;//s2
int16_t L_X;
int16_t L_Y;
int16_t R_Y;
int16_t R_X;

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
#define KEY_PRESSED_OFFSET_R ((uint32_t)0x0001<<8)
#define RC_FRAME_LENGTH 18u  

//??????
#define RC_CHANNAL_ERROR_VALUE 8000

#define GYR_ERROR_VALUE 1000
#define Friction_ERROR_VALUE 10000

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
    RC_CtrlData.rc.s1 = RC_SW_MID;
    RC_CtrlData.rc.s2 = RC_SW_MID;
    RC_CtrlData.mouse.x = 0;
    RC_CtrlData.mouse.y = 0;
    RC_CtrlData.mouse.z = 0;
    RC_CtrlData.mouse.press_l = 0;
    RC_CtrlData.mouse.press_r = 0;
    RC_CtrlData.key.v = 0;
    return 1;
}
uint8_t GYR_data_is_error(void)
{
    if (last_store[0]>GYR_ERROR_VALUE)
    {
        goto error;
    }
    if (last_store[2]>GYR_ERROR_VALUE)
    {
        goto error;
    }
    return 0;
		
	error:
		last_store[0]=0;
		last_store[2]=0;
		store[1]=0;
		store[1]=0;
    return 1;
}
uint8_t Friction_data_is_errorv(void)
{
    if (Friction_motor[0].ActualSpeed>Friction_ERROR_VALUE)
    {
        goto error;
    }
    if (Friction_motor[1].ActualSpeed>Friction_ERROR_VALUE)
    {
        goto error;
    }
    return 0;
		
	error:

    return 1;
}

void RC_data_init(void)
{
    RC_CtrlData.rc.ch0 = 0;
    RC_CtrlData.rc.ch1 = 0;
    RC_CtrlData.rc.ch2 = 0;
    RC_CtrlData.rc.ch3 = 0;
    RC_CtrlData.rc.s1 = RC_SW_MID;
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
int FRI_slove_sign=0;
void slove_FRI_lost(void)
{
    FRI_slove_sign=1;
}
//abs
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
float chasses_coefficient[2]={0.00f,0.00f};
void RemoteDataProcess(uint8_t *pData)
{
	if(pData == NULL)
	{
		return;
	}
	RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
	RC_CtrlData.rc.ch0 -=1024;
	if(RC_CtrlData.rc.ch0<10&&RC_CtrlData.rc.ch0>-10)
	{
		RC_CtrlData.rc.ch0=0;
	}
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
	RC_CtrlData.rc.ch1 -=1024;
	if(RC_CtrlData.rc.ch1<10&&RC_CtrlData.rc.ch1>-10)
	{
		RC_CtrlData.rc.ch1=0;
	}
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch2 -=1024;
	if(RC_CtrlData.rc.ch2<10&&RC_CtrlData.rc.ch2>-10)
	{
		RC_CtrlData.rc.ch2=0;
	}
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
	RC_CtrlData.rc.ch3 -=1024;
	if(RC_CtrlData.rc.ch3<10&&RC_CtrlData.rc.ch3>-10)
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
	RC_CtrlData.key.v = ((int16_t)pData[14])|((int16_t)pData[15] << 8);
	//your control code ��.

	STOP=RC_CtrlData.rc.s1;
	MODE=RC_CtrlData.rc.s2;


	//W
	KEY_Date.W=((RC_CtrlData.key.v)<<15);	
	KEY_Date.W=((KEY_Date.W)>>15);	
	//S
	KEY_Date.S=((RC_CtrlData.key.v)<<14);	
	KEY_Date.S=((KEY_Date.S)>>15);	
	//D
	KEY_Date.D=((RC_CtrlData.key.v)<<12);	
	KEY_Date.D=((KEY_Date.D)>>15);
	//A
	KEY_Date.A=((RC_CtrlData.key.v)<<13);	
	KEY_Date.A=((KEY_Date.A)>>15);
	//Q			????
	KEY_Date.Q=((RC_CtrlData.key.v)<<9);	
	KEY_Date.Q=((KEY_Date.Q)>>15);
	//E			???
	KEY_Date.E=((RC_CtrlData.key.v)<<8);	
	KEY_Date.E=((KEY_Date.E)>>15);
	//R			????
	KEY_Date.R=((RC_CtrlData.key.v & KEY_PRESSED_OFFSET_R) == KEY_PRESSED_OFFSET_R);
	
	//Shift	 
	KEY_Date.Shift=((RC_CtrlData.key.v)<<11);	
	KEY_Date.Shift=((KEY_Date.Shift)>>15);
	//Ctrl
	KEY_Date.Ctrl=((RC_CtrlData.key.v)<<10);	
	KEY_Date.Ctrl=((KEY_Date.Ctrl)>>15);
}
void RC_restart(uint16_t dma_buf_num)
{
	
	__set_FAULTMASK(1);
	NVIC_SystemReset();
	
	
}


void ReadRc_Chassis(void)
{
	float ch_middle[2];
	float KEY_middle[2];

	if(STOP==1)
	{
		//Key mouse
		KEY_Date.R_Y=0;
		KEY_Date.R_X=0;
		KEY_Date.L_X=0;
		KEY_Date.L_Y=0;

		//Remote control mode
		R_Y = RC_CtrlData.rc.ch1;
		ch_middle[1]=(((float)R_Y)/660)*Chassis_speed_proportion;
		R_Y=(int16_t)ch_middle[1];

		R_X = RC_CtrlData.rc.ch0;
		ch_middle[0]=(((float)R_X)/660)*Chassis_speed_proportion;
		R_X=(int16_t)ch_middle[0];
	}
	else if(STOP==2)
	{

		//Remote control mode
		R_X=0;
		R_Y=0;
		ch_middle[1]=0;
		ch_middle[0]=0;


		//Key mouse


		//W		
		if(KEY_Date.W==1)
		{
			chasses_coefficient[0]+=0.001f;

			if(KEY_Date.W==1  &&  KEY_Date.S==1)
			{
				chasses_coefficient[0]-=0.001f;
				if(chasses_coefficient[0]<0.1f && chasses_coefficient[0]>-0.1f)
				{
					chasses_coefficient[0]=0.0000f;
				}
			}
		}

		if(KEY_Date.W==0  &&  KEY_Date.S==0)
		{
			chasses_coefficient[0]=0.00000f;
		}

		KEY_chassis_max();//???????

		KEY_middle[1]=((float)chasses_coefficient[0])*Chassis_speed_proportion;
		KEY_Date.R_Y=(int16_t)KEY_middle[1];

		//S	
		if(KEY_Date.S==1)
		{
			chasses_coefficient[0]-=0.001f;

			if(KEY_Date.W==1  &&  KEY_Date.S==1)
			{
				if(chasses_coefficient[0]>0.0f) chasses_coefficient[0]-=0.001f;
				if(chasses_coefficient[0]<0.1f && chasses_coefficient[0]>-0.1f)
				{
					chasses_coefficient[0]=0.0000f;
				}
				if(chasses_coefficient[0]<0.0f) chasses_coefficient[0]+=0.002f;
			}

		}
		if(KEY_Date.W==0  &&  KEY_Date.S==0)
		{
			chasses_coefficient[0]=0.00000f;
		}
		KEY_chassis_max();//???????
		KEY_middle[1]=((float)chasses_coefficient[0])*Chassis_speed_proportion;
		KEY_Date.R_Y+=(int16_t)(KEY_middle[1]);

		//D
		if(KEY_Date.D==1)
		{
			chasses_coefficient[1]+=0.001f;

			if(KEY_Date.D==1  &&  KEY_Date.A==1)
			{
				chasses_coefficient[1]-=0.001f;
				if(chasses_coefficient[1]<0.1f && chasses_coefficient[1]>-0.1f)
				{
					chasses_coefficient[1]=0.0000f;
				}
			}
		}
		if(KEY_Date.A==0  &&  KEY_Date.D==0)
		{
			chasses_coefficient[1]=0.000000f;
		}
		KEY_chassis_max();//???????

		KEY_middle[0]=((float)chasses_coefficient[1])*Chassis_speed_proportion;
		KEY_Date.R_X+=(int16_t)(KEY_middle[0]);

		//A		
		if(KEY_Date.A==1)
		{
			chasses_coefficient[1]-=0.001f;

			if(KEY_Date.D==1  &&  KEY_Date.A==1)
			{
				if(chasses_coefficient[1]>0.0f) chasses_coefficient[1]-=0.001f;
				if(chasses_coefficient[1]<0.1f && chasses_coefficient[1]>-0.1f)
				{
					chasses_coefficient[1]=0.0000f;
				}
				if(chasses_coefficient[1]<0.0f) chasses_coefficient[1]+=0.002f;
			}

		}
		if(KEY_Date.D==0  &&  KEY_Date.A==0)
		{
			chasses_coefficient[1]=0.00000f;
		}
		KEY_chassis_max();//???????

		KEY_middle[0]=((float)chasses_coefficient[1])*Chassis_speed_proportion;
		KEY_Date.R_X=(int16_t)(KEY_middle[0]);

	}

	//????????????????????????
	int16_t right_X;
	int16_t right_Y;
	if(STOP==1)
	{
		right_X=R_X;
		right_Y=R_Y;
		/*
		if(MODE==3)
		{
			wheel_moter[0].target_speed=(float)(right_X-right_Y);
			wheel_moter[1].target_speed=(float)(right_X+right_Y);
			wheel_moter[2].target_speed=(float)(-right_X+right_Y);
			wheel_moter[3].target_speed=(float)(-right_X-right_Y);
		}
		if(MODE==1)
		{
			wheel_moter[0].target_speed=(float)1500;
			wheel_moter[1].target_speed=(float)1500;
			wheel_moter[2].target_speed=(float)1500;
			wheel_moter[3].target_speed=(float)1500;
		}
		if(MODE==2)
		{
			wheel_moter[0].target_speed=(float)-1500;
			wheel_moter[1].target_speed=(float)-1500;
			wheel_moter[2].target_speed=(float)-1500;
			wheel_moter[3].target_speed=(float)-1500;
		}
		*/
		
		if(MODE!=2)
		{
			wheel_moter[0].target_speed=(float)(-right_X+right_Y);
			wheel_moter[1].target_speed=(float)(-right_X-right_Y);
			wheel_moter[2].target_speed=(float)(right_X-right_Y);
			wheel_moter[3].target_speed=(float)(right_X+right_Y);
		}
	}

	if(STOP==2)
	{
		right_X=KEY_Date.R_X;
		right_Y=KEY_Date.R_Y;

			wheel_moter[0].target_speed=(float)(-right_X+right_Y);
			wheel_moter[1].target_speed=(float)(-right_X-right_Y);
			wheel_moter[2].target_speed=(float)(right_X-right_Y);
			wheel_moter[3].target_speed=(float)(right_X+right_Y);

	}	

}
	int16_t left_X;
	int16_t left_Y;
float x1=600,x2=400;
uint8_t ranging_com_flag=0;
extern uint8_t ranging_flag;
float Compensation_angle;
extern double ranging_x;
uint8_t ranging_finish_flag=0;
float ranging_yaw=0;
extern double distance_x;
extern double final_theta;

void ReadRc_Gimbal(void)
{
	static uint8_t shift_flag;
	static float shift_yaw;
	static float shift_pitch;
	static uint8_t shift_one_sign;
//	int16_t left_X;
//	int16_t left_Y;

	if(STOP==1)
	{
		L_Y = (int16_t)RC_CtrlData.rc.ch3;
		L_X = (int16_t)RC_CtrlData.rc.ch2;
		left_X=L_X;
		left_Y=L_Y;

		if((left_X>=-100&&left_X<=100)||left_X>700||left_X<-700)    left_X=0;
		if((left_Y>=-100&&left_Y<=100)||left_Y>700||left_Y<-700)   	left_Y=0;
		left_X*=0.06;
		left_Y*=0.006;
		yaw_outer_pid.target+=(degree_k*(-left_X));
		pitch_outer_pid.target+=(degree_k*(left_Y));
		if(yaw_outer_pid.target>179.9f)   yaw_outer_pid.target-=360.0f;
		else if (yaw_outer_pid.target<-179.9f)  yaw_outer_pid.target+=360.0f;	
		/*
		if(RC_CtrlData.rc.s2==1) 
		{
			pitch_outer_pid.target=15;
		}
		if(RC_CtrlData.rc.s2==3) 
		{
			pitch_outer_pid.target=0;
		}
		if(RC_CtrlData.rc.s2==2) 
		{
			pitch_outer_pid.target=-15;
		}
		*/
		/*
		if(RC_CtrlData.rc.s2==1) 
		{
			yaw_outer_pid.target=60;
		}
		if(RC_CtrlData.rc.s2==3) 
		{
			
			yaw_outer_pid.target=0;
		}
		if(RC_CtrlData.rc.s2==2) 
		{
			
			yaw_outer_pid.target=-60;
		}
		*/
	}
	if(STOP==2)
	{
		//mouse
		kalman mouse_x;
		kalman mouse_y;
		kalmanCreate(&mouse_x,600,400);
		kalmanCreate(&mouse_y,2000,3000);
		left_X = KalmanFilter(&mouse_x,RC_CtrlData.mouse.x); 
		left_Y = KalmanFilter(&mouse_y,RC_CtrlData.mouse.y); 

		//if((left_X>=-100&&left_X<=100)||left_X>700||left_X<-700)    left_X=0;
		//if((left_Y>=-100&&left_Y<=100)||left_Y>700||left_Y<-700)   	left_Y=0;
		left_X*=0.6;
		left_Y*=0.5;
		yaw_outer_pid.target+=(degree_k*(-left_X));
		pitch_outer_pid.target+=(degree_k*(-left_Y));
		if(yaw_outer_pid.target>179.9f)   yaw_outer_pid.target-=360.0f;
		else if (yaw_outer_pid.target<-179.9f)  yaw_outer_pid.target+=360.0f;	
			if(RC_CtrlData.mouse.press_r==1 && pitch_outer_pid.target!=Compensation_angle)
			{
				ranging_com_flag=1;
			}
			if(ranging_com_flag==1)
			{
				if(ranging_flag==1)
				{
					ranging_finish_flag++;
					ranging_yaw=yaw_outer_pid.target;
					//Ballistic_fitting
					//Compensation_angle=(0.26f * ((ranging_x/1000.0f)*(ranging_x/1000.0f)) - 0.77f * (ranging_x/1000.0f) + 5.63f)+pitch_outer_pid.target;
					//Ballistic_solution
					distance_x=ranging_x/1000;
					distance_test();
					Compensation_angle=((final_theta)/3.14*180)+pitch_outer_pid.target;
					if(ranging_finish_flag==1)
					{
						pitch_outer_pid.target=Compensation_angle;
						yaw_outer_pid.target=ranging_yaw-2.0f;
						ranging_com_flag=0;
						ranging_finish_flag=0;
					}
				}
			}
			if(KEY_Date.Ctrl==1)
			{
					pitch_outer_pid.target=Compensation_angle;
					yaw_outer_pid.target=ranging_yaw-1.0f;
			}
			
			if(KEY_Date.Shift==1)
			{
				shift_flag=1;
			}
			else
			{
				shift_flag=0;
			}
			if(shift_flag==1)
			{
				shift_one_sign++;
				if(shift_one_sign==1)
				{
					shift_yaw=yaw_outer_pid.target;
					shift_pitch=pitch_outer_pid.target;
				}
				yaw_outer_pid.target=shift_yaw;
				pitch_outer_pid.target=shift_pitch;
			}
			else if(shift_flag==0)
			{
				shift_one_sign=0;
				shift_flag=0;
			}
	}
}
int dial_number=0;
int dial_number1=0;
int dial_number2=0;
int dial_sign1=0;
int dial_sign_two=3;
int dial_sign=0;
int hero_shoot_number=0;
int infra_red_GPIO=3;
int infra_red_MODE=0;
int dial_red_sign=3;
void ReadRc_dial(void)
{
	dial_mode=infantry;
	infra_red_GPIO=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6);

	if(dial_mode==hero)
	{
		if(STOP==1)
		{
			
			if(MODE==1)
			{
				dial_number++;
				if(dial_number==1)
				{
					get_moto_offset(&dial_data);
				}
				if(dial_number>3) dial_number=3;
			}

			get_total_angle(&dial_data);

			if(MODE==3)
			{
				//dial_finish();
				dial_number=0;
				dial_data.total_angle=0;
				dial_data.angle_first=0;
				dial_data.angle_set=0;
			}
		}

		if(STOP==2)
		{
			if(RC_CtrlData.mouse.press_l==1)
			{
				dial_red_sign=1;
			}
			if(dial_red_sign==1)
			{
				if(infra_red_GPIO==0)
				{
					dial_sign1=1;
					if(dial_sign_two==1)
					{
						dial_sign1=0;
						dial_red_sign=0;
						dial_number1=0;
						dial_sign_two=0;
			
						dial_data.total_angle=0;
						dial_data.angle_first=0;
						dial_data.angle_set=0;
					}
				}
				else if(infra_red_GPIO==1)
				{
					if(dial_sign1==1)
					{
						dial_sign_two=1;
					}
				}
			}
			if(dial_sign1==1)
			{
				dial_number1++;
				if(dial_number1==1)
				{
					get_moto_offset(&dial_data);
				}
				if(dial_number1>3) dial_number1=3;

				get_total_angle(&dial_data);
			}
		}

		}

	if(dial_mode==infantry)
	{
		if(STOP==1)
		{
      if(MODE==1)
			{
				dial_red++;
				if(dial_red==1)
				{
					infra_red_MODE=1;
				}
				if(infra_red_MODE==1)
				{
					if(infra_red_GPIO==0)
					{
						dial_motor.target_speed=2400;
						if(infra_red_GPIO==1)
						{
							dial_motor.target_speed=0;
							infra_red_MODE=2;
						}
					}
					else if(infra_red_GPIO==1)
					{
						dial_motor.target_speed=0;
						infra_red_MODE=2;
					}
				}
				
				if(infra_red_MODE==2)
				{
					if(infra_red_GPIO==1)
					{
						dial_motor.target_speed=3000;
					}
					else if(infra_red_GPIO==0)
					{
						dial_motor.target_speed=0;
						infra_red_MODE=0;
					}
				}
			}
			
			if(MODE==3)
			{
				dial_red=0;
				dial_motor.target_speed=0;
				infra_red_MODE=0;
			}
		}
		if(STOP==2)
		{
			//shoot_number=determine_shoot();
			//if(RC_CtrlData.mouse.press_l==1 && shoot_number!=0)
			
			if(RC_CtrlData.mouse.press_l==1)
			{
				dial_sign=1;
			}
			if(dial_sign==1)
			{
				dial_red++;
				if(dial_red==1)
				{
					infra_red_MODE=1;
				}
				if(infra_red_MODE==1)
				{
					if(infra_red_GPIO==0)
					{
						dial_motor.target_speed=2000;
						if(infra_red_GPIO==1)
						{
							dial_motor.target_speed=0;
							infra_red_MODE=2;
						}
					}
					else if(infra_red_GPIO==1)
					{
						dial_motor.target_speed=0;
						infra_red_MODE=2;
					}
				}
				
				if(infra_red_MODE==2)
				{
					if(infra_red_GPIO==1)
					{
						dial_motor.target_speed=2000;
					}
					else if(infra_red_GPIO==0)
					{
						dial_motor.target_speed=0;
						infra_red_MODE=0;
						
					}
				}
				

			}
			if(RC_CtrlData.mouse.press_l==1)
			{
				dial_red=0;
				dial_motor.target_speed=0;
				infra_red_MODE=0;
			}

		}
	}
	

}
void KEY_chassis_max()
{
	int wheel_number=0;
	for(wheel_number=0;wheel_number<2;wheel_number++)
	{
			if(chasses_coefficient[wheel_number]>=wheel_KEY_speed_max)
			{
				chasses_coefficient[wheel_number]=1;
			}
			if(chasses_coefficient[wheel_number]<=-wheel_KEY_speed_max)
			{
				chasses_coefficient[wheel_number]=-1;
			}
	}
}


