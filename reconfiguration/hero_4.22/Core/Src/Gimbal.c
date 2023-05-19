/*
 * @Description: Gimbal control
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-24 00:55:19
 */
#include "Gimbal.h"
extern PID_t yaw_outer_pid;
extern PID_t yaw_inner_pid;
extern PID_t pitch_outer_pid;
extern PID_t pitch_inner_pid;

extern osSemaphoreId Imu_flagHandle;
extern osSemaphoreId Rc_flagHandle;

extern uint8_t UART5_temp[128];
extern volatile uint8_t sbus_rx_buffer[2][18u];


static RC_Ctl_t RC_CtrlData;
static float yaw_out_targer,pitch_out_targer;
/**
 * @brief: Gimbal Task
 * @param {void*} argument
 * @return {*}
 */
void Gimbal_Task(void const * argument)
{
	uint32_t PreviousWaketime = osKernelSysTick();
	for(;;)
	{
		f_rc_data_processing();
		f_imu_data_processing();
		ReadRc_Gimbal();
		Gimbal_PID();
		osDelayUntil(&PreviousWaketime,2);
	}
}
/**
 * @brief: Process Imu data when receiving semaphores
 * @return {*}
 */
void f_imu_data_processing(void)
{
	if(osOK == osSemaphoreWait(Imu_flagHandle,1))
	{
		IMU_Pack_Recive(UART5_temp);
	}
}
/**
 * @brief: Process Rc data when receiving semaphores
 * @return {*}
 */
void f_rc_data_processing(void)
{
	if(osOK == osSemaphoreWait(Rc_flagHandle,1))
	{
		f_RemoteDataProcess((uint8_t*)&sbus_rx_buffer[0]);
	}
}
/**
 * @brief: Determination of target values for pan tilt data
 * @return {*}
 */
void ReadRc_Gimbal(void)
{
	static int16_t RC_left_x;
	static int16_t RC_left_y;
	f_get_RC(&RC_CtrlData);
	f_get_mouse(&RC_CtrlData);

	if(RC_CtrlData.rc.s1==RC_mode)
	{
		RC_left_y = (int16_t)RC_CtrlData.rc.ch3;
		RC_left_x = (int16_t)RC_CtrlData.rc.ch2;

		RC_left_x*=RC_X_ratio;
		RC_left_y*=RC_Y_ratio;
		yaw_out_targer+=(degree_k*(-RC_left_x));
		pitch_out_targer+=(degree_k*(RC_left_y));

		if(yaw_out_targer>179.9f)   yaw_out_targer-=360.0f;
		else if (yaw_out_targer<-179.9f)  yaw_out_targer+=360.0f;	
		if(pitch_out_targer>=PITCH_MAX) pitch_out_targer=PITCH_MAX;         //俯仰角限幅
		else if(pitch_out_targer<=PITCH_MIN) pitch_out_targer=PITCH_MIN;

		
		if(RC_CtrlData.rc.s2==1) 
		{
			pitch_out_targer=10;
		}
		if(RC_CtrlData.rc.s2==3) 
		{
			pitch_out_targer=0;
		}
		if(RC_CtrlData.rc.s2==2) 
		{
			pitch_out_targer=-15;
		}
		
		/*
		if(RC_CtrlData.rc.s2==1) 
		{
			yaw_out_targer=60;
		}
		if(RC_CtrlData.rc.s2==3) 
		{
			
			yaw_out_targer=0;
		}
		if(RC_CtrlData.rc.s2==2) 
		{
			
			yaw_out_targer=-60;
		}
		*/
	}
	if(RC_CtrlData.rc.s1==KEY_mode)
	{
		//mouse
		kalman mouse_x;
		kalman mouse_y;
		kalmanCreate(&mouse_x,60000,40000);
		kalmanCreate(&mouse_y,2000,3000);
		RC_left_x = KalmanFilter(&mouse_x,RC_CtrlData.mouse.x); 
		RC_left_y = KalmanFilter(&mouse_y,RC_CtrlData.mouse.y); 

		RC_left_x*=Key_X_ratio;
		RC_left_y*=Key_Y_ratio;
		yaw_out_targer+=(degree_k*(-RC_left_x));
		pitch_out_targer+=(degree_k*(-RC_left_y));
		if(yaw_out_targer>179.9f)   yaw_out_targer-=360.0f;
		else if (yaw_out_targer<-179.9f)  yaw_out_targer+=360.0f;
	}
}
/**
 * @brief: Gimbal PID count
 * @return {*}
 */
