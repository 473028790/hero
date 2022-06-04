/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "pid.h"
#include "rc.h"



//void get_moto_offset(struct dial_data *ptr)
extern void ReadRc_dial(void);

struct motor wheel_moter[4];
struct motor dial_motor;
struct motor gimbal_yaw_motor;
struct motor gimbal_pitch_motor;
struct motor Friction_motor[2];
struct dial_data dial_data;
struct supercap supercap;

struct gimbal_inner_pid yaw_inner_pid;
struct gimbal_inner_pid pitch_inner_pid;
struct gimbal_outer_pid yaw_outer_pid;
struct gimbal_outer_pid pitch_outer_pid;

struct gimbal_inner_pid dial_inner_pid;
struct gimbal_outer_pid dial_outer_pid;

float Dial_motor_angle_Act_last;
float Dial_motor_angle_Act;

struct motor_measure_t motor_yaw, motor_pitch, motor_trigger, motor_chassis[4],motor_friction[2];


/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = ENABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}
/* CAN2 init function */
void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 3;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = ENABLE;
  hcan2.Init.AutoWakeUp = ENABLE;
  hcan2.Init.AutoRetransmission = ENABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  /* USER CODE END CAN2_Init 2 */

}

static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* CAN2 clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void CAN1_Filterinit_And_Start(void)
{
	CAN_FilterTypeDef can1_filter;
	can1_filter.FilterBank = 0;
	can1_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	can1_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	can1_filter.FilterIdHigh = 0x0000;
	can1_filter.FilterIdLow = 0x0000;
	can1_filter.FilterMaskIdHigh = 0x0000;
	can1_filter.FilterMaskIdLow = 0x0000;
	can1_filter.FilterFIFOAssignment = 0;
	can1_filter.FilterActivation = ENABLE;
	can1_filter.SlaveStartFilterBank = 0;
	HAL_CAN_ConfigFilter(&hcan1,&can1_filter);//��ʼ�� CAN1 ������
	HAL_CAN_Start(&hcan1);//���� CAN1
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);//����CAN1 FIFO0 ����
}

void CAN2_Filterinit_And_Start(void)
{
	CAN_FilterTypeDef Filter_Can2;
	Filter_Can2.FilterBank = 14;
	Filter_Can2.FilterMode = CAN_FILTERMODE_IDMASK;
	Filter_Can2.FilterScale = CAN_FILTERSCALE_32BIT;
	Filter_Can2.FilterIdHigh = 0x0000;
	Filter_Can2.FilterIdLow = 0x0000;
	Filter_Can2.FilterMaskIdHigh = 0x0000;
	Filter_Can2.FilterMaskIdLow = 0x0000;
	Filter_Can2.FilterFIFOAssignment = 0;
	Filter_Can2.FilterActivation = ENABLE;
	Filter_Can2.SlaveStartFilterBank = 0;
	HAL_CAN_ConfigFilter(&hcan2,&Filter_Can2);
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);
}
extern RC_Ctl_t RC_CtrlData;

//extern float STOP,MODE,L_Y,L_X,R_X,R_Y;.
void get_motor_measure(struct motor_measure_t *ptr,uint8_t Data[])                         \
{                                                                                          \
    ptr->last_ecd = ptr->ecd;                                                              \
    (ptr)->ecd = (float)((int16_t)(Data[0]<<8|Data[1]));                                   \
    (ptr)->speed_rpm = (float)(uint16_t)(Data[2] << 8 |Data[3]);                           \
    (ptr)->given_current = (float)(uint16_t)(Data[4] << 8 | Data[5]);                      \
    (ptr)->temperate =(float)(uint16_t)Data[6];                                            \
}

void get_gimbal_motor_measuer(struct motor_measure_t *ptr,uint8_t Data[])                  \
{                                                                                          \
    (ptr)->last_ecd = (ptr)->ecd;                                                          \
    (ptr)->ecd = (float)((int16_t)(Data[0]<<8|Data[1]));	                                 \
    (ptr)->speed_rpm = (float)(uint16_t)(Data[2] << 8 | Data[3]);                          \
    (ptr)->given_current = (float)(uint16_t)((Data[4] << 8 | Data[5]));                    \
    (ptr)->temperate =(float)(uint16_t)Data[6];                                            \
}




