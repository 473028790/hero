#include "pid.h"
#include "can.h"
#include "rc.h"
#include "imu_data_decode.h" 
#include "math.h"

float change_gy;
float change_degree;
float change_ecd;
float Degree_po=0;
extern RC_Ctl_t RC_CtrlData;
extern uint8_t MODE;
extern uint8_t STOP;
extern int16_t L_Y;
extern int16_t L_X;
extern int16_t R_Y;
extern int16_t R_X;

extern KEY	KEY_Date;
float last_store[3];	//储存陀螺仪测量的角度

int16_t store[3]; 			//储存陀螺仪测量的角速度

void Incremental_PID(struct motor *motor);
void location_PID(struct motor *motor);




void PID_init()            //集合PID初始化
{ 
   wheel_pid_init();      //轮子pid初始化

}

void wheel_pid_init()      //轮子PID初始化
{
   int motor_id=0;

      wheel_moter[0].kp=8.0;
      wheel_moter[0].ki=0.33;
      wheel_moter[0].kd=0.01;

      wheel_moter[1].kp=8.0;
      wheel_moter[1].ki=0.32;
      wheel_moter[1].kd=0.01;

      wheel_moter[2].kp=8.0;
      wheel_moter[2].ki=0.3;
      wheel_moter[2].kd=0.01;

      wheel_moter[3].kp=8.0;
      wheel_moter[3].ki=0.38;
      wheel_moter[3].kd=0.01;
	
   for(motor_id=0;motor_id<4;motor_id++)
   {
      wheel_moter[motor_id].voltage=0;
      wheel_moter[motor_id].ActualSpeed=0;
      wheel_moter[motor_id].target_speed=0;
      wheel_moter[motor_id].Pr_Error=0;
      wheel_moter[motor_id].Last_Error=0;
      wheel_moter[motor_id].thisError=0;
      wheel_moter[motor_id].integral=0;
      wheel_moter[motor_id].increment=0;
      wheel_moter[motor_id].out=0;
   }
	 
	 
	 
}

void Gimbal_PID_init()     //云台PID初始化 
{
   gimbal_yaw_pid_init();
   gimbal_pitch_pid_init();
}
void gimbal_yaw_pid_init()                   //yaw内外环PID初始化
{
   gimbal_yaw_inner_pid_init();
   gimbal_yaw_outer_pid_init();
}
void gimbal_yaw_outer_pid_init()             //yaw轴外环PID初始化
{
   yaw_outer_pid.target=0.0;
   yaw_outer_pid.ActualSpeed=0.0;

   yaw_outer_pid.kp=10.0;
   yaw_outer_pid.ki=0;
   yaw_outer_pid.kd=0;
   yaw_outer_pid.integral=0;

   yaw_outer_pid.voltage=0;
   yaw_outer_pid.thisError=0.0;
   yaw_outer_pid.Last_Error=0.0;
   yaw_outer_pid.judge=1;
   yaw_outer_pid.out=0;
}
void gimbal_yaw_inner_pid_init()             //yaw轴内环PID初始化
{
   yaw_inner_pid.target=0.0;
   yaw_inner_pid.ActualSpeed=0.0;

   yaw_inner_pid.kp=110.0;
   yaw_inner_pid.ki=0.65;
   yaw_inner_pid.kd=0;
   yaw_inner_pid.integral=0;

   yaw_inner_pid.voltage=0;
   yaw_inner_pid.thisError=0.0;
   yaw_inner_pid.Last_Error=0.0;
}

void gimbal_pitch_pid_init()                   //pitch内外环PID初始化
{
   gimbal_pitch_inner_pid_init();
   gimbal_pitch_outer_pid_init();
}
void gimbal_pitch_outer_pid_init()             //pitch轴外环PID初始化
{
   pitch_outer_pid.target=0.0;
   pitch_outer_pid.ActualSpeed=0.0;

   pitch_outer_pid.kp=10.0;
   pitch_outer_pid.ki=0;
   pitch_outer_pid.kd=0;
   pitch_outer_pid.integral=0;

   pitch_outer_pid.voltage=0;
   pitch_outer_pid.thisError=0.0;
   pitch_outer_pid.Last_Error=0.0;
   pitch_outer_pid.judge=0;
   pitch_outer_pid.out=0;
}
void gimbal_pitch_inner_pid_init()             //pitch轴内环PID初始化
{
   pitch_inner_pid.target=0.0;
   pitch_inner_pid.ActualSpeed=0.0;

   pitch_inner_pid.kp=110;
   pitch_inner_pid.ki=0.75;
   pitch_inner_pid.kd=0;
   pitch_inner_pid.integral=0;

   pitch_inner_pid.voltage=0;
   pitch_inner_pid.thisError=0.0;
   pitch_inner_pid.Last_Error=0.0;
}


extern int16_t CAN1_0X200[4];
void Chassis_PID()          //底盘4个电机进行pid运算
{
	
		Incremental_PID(&wheel_moter[0]);
		Incremental_PID(&wheel_moter[1]);
		Incremental_PID(&wheel_moter[2]);
		Incremental_PID(&wheel_moter[3]);
	
		//location_PID(&wheel_moter[0]);
		//location_PID(&wheel_moter[1]);
		//location_PID(&wheel_moter[2]);
		//location_PID(&wheel_moter[3]);
	
	
		CAN1_0X200[0]=wheel_moter[0].out;
		CAN1_0X200[1]=wheel_moter[1].out;
		CAN1_0X200[2]=wheel_moter[2].out;
		CAN1_0X200[3]=wheel_moter[3].out;
	
}


#define ABS(x)	( (x>0) ? (x) : (-x) )


void Incremental_PID(struct motor *motor) //增量式PID通用
{
   motor->thisError=(motor->target_speed-motor->ActualSpeed);

   motor->increment=motor->kp*(motor->thisError-motor->Last_Error)+motor->ki*motor->thisError
   +motor->kd*(motor->thisError-2*motor->Last_Error+motor->Pr_Error);

   motor->Pr_Error=motor->Last_Error;
   motor->Last_Error=motor->thisError;

   motor->out+=motor->increment;
   if(motor->out>=10000)   motor->out=10000;
   if(motor->out<=-10000)  motor->out=-10000;
}

void location_PID(struct motor *motor) //位置式PID通用
{
   motor->thisError=(motor->target_speed-motor->ActualSpeed);
   motor->integral+=motor->thisError;
   if(motor->integral>=10000) motor->integral=10000;
   if(motor->integral<=-10000) motor->integral=-10000;

   motor->voltage=motor->kp*motor->thisError+motor->ki*motor->integral+
                  motor->kd*(motor->thisError-motor->Last_Error);
   motor->Last_Error=motor->thisError;
   motor->out=motor->voltage;

   if(motor->out>=10000)   motor->out=10000;
   if(motor->out<=-10000)  motor->out=-10000;
}



