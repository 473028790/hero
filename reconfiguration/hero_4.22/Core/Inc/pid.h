#ifndef __PID_H
#define __PID_H
#include "main.h"
#include "math.h"

void Incremental_PI(PI_t*pid,float target,float ActualSpeed);
void Position_PID(PID_t*pid,float target,float ActualSpeed);
void Gimbal_outer_PID(PID_t*pid,float target,float ActualSpeed);
#endif


