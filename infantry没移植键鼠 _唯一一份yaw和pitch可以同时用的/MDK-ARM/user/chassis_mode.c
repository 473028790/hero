#include "chassis_mode.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "math.h"
#include "can.h"
#include "cmsis_os.h"
#include "rc.h"
int16_t Is_ny=0;
#define YAW         1
#define PITCH       0

#define front_degree 7720.0f    //当yaw朝前的时候，yaw电机的ecd


void Function_Choose(void)
{
	if(STOP==3)
	{
		int16_t gb=R_Y;
		int16_t lr=R_X;
		if(MODE==3)  CtoG();   				//底盘跟随
		else if(MODE==2)  Circle(lr,gb);		//小陀螺
		//shoot[2].target=1500;
	}
}
/*************************************底盘跟随**************************************/
float Xsin,Xcos,Ysin,Ycos;
float hudu;
float ecd_ch;
float ecd_ch1;
float degree_ch;
float hudu_ch;
float Is_con;
float ecd_diff;
int16_t Is_finishctog=1;
void CtoG()
{
		ecd_diff=front_degree-gimbal_yaw_motor.ecd;								//参考机械角度与实际角度的差值
	
	  if(ecd_diff>4095)    ecd_diff-=8192;     //边界处理
		else if(ecd_diff<-4095)  	ecd_diff+=8192;
		
		if(Is_finishctog==1)
		{
			if(ecd_diff>=5||ecd_diff<=-5)    Is_finishctog=0; //判断云台是否转动和底盘是否开始跟随
		}
		if(ecd_diff<=5&&ecd_diff>=-5) Is_finishctog=1;         //如果机械角度靠近开始跟随时的机械角度，则完成跟随
		
		if(Is_finishctog==0)      //底盘开始跟随，则进入调整
		{
			if(ecd_diff>=540) ecd_diff=540;         //跟随速度限幅
			else if(ecd_diff <=-540) ecd_diff=-540;
			
			wheel_moter[0].target_speed+=ecd_diff*5;
			wheel_moter[1].target_speed+=ecd_diff*5;
			wheel_moter[2].target_speed+=ecd_diff*5;
			wheel_moter[3].target_speed+=ecd_diff*5;
		}
}
/*************************************小陀螺**************************************/	
void Circle(int16_t right_Y,int16_t right_X)
{
		ecd_ch1=gimbal_yaw_motor.llast_ecd-gimbal_yaw_motor.ecd;  //机械角度化角度，角度化弧度
		if(ecd_ch1>4095.9f)  ecd_ch1-=8192;     //边界处理
		if(ecd_ch1<-4095.9f) ecd_ch1+=8192;
		
		ecd_ch=ecd_ch1;
		degree_ch=ecd_ch*(360.0f / 8191.0f);   //机械角度化角度
		degree_ch=-degree_ch;
		hudu=degree_ch*(3.1415f / 180.0f);
		
		Xsin=right_X*sinf(hudu);         //分解
		Xcos=right_X*cosf(hudu);
		Ysin=right_Y*sinf(hudu);
		Ycos=right_Y*cosf(hudu);
		
		wheel_moter[0].target_speed=2000;   //旋转量
		wheel_moter[1].target_speed=2000;
		wheel_moter[2].target_speed=2000;
		wheel_moter[3].target_speed=2000;
	
        wheel_moter[0].target_speed+=Xcos-Ysin+Xsin+Ycos;
        wheel_moter[1].target_speed+=-Xcos-Ysin+Xsin+Ycos;
        wheel_moter[2].target_speed+=Xcos-Ysin-Xsin+Ycos;
        wheel_moter[3].target_speed+=-Xcos-Ysin-Xsin+Ycos;
}
