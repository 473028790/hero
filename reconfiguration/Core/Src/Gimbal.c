#include "Gimbal.h"
#include "rc.h"
#include "kalman.h"
#include "pid.h"
#include "variable_config.h"
#include "imu_data_decode.h"
extern uint8_t STOP;//s1,1--control   2--KEY
extern uint8_t MODE;//s2
extern RC_Ctl_t RC_CtrlData;
extern PID_t yaw_outer_pid;
extern PID_t yaw_inner_pid;
extern PID_t pitch_outer_pid;
extern PID_t pitch_inner_pid;
imu_data_t Info_imu;
//static float yaw_out_targer,pitch_out_targer;
static float yaw_out_targer,pitch_out_targer;
/*Gimbal target value*/
void ReadRc_Gimbal(void)
{
	static int16_t left_X;
	static int16_t left_Y;

	if(STOP==RC_mode)
	{
		L_Y = (int16_t)RC_CtrlData.rc.ch3;
		L_X = (int16_t)RC_CtrlData.rc.ch2;
		left_X=L_X;
		left_Y=L_Y;

		left_X*=RC_X_ratio;
		left_Y*=RC_Y_ratio;
		yaw_out_targer+=(degree_k*(-left_X));
		pitch_out_targer+=(degree_k*(left_Y));
		if(yaw_out_targer>179.9f)   yaw_out_targer-=360.0f;
		else if (yaw_out_targer<-179.9f)  yaw_out_targer+=360.0f;	
		/*
		if(RC_CtrlData.rc.s2==1) 
		{
			pitch_out_targer=15;
		}
		if(RC_CtrlData.rc.s2==3) 
		{
			pitch_out_targer=0;
		}
		if(RC_CtrlData.rc.s2==2) 
		{
			pitch_out_targer=-15;
		}
		*/
		/*
		if(RC_CtrlData.rc.s2==1) 
		{
			yaw_out_targer=20;
		}
		if(RC_CtrlData.rc.s2==3) 
		{
			
			yaw_out_targer=0;
		}
		if(RC_CtrlData.rc.s2==2) 
		{
			
			yaw_out_targer=-20;
		}
		*/
	}
	if(STOP==KEY_mode)
	{
		//mouse
		kalman mouse_x;
		kalman mouse_y;
		kalmanCreate(&mouse_x,60000,40000);
		kalmanCreate(&mouse_y,2000,3000);
		left_X = KalmanFilter(&mouse_x,RC_CtrlData.mouse.x); 
		left_Y = KalmanFilter(&mouse_y,RC_CtrlData.mouse.y); 

		left_X*=Key_X_ratio;
		left_Y*=Key_Y_ratio;
		yaw_out_targer+=(degree_k*(-left_X));
		pitch_out_targer+=(degree_k*(-left_Y));
		if(yaw_out_targer>179.9f)   yaw_out_targer-=360.0f;
		else if (yaw_out_targer<-179.9f)  yaw_out_targer+=360.0f;
	}
}
/*Gimbal PID count*/
void Gimbal_PID(void)           //云台PID运算
{
	static int GYR_first=0;
	static float yaw_eular_ActualSpeed,pitch_eular_ActualSpeed;
	static float yaw_gyro_ActualSpeed,pitch_gyro_ActualSpeed;

	get_eular(Info_imu.eular);
	get_raw_gyo(Info_imu.gyro);
	pitch_eular_ActualSpeed=Info_imu.eular[0];     //反馈的角度设置为pitch外环的实际值
	pitch_gyro_ActualSpeed=Info_imu.gyro[1]*0.1;   //将绕x轴转动的角速度（pitch轴）赋值结构体内环
	yaw_eular_ActualSpeed=Info_imu.eular[2];     //反馈的角度设置为外环的实际值
	yaw_gyro_ActualSpeed=Info_imu.gyro[2]*0.1;   //将绕z轴转动的角速度（yaw轴）赋值结构体内环

	GYR_first++;
	if(GYR_first<5)
	{
		pitch_out_targer=pitch_eular_ActualSpeed;
		yaw_out_targer=yaw_eular_ActualSpeed;
	}
	else if(GYR_first>5) GYR_first=6;

	if(pitch_out_targer>=24.0f) pitch_out_targer=24.0f;         //俯仰角限幅
	else if(pitch_out_targer<=-18.0f) pitch_out_targer=-18.0f;

    //yaw轴的串级PID
	Position_PID(&yaw_outer_pid,yaw_out_targer,yaw_eular_ActualSpeed);
	Position_PID(&yaw_inner_pid,yaw_outer_pid.out,yaw_gyro_ActualSpeed);

    //pitch轴的串级PID
	Position_PID(&pitch_outer_pid,pitch_out_targer,pitch_eular_ActualSpeed);
	Position_PID(&pitch_inner_pid,pitch_outer_pid.out,pitch_gyro_ActualSpeed);
}






