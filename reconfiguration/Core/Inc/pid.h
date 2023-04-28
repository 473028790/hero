#ifndef __PID_H
#define __PID_H
#include "main.h"
#include "math.h"
typedef struct
{
    float integral;
    float integral_max;
    float Deadzone;

    float kp;
    float ki;
    float kd;

    float P_out;
    float I_out;
    float D_out;

    float thisError;
    float Last_Error;
    float voltage;

    float out;
    float out_max;
}PID_t;

typedef struct
{
    float kp;
    float ki;
    
    float thisError;
    float Last_Error;

    float P_out;
    float I_out;

    float out;
    float out_max;
}PI_t;
void Position_PID(PID_t*pid,float target,float ActualSpeed);
#endif


