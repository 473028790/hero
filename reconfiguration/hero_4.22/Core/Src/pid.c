/*
 * @Description: PID structure initialization
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-24 01:24:48
 */
#include "pid.h"

PID_t yaw_outer_pid = {
    .kp = 11.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .out_max = 25000.0f,
    .integral_max = 25000.0f,
    .Deadzone = 0.0f,
};
PID_t yaw_inner_pid= {
  .kp = 350.0f,
  .ki = 0.8f,
  .kd = 0.0f,

  .Deadzone = 0.0f,
  .integral_max= 20000.0f,
  .out_max = 25000.0f,
};
PID_t pitch_outer_pid= {
  .kp = 10.0f,
  .ki = 0.0f,
  .kd = 0.0f,
  .Deadzone = 0.0f,
  .integral_max=25000.0f,
  .out_max = 25000.0f,
};
PID_t pitch_inner_pid= {
  .kp = 50.0f,
  .ki = 0.01f,
  .kd = 0.0f,
  .Deadzone = 0.0f,
  .integral_max=20000.0f,
  .out_max = 25000.0f,
};
PID_t dial_outer_pid = {
    .kp = 5.3f,
    .ki = 0.0f,
    .kd = 0.0f,
    .out_max = 5000.0f,
    .integral_max = 0.0f,
    .Deadzone = 0.0f,
};
PID_t dial_inner_pid= {
  .kp = 12.0f,
  .ki = 0.40f,
  .kd = 0.0f,
  .out_max = 10000.0f,
  .Deadzone = 0.0f,
};

PI_t PI_Fric_201 = {
  .kp = 8.1f,
  .ki = 0.2f,
  .out_max = 10000.0f,
};
PI_t PI_Fric_202 = {
  .kp = 8.4f,
  .ki = 0.21f,
  .out_max = 10000.0f,
};

/**
 * @brief: incremental pi
 * @param {float} target
 * @param {float} ActualSpeed
 */
void Incremental_PI(PI_t*pid,float target,float ActualSpeed)
{
  pid->thisError = target - ActualSpeed;
  /*Calculate PI Output*/
  pid->P_out = (pid->thisError - pid->Last_Error) * pid->kp;
  pid->I_out = pid->thisError * pid->ki;
  pid->out += (pid->P_out + pid->I_out);
  pid->Last_Error = pid->thisError;
  /*Output Limit Control*/
  if(pid->out > pid->out_max) pid->out = pid->out_max;
  if(pid->out < -pid->out_max) pid->out = -pid->out_max;
}

/**
 * @brief: Position PID
 * @param {float} target
 * @param {float} ActualSpeed
 */
void Position_PID(PID_t*pid,float target,float ActualSpeed)
{
  pid->thisError = target - ActualSpeed;
  if(fabs(pid->thisError)<pid->Deadzone)pid->thisError=0;
  if(pid->thisError>179.9f &&target<=179.9f&&target>=-179.9f)
  {
    pid->thisError-=360;        //边界处理
  }
  else if(pid->thisError<-179.9f &&target<=179.9f&&target>=-179.9f) 
  {
    pid->thisError+=360;
  }
  pid->integral+=pid->thisError;
  if(pid->integral>=pid->integral_max)
  {
    pid->integral=pid->integral_max;
  }
	if(pid->integral<=-pid->integral_max)
  {
    pid->integral=-pid->integral_max;
  } 
  pid->P_out = pid->thisError * pid->kp;
  pid->I_out = pid->integral * pid->ki;
  pid->D_out = (pid->thisError-pid->Last_Error) * pid->kd;

  pid->out=  pid->P_out + pid->I_out + pid->D_out;
  pid->Last_Error = pid->thisError;
  /*Output limiting*/
  if(pid->out > pid->out_max) pid->out = pid->out_max;
  if(pid->out < -pid->out_max) pid->out = -pid->out_max;
}



void Gimbal_outer_PID(PID_t*pid,float target,float ActualSpeed) //云台外环位置式PID专用
{
  pid->thisError = target - ActualSpeed;
  if(pid->thisError>179.9f &&target<=179.9f&&target>=-179.9f)
  {
      pid->thisError-=360;        //边界处理
  }
  else if(pid->thisError<-179.9f &&target<=179.9f&&target>=-179.9f) 
  {
      pid->thisError+=360;
  }
   pid->integral+=pid->thisError;

  if(pid->integral>=25000) pid->integral=25000;
	if(pid->integral<=-25000)  pid->integral=-25000;

   pid->voltage=pid->kp*pid->thisError+pid->ki*pid->integral+pid->kd*(pid->thisError-pid->Last_Error);

   pid->Last_Error=pid->thisError;

   pid->out=pid->voltage;
   if(pid->out>=30000)   pid->out=30000;
   if(pid->out<=-30000)  pid->out=-30000;
}
