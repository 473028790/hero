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
extern int dial_mode;
float ch1,ch2,ch3,ch4,s1,s2;
uint8_t hi229_data;
float degree_k=0.01;   

RC_Ctl_t RC_CtrlData;
KEY	KEY_Date;

void RemoteDataProcess(uint8_t *pData);
void (*dma_M0_rx_callback)(void);
void (*dma_M1_rx_callback)(void);
void KEY_chassis_max(void);

uint8_t STOP;//s1,1--control   2--KEY
uint8_t MODE;//s2
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
#define RC_FRAME_LENGTH 18u  

//??????
#define RC_CHANNAL_ERROR_VALUE 8000

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
	if(RC_CtrlData.rc.ch0<5&&RC_CtrlData.rc.ch0>-5)
	{
		RC_CtrlData.rc.ch0=0;
	}
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
	RC_CtrlData.rc.ch1 -=1024;
	if(RC_CtrlData.rc.ch1<5&&RC_CtrlData.rc.ch1>-5)
	{
		RC_CtrlData.rc.ch1=0;
	}
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch2 -=1024;
	if(RC_CtrlData.rc.ch2<5&&RC_CtrlData.rc.ch2>-5)
	{
		RC_CtrlData.rc.ch2=0;
	}
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
	RC_CtrlData.rc.ch3 -=1024;
	if(RC_CtrlData.rc.ch3<5&&RC_CtrlData.rc.ch3>-5)
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
	//your control code ….

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
	KEY_Date.E=((RC_CtrlData.key.v)>>7);


	//??????		
	//Shift	 
	KEY_Date.Shift=((RC_CtrlData.key.v)<<11);	
	KEY_Date.Shift=((KEY_Date.Shift)>>15);
	//Ctrl
	KEY_Date.Ctrl=((RC_CtrlData.key.v)<<10);	
	KEY_Date.Ctrl=((KEY_Date.Ctrl)>>15);

}
void RC_restart(uint16_t dma_buf_num)
{
	/*
	__set_FAULTMASK(1);
	NVIC_SystemReset();
	*/
}
int Chassis_speed_proportion=4000; //右边控制底盘最大速度的比例
int chassis_cnt=0;
void ReadRc_Chassis(void)
{
	float ch_middle[2];
	float KEY_middle[2];

	if(STOP==1)
	{
		chassis_cnt++;
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
//				chasses_coefficient[0]-=0.001f;
//				if(chasses_coefficient[0]<0.1f && chasses_coefficient[0]>-0.1f)
//				{
					chasses_coefficient[0]=0.0000f;
//				}
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
//				if(chasses_coefficient[0]>0.0f) chasses_coefficient[0]-=0.001f;
//				if(chasses_coefficient[0]<0.1f && chasses_coefficient[0]>-0.1f)
//				{
					chasses_coefficient[0]=0.0000f;
//				}
//				if(chasses_coefficient[0]<0.0f) chasses_coefficient[0]+=0.002f;
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
//				chasses_coefficient[1]-=0.001f;
//				if(chasses_coefficient[1]<0.1f && chasses_coefficient[1]>-0.1f)
//				{
					chasses_coefficient[1]=0.0000f;
//				}
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
//				if(chasses_coefficient[1]>0.0f) chasses_coefficient[1]-=0.001f;
//				if(chasses_coefficient[1]<0.1f && chasses_coefficient[1]>-0.1f)
//				{
					chasses_coefficient[1]=0.0000f;
//				}
//				if(chasses_coefficient[1]<0.0f) chasses_coefficient[1]+=0.002f;
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
			wheel_moter[0].target_speed=(float)(right_X-right_Y);
			wheel_moter[1].target_speed=(float)(+right_X+right_Y);
			wheel_moter[2].target_speed=(float)(-right_X+right_Y);
			wheel_moter[3].target_speed=(float)(-right_X-right_Y);
		}
		
	}

	if(STOP==2)
	{
		right_X=KEY_Date.R_X;
		right_Y=KEY_Date.R_Y;

			wheel_moter[0].target_speed=(float)(right_X-right_Y);
			wheel_moter[1].target_speed=(float)(+right_X+right_Y);
			wheel_moter[2].target_speed=(float)(-right_X+right_Y);
			wheel_moter[3].target_speed=(float)(-right_X-right_Y);

	}	

}
void ReadRc_Gimbal(void)
{
	int16_t left_X;
	int16_t left_Y;

	if(STOP==1)
	{
		L_Y = (int16_t)RC_CtrlData.rc.ch3;
		L_X = (int16_t)RC_CtrlData.rc.ch2;
		left_X=L_X;
		left_Y=L_Y;

		if((left_X>=-100&&left_X<=100)||left_X>700||left_X<-700)    left_X=0;
		if((left_Y>=-100&&left_Y<=100)||left_Y>700||left_Y<-700)   	left_Y=0;
		left_X*=0.03;
		left_Y*=0.006;
		yaw_outer_pid.target+=(degree_k*(-left_X));
		pitch_outer_pid.target+=(degree_k*(left_Y));
		if(yaw_outer_pid.target>179.9f)   yaw_outer_pid.target-=360.0f;
		else if (yaw_outer_pid.target<-179.9f)  yaw_outer_pid.target+=360.0f;	
		/*
		if(RC_CtrlData.rc.s2==1) 
		{
			pitch_outer_pid.target=10;
		}
		if(RC_CtrlData.rc.s2==3) 
		{
			pitch_outer_pid.target=0;
		}
		if(RC_CtrlData.rc.s2==2) 
		{
			pitch_outer_pid.target=-10;
		}
		*/
	}
	if(STOP==2)
	{
		//mouse
		kalman mouse_x;
		kalman mouse_y;
		kalmanCreate(&mouse_x,100,300);
		kalmanCreate(&mouse_y,200,300);
		left_X = KalmanFilter(&mouse_x,RC_CtrlData.mouse.x); 
		left_Y = KalmanFilter(&mouse_y,RC_CtrlData.mouse.y); 

		//if((left_X>=-100&&left_X<=100)||left_X>700||left_X<-700)    left_X=0;
		//if((left_Y>=-100&&left_Y<=100)||left_Y>700||left_Y<-700)   	left_Y=0;
		left_X*=0.4;
		left_Y*=0.2;
		yaw_outer_pid.target+=(degree_k*(-left_X));
		pitch_outer_pid.target+=(degree_k*(-left_Y));
		if(yaw_outer_pid.target>179.9f)   yaw_outer_pid.target-=360.0f;
		else if (yaw_outer_pid.target<-179.9f)  yaw_outer_pid.target+=360.0f;	

	}
}
int dial_number=0;
int dial_number1=0;
int dial_number2=0;
int dial_sign=0;
int dial_back_sign=0;
int shoot_number=0;
int infra_red_GPIO=3;
int infra_red_MODE=0;
int dial_red=0;
void ReadRc_dial(void)
{
	//infra_red_GPIO=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7);
	dial_mode=dial_mode_choose;//??????

/*
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
			shoot_number=determine_shoot();
			//if(RC_CtrlData.mouse.press_l==1 && shoot_number!=0)
			
			if(RC_CtrlData.mouse.press_l==1)
			{
				dial_sign=1;
			}
			if(dial_sign==1)
			{
				dial_number1++;
				if(dial_number1==1)
				{
					get_moto_offset(&dial_data);
				}
				if(dial_number1>3) dial_number1=3;

				get_total_angle(&dial_data);

			}





			if(RC_CtrlData.mouse.press_r==1)
			{
				dial_back_sign=1;
			}
			if(dial_back_sign==1)
			{
				dial_number2++;
				if(dial_number2==1)
				{
					get_back_offset(&dial_data);
				}
				if(dial_number2>3) dial_number2=3;

				get_total_angle(&dial_data);

			}


		}
*/
	if(dial_mode==infantry)
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
			
			if(MODE==3)
			{
				dial_red=0;
				dial_motor.target_speed=0;
				infra_red_MODE=0;
			}
		/*
      if(MODE==1)
      { 
				if(infra_red_GPIO==1)
				{
					dial_motor.target_speed=2000;
				}
				if(infra_red_GPIO==0)
				{
					dial_motor.target_speed=0;
				}
      }
	  if(MODE==3)
      {
				if(infra_red_GPIO==0)
				{
					dial_motor.target_speed=2000;
				}
				if(infra_red_GPIO==1)
				{
					dial_motor.target_speed=0;
				}
      }
			*/
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

