#ifndef _PID_H_
#define _PID_H_
#include "can.h"

#define Friction 7000
#define hero 1
#define infantry 2
#define hero_and_infantry 3

void PID_init(void);
void Chassis_PID(void);

void wheel_pid_init(void);
void Gimbal_PID_init(void); 
void dial_pid_init(void);
void friction_PID_init(void);
void Friction_PID(void);
void dial_PID(void);
void get_total_angle(struct dial_data *p);
void get_moto_offset(struct dial_data *ptr);
void get_back_offset(struct dial_data *ptr);

void dial_finish(void);
void gimbal_yaw_pid_init(void);                   //yaw内外环PID初始化
void dial_outer_pid_init(void);
void dial_inner_pid_init(void);
void gimbal_yaw_outer_pid_init(void);             //yaw轴外环PID初始化
void gimbal_yaw_inner_pid_init(void);             //yaw轴内环PID初始化
void gimbal_pitch_pid_init(void);                   //pitch内外环PID初始化
void gimbal_pitch_outer_pid_init(void);             //pitch轴外环PID初始化
void gimbal_pitch_inner_pid_init(void);            //pitch轴内环PID初始化


#endif