void CAN1_0x200_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	CAN_TxHeaderTypeDef tx;

	tx.StdId = 0x200;
	tx.RTR = CAN_RTR_DATA;
	tx.IDE = CAN_ID_STD;
	tx.DLC = 8;

	static uint8_t Data[8];
	Data[0]=(int16_t)motor1>>8;
	Data[1]=(int16_t)motor1;
	Data[2]=(int16_t)motor2>>8;
	Data[3]=(int16_t)motor2;
	Data[4]=(int16_t)motor3 >> 8;
	Data[5]=(int16_t)motor3;
	Data[6]=(int16_t)motor4 >> 8;
	Data[7]=(int16_t)motor4;
	HAL_CAN_AddTxMessage(&hcan1,&tx,Data,(uint32_t *)CAN_TX_MAILBOX0);	
}
//yaw
void CAN1_0x1FF_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	CAN_TxHeaderTypeDef tx;

	tx.StdId = 0x1FF;
	tx.RTR = CAN_RTR_DATA;
	tx.IDE = CAN_ID_STD;
	tx.DLC = 8;

	static uint8_t Data[8];
	Data[0]=motor1>>8;
	Data[1]=motor1;
	Data[2]=motor2>>8;
	Data[3]=motor2;
	Data[4]=motor3>>8;
	Data[5]=motor3;
	Data[6]=motor4 >> 8;
	Data[7]=motor4;
	HAL_CAN_AddTxMessage(&hcan1,&tx,Data,(uint32_t *)CAN_TX_MAILBOX0);
}


//pitch
void CAN2_0x1FF_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	CAN_TxHeaderTypeDef tx;

	tx.StdId = 0x1FF;
	tx.RTR = CAN_RTR_DATA;
	tx.IDE = CAN_ID_STD;
	tx.DLC = 8;

	static uint8_t Data[8];
	Data[0]=motor1>>8;
	Data[1]=motor1;
	Data[2]=motor2>>8;
	Data[3]=motor2;
	Data[4]=motor3>>8;
	Data[5]=motor3;
	Data[6]=motor4 >> 8;
	Data[7]=motor4;
	HAL_CAN_AddTxMessage(&hcan2,&tx,Data,(uint32_t *)CAN_TX_MAILBOX0);
}



