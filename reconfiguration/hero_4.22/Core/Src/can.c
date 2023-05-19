/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
Info_3508_t Dial_motor;
Info_3508_t Friction_motor1;
Info_3508_t Friction_motor2;

Info_6020_t Yaw_motor;
Info_6020_t Pitch_motor;

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
  hcan1.Init.TransmitFifoPriority = ENABLE;
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
  hcan2.Init.TransmitFifoPriority = ENABLE;
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
void CAN1_Filter_Config_and_Start(void)
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
  /*if the initialization failed,the code will go to function-Error_Handler()*/
  if(HAL_CAN_ConfigFilter(&hcan1,&can1_filter) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }
}
void CAN2_Filter_Config_and_Start(void)
{
  CAN_FilterTypeDef can2_filter;
  can2_filter.SlaveStartFilterBank = 14;
  can2_filter.FilterBank = 14;
  can2_filter.FilterMode = CAN_FILTERMODE_IDMASK;
  can2_filter.FilterScale = CAN_FILTERSCALE_32BIT;
  can2_filter.FilterIdHigh = 0x0000;
  can2_filter.FilterIdLow = 0x0000;
  can2_filter.FilterMaskIdHigh = 0x0000;
  can2_filter.FilterMaskIdLow = 0x0000;
  can2_filter.FilterFIFOAssignment = 0;
  can2_filter.FilterActivation = ENABLE;

  if(HAL_CAN_ConfigFilter(&hcan2,&can2_filter) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_CAN_Start(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }
}

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
      /*YAW*/
    	case 0x205:
        Yaw_motor.ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
        Yaw_motor.Speed=(float)((int16_t)(Data[2]<<8|Data[3]));
        Yaw_motor.Current=(float)((int16_t)(Data[4]<<8|Data[5]));
        Yaw_motor.Temp=(float)((int16_t)(Data[6]));
			break;
      /*Dial*/
			case 0x207:
        Dial_motor.ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
        Dial_motor.Speed=(float)((int16_t)(Data[2]<<8|Data[3]));
        Dial_motor.Current=(float)((int16_t)(Data[4]<<8|Data[5]));
        Dial_motor.Temp=(float)((int16_t)(Data[6]));
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
      /*Pitch*/
			case 0x206:
        Pitch_motor.ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
        Pitch_motor.Speed=(float)((int16_t)(Data[2]<<8|Data[3]));
        Pitch_motor.Current=(float)((int16_t)(Data[4]<<8|Data[5]));
        Pitch_motor.Temp=(float)((int16_t)(Data[6]));
			break;
			case 0x201:
        Friction_motor1.ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
        Friction_motor1.Speed=(float)((int16_t)(Data[2]<<8|Data[3]));
        Friction_motor1.Current=(float)((int16_t)(Data[4]<<8|Data[5]));
        Friction_motor1.Temp=(float)((int16_t)(Data[6]));
      break;
			case 0x202:
        Friction_motor2.ecd=(float)((int16_t)(Data[0]<<8|Data[1]));
        Friction_motor2.Speed=(float)((int16_t)(Data[2]<<8|Data[3]));
        Friction_motor2.Current=(float)((int16_t)(Data[4]<<8|Data[5]));
        Friction_motor2.Temp=(float)((int16_t)(Data[6]));
      break;
			default: {break;}
		}
	}
}
/* USER CODE END 1 */
