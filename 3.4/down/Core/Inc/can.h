/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
struct motor
{
    float ecd;    						//????
    int16_t speed_rpm;         //????
    uint8_t temperate;          //????
    float last_ecd;            //????????
	  float llast_ecd;
		float degree_v;           //???
		float degree;							//??
		float last_degree;
		float target_speed;							//????

    float target;                   //??????
    float outer_KP;                 
    float outer_out;                
    float inner_KP;                 
    float inner_KI;                 
    float inner_KD;                 

    float ActualSpeed;
		float kp;
		float ki;
		float kd;
		float Pr_Error;
		float Last_Error;
		float thisError;
		float integral;
    float voltage;
		int16_t out;
    float increment;            
};


struct gimbal_outer_pid
{
    float target;
    float ActualSpeed;
    float integral;
    float kp;
    float ki;
    float kd;
    
    float thisError;
    float Last_Error;
    float voltage;
    float out;

    int judge;
};

struct gimbal_inner_pid
{
    float target;
    float ActualSpeed;
    float integral;
    float kp;
    float ki;
    float kd;
    
    float thisError;
    float Last_Error;
    float voltage;
    float out;
		int judge;
};

struct dial_data
{
  float angle;
  float last_angle;
  float llast_angle;
  float total_angle;
  float angle_set;
  float angle_first;

  float state;
};

struct motor_measure_t
{
    float  ecd;
    float speed_rpm;
    float given_current;
    float temperate;
    float last_ecd;
};

struct supercap
{
    float Bat_V;
    float power;
    float cap_V;
    float cap_state;
};

struct RC
{
    float Bat_V;
    float power;
    float cap_V;
    float cap_state;
};


extern struct supercap supercap;

extern struct dial_data dial_data;

extern struct gimbal_outer_pid yaw_outer_pid;
extern struct gimbal_outer_pid pitch_outer_pid;
extern struct gimbal_outer_pid dial_outer_pid;

extern struct gimbal_inner_pid yaw_inner_pid;
extern struct gimbal_inner_pid pitch_inner_pid;
extern struct gimbal_inner_pid dial_inner_pid;


extern struct motor wheel_moter[4];
extern struct motor dial_motor;
extern struct motor gimbal_yaw_motor;
extern struct motor gimbal_pitch_motor;
extern struct motor Friction_motor[2];
extern float Dial_motor_angle_Act_last;
extern float Dial_motor_angle_Act;

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

extern CAN_HandleTypeDef hcan2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_CAN1_Init(void);
void MX_CAN2_Init(void);

/* USER CODE BEGIN Prototypes */
void CAN1_Filterinit_And_Start(void);
void CAN_TX_Message(CAN_TxHeaderTypeDef*CAN1_TX);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void Dial_CAN_TX_Message(int16_t motor);
void CAN2_Filterinit_And_Start(void);
void friction_CAN_TX(int16_t motor1,int16_t motor2);
void pitch_CAN_TX(int16_t motor);
void yaw_CAN_TX(int16_t motor);
void CAN1_0x333_TX(int16_t motor1);
void CAN2_0x300_TX(int16_t motor1,int16_t motor2,int16_t motor3,int16_t motor4);

void CAN2_0x200_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CAN2_0x1FF_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CAN1_0x1FF_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CAN1_0x200_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void RC_can(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

