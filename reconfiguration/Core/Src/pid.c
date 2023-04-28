#include "pid.h"

PID_t yaw_outer_pid = {
    .kp = 10.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .out_max = 5000.0f,
    .integral_max = 0.0f,
    .Deadzone = 0.0f,
};
PID_t yaw_inner_pid= {
  .kp = 260.0f,
  .ki = 2.57f,
  .kd = 0.0f,
  .out_max = 10000.0f,
};
PID_t pitch_outer_pid= {
  .kp = 18.4f,
  .ki = 0.0f,
  .kd = 0.0f,
  .out_max = 10000.0f,
};
PID_t pitch_inner_pid= {
  .kp = 130.0f,
  .ki = 1.2f,
  .kd = 0.0f,
  .out_max = 10000.0f,
};
PI_t PI_Fric_201 = {
  .kp = 3.8f,
  .ki = 0.02f,
  .out_max = 10000.0f,
};
PI_t PI_Fric_202 = {
  .kp = 3.8f,
  .ki = 0.02f,
  .out_max = 10000.0f,
};


void Incremental_PI(PI_t*pid,float Target,float FeedBack)
{
  pid->thisError = Target - FeedBack;
  /*Calculate PI Output*/
  pid->P_out = (pid->thisError - pid->Last_Error) * pid->kp;
  pid->I_out = pid->thisError * pid->ki;
  pid->out += (pid->P_out + pid->I_out);
  pid->Last_Error = pid->thisError;
  /*Output Limit Control*/
  if(pid->out > pid->out_max) pid->out = pid->out_max;
  if(pid->out < -pid->out_max) pid->out = -pid->out_max;
}

void Position_PID(PID_t*pid,float target,float ActualSpeed)
{
   pid->thisError = target - ActualSpeed;
   if(fabs(pid->thisError)<pid->Deadzone)pid->thisError=0;
   /*PID calculation*/
   pid->P_out = pid->thisError * pid->kp;
   pid->integral+= pid->thisError;
   pid->I_out = pid->integral * pid->ki;
   pid->D_out = (pid->thisError-pid->Last_Error) * pid->kd;
   /*Integral limiting*/
   if(pid->I_out > pid->integral_max) pid->I_out = pid->integral_max;
   if(pid->I_out < -pid->integral_max) pid->I_out = -pid->integral_max;

   pid->out=  pid->P_out + pid->I_out + pid->D_out;
   pid->Last_Error = pid->thisError;
   /*Output limiting*/
   if(pid->out > pid->out_max) pid->out = pid->out_max;
   if(pid->out < -pid->out_max) pid->out = -pid->out_max;
}