void Gimbal_PID(void)
{
	static int GYR_first=0;
	static float yaw_eular_ActualSpeed,pitch_eular_ActualSpeed;
	static float yaw_eular_ActualSpeed;
	static float yaw_gyro_ActualSpeed,pitch_gyro_ActualSpeed;
	static id0x92_t IMU_data;

	get_eular(IMU_data.eul);
	get_raw_gyr(IMU_data.gyr);
	pitch_eular_ActualSpeed=IMU_data.eul[1];     //反馈的角度设置为pitch外环的实际值
	pitch_gyro_ActualSpeed=IMU_data.gyr[1]*0.1;   //将绕x轴转动的角速度（pitch轴）赋值结构体内环
	yaw_eular_ActualSpeed=IMU_data.eul[2];     //反馈的角度设置为外环的实际值
	yaw_gyro_ActualSpeed=IMU_data.gyr[2]*0.1;   //将绕z轴转动的角速度（yaw轴）赋值结构体内环

	GYR_first++;
	if(GYR_first<5)
	{
		pitch_out_targer=pitch_eular_ActualSpeed;
		yaw_out_targer=yaw_eular_ActualSpeed;
	}
	else if(GYR_first>5) GYR_first=6;

    //YAW
	Gimbal_outer_PID(&yaw_outer_pid,yaw_out_targer,yaw_eular_ActualSpeed);
	Gimbal_outer_PID(&yaw_inner_pid,yaw_outer_pid.out,yaw_gyro_ActualSpeed);

    //PITCH
	Position_PID(&pitch_outer_pid,pitch_out_targer,pitch_eular_ActualSpeed);
	Position_PID(&pitch_inner_pid,pitch_outer_pid.out,pitch_gyro_ActualSpeed);

	if(RC_CtrlData.rc.s1!=OFFLINE_MODE)
	{
		//f_can_assignment(1,0x1FF,0,-(yaw_inner_pid.out));
		//f_can_assignment(2,0x1FF,1,pitch_inner_pid.out);
	}
}
/**
 * @brief: Gimbal offline
 * @return {*}
 */
void f_Gimbal_set_offline(void)
{
	yaw_outer_pid.integral=0;
	yaw_outer_pid.P_out=0;
	yaw_outer_pid.I_out=0;
	yaw_outer_pid.D_out=0;
	yaw_outer_pid.thisError=0;
	yaw_outer_pid.Last_Error=0;
	yaw_outer_pid.voltage=0;
	yaw_outer_pid.out=0;

	pitch_outer_pid.integral=0;
	pitch_outer_pid.P_out=0;
	pitch_outer_pid.I_out=0;
	pitch_outer_pid.D_out=0;
	pitch_outer_pid.thisError=0;
	pitch_outer_pid.Last_Error=0;
	pitch_outer_pid.voltage=0;
	pitch_outer_pid.out=0;

	yaw_inner_pid.integral=0;
	yaw_inner_pid.P_out=0;
	yaw_inner_pid.I_out=0;
	yaw_inner_pid.D_out=0;
	yaw_inner_pid.thisError=0;
	yaw_inner_pid.Last_Error=0;
	yaw_inner_pid.voltage=0;
	yaw_inner_pid.out=0;

	pitch_inner_pid.integral=0;
	pitch_inner_pid.P_out=0;
	pitch_inner_pid.I_out=0;
	pitch_inner_pid.D_out=0;
	pitch_inner_pid.thisError=0;
	pitch_inner_pid.Last_Error=0;
	pitch_inner_pid.voltage=0;
	pitch_inner_pid.out=0;
}