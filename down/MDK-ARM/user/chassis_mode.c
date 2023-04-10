#include "chassis_mode.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "math.h"
#include "can.h"
#include "cmsis_os.h"
#include "rc.h"
extern KEY	KEY_Date;

#define front_degree 1397.0f    //当yaw朝前的时候，yaw电机的ecd
#define cheassis_follow_speed 200.00f

int chassis_mode=cheassis_follow;
int Q_sign=1;
int E_sign=0;



void Function_Choose(void)
{
	if(STOP==1)
	{
		if(MODE==1 || MODE==3)  CtoG();   					//底盘跟随
		else if(MODE==2)
		{
			Circle(R_X,R_Y);		//小陀螺
		}
	}
	if(STOP==2)
	{
		if(KEY_Date.Q==1 && KEY_Date.E==0)
		{
			E_sign=0;
			Q_sign=1;
		}
		if(KEY_Date.E==1 && KEY_Date.Q==0)
		{
			Q_sign=0;
			E_sign=1;
		}
		if(Q_sign==1 &&E_sign==0)	
		{
			chassis_mode=cheassis_follow;
		}
		else if(E_sign==1 && Q_sign==0)
		{
			chassis_mode=Small_gyroscope;
		}

		if(chassis_mode==Small_gyroscope)
		{
			Circle(KEY_Date.R_X,KEY_Date.R_Y);					//小陀螺
		}	
		else
		{
			CtoG();   							//底盘跟随
		}
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
		if(ecd_diff>=50||ecd_diff<=-50)    Is_finishctog=0; //判断云台是否转动和底盘是否开始跟随
	}
	if(ecd_diff<=50&&ecd_diff>=-50) Is_finishctog=1;         //如果机械角度靠近开始跟随时的机械角度，则完成跟随

	if(Is_finishctog==0)      //底盘开始跟随，则进入调整
	{
		if(ecd_diff>=540) ecd_diff=540;         //跟随速度限幅
		else if(ecd_diff <=-540) ecd_diff=-540;
		
		wheel_moter[0].target_speed+=-((ecd_diff/cheassis_follow_speed)*1800.0f);
		wheel_moter[1].target_speed+=-((ecd_diff/cheassis_follow_speed)*1800.0f);
		wheel_moter[2].target_speed+=-((ecd_diff/cheassis_follow_speed)*1800.0f);
		wheel_moter[3].target_speed+=-((ecd_diff/cheassis_follow_speed)*1800.0f);
	}
}
/*************************************小陀螺**************************************/	
int ciecle_speed=5200;
void Circle(int16_t right_X,int16_t right_Y)
{
		//底盘坐标系的速度
		float V_x;
		float V_y;

		if(gimbal_yaw_motor.ecd<front_degree)
		{
			ecd_ch=8191.0f+gimbal_yaw_motor.ecd-front_degree;
		}
		else
		{
			ecd_ch=gimbal_yaw_motor.ecd-front_degree;  //机械角度化角度，角度化弧度
		}



		degree_ch=ecd_ch*(360.0f / 8191.0f);   //机械角度化角度	
		hudu=degree_ch*(3.14f / 180.0f);
		
		Xsin=right_X*sinf(hudu);         //分解
		Xcos=right_X*cosf(hudu);
		Ysin=right_Y*sinf(hudu);
		Ycos=right_Y*cosf(hudu);
		
		wheel_moter[0].target_speed=ciecle_speed;   //旋转量
		wheel_moter[1].target_speed=ciecle_speed;
		wheel_moter[2].target_speed=ciecle_speed;
		wheel_moter[3].target_speed=ciecle_speed;

		V_x=Xcos+Ysin;
		V_y=-Xsin+Ycos;
			
	wheel_moter[0].target_speed+=(float)(V_x-V_y);
	wheel_moter[1].target_speed+=(float)(V_x+V_y);
	wheel_moter[2].target_speed+=(float)(-V_x+V_y);
	wheel_moter[3].target_speed+=(float)(-V_x-V_y);

}

