#ifndef _GIMBAL_H
#define _GIMBAL_H

#include "main.h"
#include "Can_Transfer.h"
#include "cmsis_os.h"
#include "rc.h"
#include "kalman.h"
#include "pid.h"
#include "variable_config.h"
#include "imu_data_decode.h"
#include "packet.h"
#include "Ranging.h"

void ReadRc_Gimbal(void);
void Gimbal_PID(void);
void f_Gimbal_set_offline(void);
void f_imu_data_processing(void);
void f_rc_data_processing(void);
#endif