void CAN2_0x200_TX(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	CAN_TxHeaderTypeDef tx;

	tx.StdId = 0x200;
	tx.RTR = CAN_RTR_DATA;
	tx.IDE = CAN_ID_STD;
	tx.DLC = 8;

	static uint8_t Data[8];
	Data[0]=motor1>>8;
	Data[1]=motor1;
	Data[2]=motor2>>8;
	Data[3]=motor2;
	Data[4]=motor3>>8;
	Data[5]=motor3;
	Data[6]=motor4 >> 8;
	Data[7]=motor4;
	HAL_CAN_AddTxMessage(&hcan2,&tx,Data,(uint32_t *)CAN_TX_MAILBOX0);
}
void CAN1_0x333_TX(int16_t motor1)
{
	CAN_TxHeaderTypeDef tx;

	tx.StdId = 0x333;
	tx.RTR = CAN_RTR_DATA;
	tx.IDE = CAN_ID_STD;
	tx.DLC = 8;

	static uint8_t Data[8];
	Data[0]=motor1>>8;
	Data[1]=motor1;
	Data[2]=0;
	Data[3]=0;
	Data[4]=0;
	Data[5]=0;
	Data[6]=0;
	Data[7]=0;
	HAL_CAN_AddTxMessage(&hcan1,&tx,Data,(uint32_t *)CAN_TX_MAILBOX0);
}
/*
void Dial_CAN_TX_Message(int16_t motor)     
{
  CAN_TxHeaderTypeDef tx1;
	
	tx1.StdId = 0x1FF;
	tx1.RTR = CAN_RTR_DATA;
	tx1.IDE = CAN_ID_STD;
	tx1.DLC = 8;

	static uint8_t Data[8];
	Data[0]=0;
	Data[1]=0;
	Data[2]=0;
	Data[3]=0;
	Data[4]=motor>>8;
	Data[5]=motor;
	Data[6]=0;
	Data[7]=0;
	HAL_CAN_AddTxMessage(&hcan1,&tx1,Data,(uint32_t *)CAN_TX_MAILBOX0);	
}

void friction_CAN_TX(int16_t motor1,int16_t motor2)
{
  CAN_TxHeaderTypeDef tx2;
	
	tx2.StdId = 0x200;
	tx2.RTR = CAN_RTR_DATA;
	tx2.IDE = CAN_ID_STD;
	tx2.DLC = 8;

	static uint8_t Data[8];
	Data[0]=motor1>>8;
	Data[1]=motor1;
	Data[2]=motor2>>8;
	Data[3]=motor2;
	Data[4]=0;
	Data[5]=0;
	Data[6]=0;
	Data[7]=0;
	HAL_CAN_AddTxMessage(&hcan2,&tx2,Data,(uint32_t *)CAN_TX_MAILBOX0);	
}

void pitch_CAN_TX(int16_t motor)
{
  CAN_TxHeaderTypeDef tx3;
	
	tx3.StdId = 0x1FF;
	tx3.RTR = CAN_RTR_DATA;
	tx3.IDE = CAN_ID_STD;
	tx3.DLC = 8;
  //motor=-motor;

	static uint8_t Data[8];
	Data[0]=0;
	Data[1]=0;
	Data[2]=motor>>8;
	Data[3]=motor;
	Data[4]=0;
	Data[5]=0;
	Data[6]=0;
	Data[7]=0;
	HAL_CAN_AddTxMessage(&hcan2,&tx3,Data,(uint32_t *)CAN_TX_MAILBOX0);	
}

void yaw_CAN_TX(int16_t motor)
{
  CAN_TxHeaderTypeDef tx4;
	
	tx4.StdId = 0x1FF;
	tx4.RTR = CAN_RTR_DATA;
	tx4.IDE = CAN_ID_STD;
	tx4.DLC = 8;

	static uint8_t Data[8];
	Data[0]=motor>>8;
	Data[1]=motor;
	Data[2]=0;
	Data[3]=0;
	Data[4]=0;
	Data[5]=0;
	Data[6]=0;
	Data[7]=0;
	HAL_CAN_AddTxMessage(&hcan1,&tx4,Data,(uint32_t *)CAN_TX_MAILBOX0);	

}
*/

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef CAN1_RX;
  CAN_RxHeaderTypeDef CAN2_Rx;

	if(hcan->Instance==CAN1)     //CAN1
	{
    static uint8_t Data[8];
		HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&CAN1_RX,Data);
		switch(CAN1_RX.StdId)
		{
			case 0x201:
        get_motor_measure(&motor_chassis[0],&Data[8]);
        wheel_moter[0].ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
				wheel_moter[0].ActualSpeed=(float)((int16_t)(Data[2]<<8|Data[3]));
			break;
			case 0x202:
        get_motor_measure(&motor_chassis[1],&Data[8]);
				wheel_moter[1].ActualSpeed=(float)((int16_t)(Data[2]<<8|Data[3]));
			break;
			case 0x203:
        get_motor_measure(&motor_chassis[2],&Data[8]);
				wheel_moter[2].ActualSpeed=(float)((int16_t)(Data[2]<<8|Data[3]));
			break;
    	case 0x204:
        get_motor_measure(&motor_chassis[3],&Data[8]);
				wheel_moter[3].ActualSpeed=(float)((int16_t)(Data[2]<<8|Data[3]));
			break;
    	case 0x205://yaw
        get_gimbal_motor_measuer(&motor_yaw,&Data[8]);

        gimbal_yaw_motor.ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
        /*
				gimbal_yaw_motor.ActualSpeed=(float)((int16_t)(Data[2]<<8|Data[3]));
        */
			break; 
      
			case 0x207://dial
			get_motor_measure(&motor_trigger, Data);
			dial_data.angle=(float)((int16_t)(Data[0]<<8|Data[1]));
			dial_motor.ActualSpeed=(float)((int16_t)(Data[2]<<8|Data[3]));

			break;

    	case 0x666://supercap
        supercap.Bat_V=(float)((int16_t)(Data[0]<<8|Data[1]));
        supercap.power=(float)((int16_t)(Data[2]<<8|Data[3]));
        supercap.cap_V=(float)((int16_t)(Data[4]<<8|Data[5]));
		
        supercap.cap_state=(float)((int16_t)(Data[6]));
			break; 
			default: {break;}	
		}
	}
	
	if(hcan->Instance==CAN2)   //CAN2
	{
		static uint8_t Data[8];
		HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&CAN2_Rx,Data);
		
		switch(CAN2_Rx.StdId)
		{
      
			case 0x206:             //pitch
      get_gimbal_motor_measuer(&motor_pitch,&Data[8]);
      gimbal_pitch_motor.llast_ecd=gimbal_pitch_motor.last_ecd;
      gimbal_pitch_motor.last_ecd=gimbal_pitch_motor.ecd;
		  //gimbal_pitch_motor.ActualSpeed=(float)((int16_t)(Data[2] << 8 | Data[3]));
      gimbal_pitch_motor.ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
			break;
			case 0x201:
      get_motor_measure(&motor_friction[0], Data);
			Friction_motor[0].ActualSpeed=(float)((int16_t)(Data[2] << 8 | Data[3]));
      break;
			case 0x202:
      get_motor_measure(&motor_friction[1], Data);
			Friction_motor[1].ActualSpeed=(float)((int16_t)(Data[2] << 8 | Data[3]));
      break;
			default: {break;}
		}
	}

}

/* USER CODE END 1 */
