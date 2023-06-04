#include "chassis_mode.h"
#include "FreeRTOS.h"
#include "main.h"
#include "math.h"
#include "can.h"
#include "cmsis_os.h"
#include "rc.h"
extern KEY	KEY_Date;

float front_degree=1335.0f;    //当yaw朝前的时候，yaw电机的ecd

#define  front_degree1 1335.0f    //当yaw朝前的时候，yaw电机的ecd

#define cheassis_follow_speed 600.00f

int chassis_mode=cheassis_follow;
int Q_sign=1;
int E_sign=0;
uint8_t R_sign=0;
extern int16_t R_Y;
extern int16_t R_X;

void Function_Choose(void)
{
	if(STOP==1)
	{
		if(MODE==1 || MODE==3)  CtoG(R_X,R_Y);   					//底盘跟随
		else if(MODE==2)
		{
			Circle(R_X,R_Y);		//小陀螺
		}
	}
	if(STOP==2)
	{
		if(KEY_Date.Q==1 && KEY_Date.E==0 && KEY_Date.R==0) 
		{
			E_sign=0;
			Q_sign=1;
			R_sign=0;
		}
		if(KEY_Date.E==1 && KEY_Date.Q==0 && KEY_Date.R==0)
		{
			Q_sign=0;
			E_sign=1;
			R_sign=0;
		}
		if(KEY_Date.E==0 && KEY_Date.Q==0 && KEY_Date.R==1)
		{
			Q_sign=0;
			E_sign=0;
			R_sign=1;
		}
		
		if(Q_sign==1 &&E_sign==0 && R_sign==0)	
		{
			chassis_mode=cheassis_follow;
			front_degree=1335.0f;
		}
		else if(E_sign==1 && Q_sign==0 && R_sign==0)
		{
			chassis_mode=Small_gyroscope;
		}
		if(Q_sign==0 &&E_sign==0 && R_sign==1)	
		{
			chassis_mode=cheassis_follow;
			front_degree=2316.0f;
		}

		if(chassis_mode==Small_gyroscope)
		{
			Circle(KEY_Date.R_X,KEY_Date.R_Y);					//小陀螺
		}	
		else
		{
			CtoG((KEY_Date.R_X),(KEY_Date.R_Y)); 							//底盘跟随
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
float cheassis_speed_ratio;
int16_t Is_finishctog=1;

void CtoG(int16_t right_X,int16_t right_Y)
{
	wheel_moter[0].target_speed=0.0f;
	wheel_moter[1].target_speed=0.0f;
	wheel_moter[2].target_speed=0.0f;
	wheel_moter[3].target_speed=0.0f;
	
	
	ecd_diff=front_degree-gimbal_yaw_motor.ecd;								//参考机械角度与实际角度的差值
 
	if(ecd_diff>4095)    ecd_diff-=8192;     //边界处理
	else if(ecd_diff<-4095)  	ecd_diff+=8192;
	
	cheassis_speed_ratio=(ecd_diff/cheassis_follow_speed);
	if(cheassis_speed_ratio<=0.26f && cheassis_speed_ratio>=-0.26f) cheassis_speed_ratio=0;
	if(cheassis_speed_ratio>3.0f) cheassis_speed_ratio=3.0f;
	if(cheassis_speed_ratio<-3.0f) cheassis_speed_ratio=-3.0f;
	wheel_moter[0].target_speed+=-(cheassis_speed_ratio*1200.0f);
	wheel_moter[1].target_speed+=-(cheassis_speed_ratio*1200.0f);
	wheel_moter[2].target_speed+=-(cheassis_speed_ratio*1200.0f);
	wheel_moter[3].target_speed+=-(cheassis_speed_ratio*1200.0f);
	
		//底盘坐标系的速度
		float V_x;
		float V_y;

		if(gimbal_yaw_motor.ecd<front_degree1)
		{
			ecd_ch=8191.0f+gimbal_yaw_motor.ecd-front_degree1;
		}
		else
		{
			ecd_ch=gimbal_yaw_motor.ecd-front_degree1;  //机械角度化角度，角度化弧度
		}



		degree_ch=ecd_ch*(360.0f / 8191.0f);   //机械角度化角度	
		hudu=degree_ch*(3.14f / 180.0f);
		
		Xsin=right_X*sin(hudu);         //分解
		Xcos=right_X*cos(hudu);
		Ysin=right_Y*sin(hudu);
		Ycos=right_Y*cos(hudu);

		V_x=-(Xcos+Ysin);
		V_y=-(-Xsin+Ycos);
			
	wheel_moter[0].target_speed+=(float)(V_x-V_y);
	wheel_moter[1].target_speed+=(float)(V_x+V_y);
	wheel_moter[2].target_speed+=(float)(-V_x+V_y);
	wheel_moter[3].target_speed+=(float)(-V_x-V_y);
	

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

		V_x=-(Xcos+Ysin);
		V_y=-(-Xsin+Ycos);
			
	wheel_moter[0].target_speed+=(float)(V_x-V_y);
	wheel_moter[1].target_speed+=(float)(V_x+V_y);
	wheel_moter[2].target_speed+=(float)(-V_x+V_y);
	wheel_moter[3].target_speed+=(float)(-V_x-V_y);

}

