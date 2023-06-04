/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rc.h"
#include "task.h"
#include "pid.h"
#include "can.h"
#include "vision.h"
#include "Report.h"
#include "referee_task.h"
#include "detect_task.h"
#include "chassis_mode.h"

extern void Chassis_PID(void);
extern void Gimbal_PID(void);
extern void dial_PID(void);
extern void Friction_PID(void);
extern int infra_red_GPIO;
extern int infra_red_MODE;
extern judge_info_t judge_infop;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId chassis__taskHandle;
osThreadId Gimbal__TaskHandle;
osThreadId can__TaskHandle;
osThreadId vofa__TaskHandle;
osThreadId client_Handle;
osThreadId super_capHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void chassis_task(void const * argument);
void Gimbal_Task(void const * argument);
void can_Task(void const * argument);
void vofa_Task(void const * argument);
void client(void const * argument);
void super_cap_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of chassis__task */
  osThreadDef(chassis__task, chassis_task, osPriorityNormal, 0, 512);
  chassis__taskHandle = osThreadCreate(osThread(chassis__task), NULL);

  /* definition and creation of Gimbal__Task */
  osThreadDef(Gimbal__Task, Gimbal_Task, osPriorityBelowNormal, 0, 512);
  Gimbal__TaskHandle = osThreadCreate(osThread(Gimbal__Task), NULL);

  /* definition and creation of can__Task */
  osThreadDef(can__Task, can_Task, osPriorityAboveNormal, 0, 512);
  can__TaskHandle = osThreadCreate(osThread(can__Task), NULL);

  /* definition and creation of vofa__Task */
  osThreadDef(vofa__Task, vofa_Task, osPriorityLow, 0, 128);
  vofa__TaskHandle = osThreadCreate(osThread(vofa__Task), NULL);

  /* definition and creation of client_ */
  osThreadDef(client_, client, osPriorityBelowNormal, 0, 128);
  client_Handle = osThreadCreate(osThread(client_), NULL);

  /* definition and creation of super_cap */
  osThreadDef(super_cap, super_cap_task, osPriorityBelowNormal, 0, 128);
  super_capHandle = osThreadCreate(osThread(super_cap), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_chassis_task */
/**
  * @brief  Function implementing the chassis__task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_chassis_task */
void chassis_task(void const * argument)
{
  /* USER CODE BEGIN chassis_task */
	static portTickType PreviousWakeTime1;
	const portTickType TimeIncrement=pdMS_TO_TICKS(2);
	PreviousWakeTime1=xTaskGetTickCount();
	
  /* Infinite loop */
  for(;;)
  {
		RC_can();
		ReadRc_Chassis();
    Function_Choose();
		Chassis_PID();
   	vTaskDelayUntil(&PreviousWakeTime1,TimeIncrement);
  }
  /* USER CODE END chassis_task */
}

/* USER CODE BEGIN Header_Gimbal_Task */
/**
* @brief Function implementing the Gimbal__Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Gimbal_Task */
int cnt26=0;
extern float ecd_diff;
void Gimbal_Task(void const * argument)
{
  /* USER CODE BEGIN Gimbal_Task */
	static portTickType PreviousWakeTime1;
	const portTickType TimeIncrement=pdMS_TO_TICKS(2);
	PreviousWakeTime1=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		cnt26++;
		report_SendData(wheel_moter[0].ActualSpeed,wheel_moter[1].ActualSpeed,wheel_moter[2].target_speed,wheel_moter[2].ActualSpeed,wheel_moter[3].ActualSpeed);

    Offline_task(); 

	  vTaskDelayUntil(&PreviousWakeTime1,TimeIncrement);
  }
  /* USER CODE END Gimbal_Task */
}

/* USER CODE BEGIN Header_can_Task */
/**
* @brief Function implementing the can__Task thread.
* @param argument: Not used
* @retval None
*/
int hero_shoot_number=0;
int cnt315=0;
/* USER CODE END Header_can_Task */
void can_Task(void const * argument)
{
  /* USER CODE BEGIN can_Task */
	static portTickType PreviousWakeTime1;
	const portTickType TimeIncrement=pdMS_TO_TICKS(5);
	PreviousWakeTime1=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		Offline_task();  
cnt315++;		
		hero_shoot_number=hero_refree_shoot_number();
	  CAN1_send();
	  //CAN2_send();
		vTaskDelayUntil(&PreviousWakeTime1,TimeIncrement);
  }
  /* USER CODE END can_Task */
}

/* USER CODE BEGIN Header_vofa_Task */
/**
* @brief Function implementing the vofa__Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vofa_Task */
void vofa_Task(void const * argument)
{
  /* USER CODE BEGIN vofa_Task */
  static portTickType PreviousWakeTime1;
	const portTickType TimeIncrement=pdMS_TO_TICKS(5);
	PreviousWakeTime1=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    //vision_Send();
	  vTaskDelayUntil(&PreviousWakeTime1,TimeIncrement);
  }
  /* USER CODE END vofa_Task */
}

/* USER CODE BEGIN Header_client */
/**
* @brief Function implementing the client_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_client */
void client(void const * argument)
{
  /* USER CODE BEGIN client */
	uint32_t previousWakeTime=osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
    UI_Ctrl();
	  osDelayUntil(&previousWakeTime,10);
  }
  /* USER CODE END client */
}

/* USER CODE BEGIN Header_super_cap_task */
/**
* @brief Function implementing the super_cap thread.
* @param argument: Not used
* @retval None
*/
extern int ciecle_speed;
extern int chassis_referee_mode;
int cnt225=0;
extern int Chassis_speed_proportion;
extern int shoot_referee_mode;
extern KEY	KEY_Date;
/* USER CODE END Header_super_cap_task */
void super_cap_task(void const * argument)
{
  /* USER CODE BEGIN super_cap_task */
	uint32_t previousWakeTime=osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
		cnt225++;
		//CAN1_0x333_TX(55);
		chassis_mode_judge();
		super_cap_buffer();
		switch(chassis_referee_mode)
		{
			case Power_priority:
			{
					if(judge_infop.GameRobotStatus.robot_level==1)
					{
						ciecle_speed=2500;
						Chassis_speed_proportion=2500;
					}
					else if(judge_infop.GameRobotStatus.robot_level==2)
					{
						ciecle_speed=3200;
						Chassis_speed_proportion=3000;
					}
					else if(judge_infop.GameRobotStatus.robot_level==3)
					{
						ciecle_speed=4200;
						Chassis_speed_proportion=3000;
					}
			}
			break;
			
			
			case Health_priority:
			{
					if(judge_infop.GameRobotStatus.robot_level==1)
					{
						ciecle_speed=2000;
						Chassis_speed_proportion=2000;
					}
					else if(judge_infop.GameRobotStatus.robot_level==2)
					{
						ciecle_speed=3200;
						Chassis_speed_proportion=2500;
					}
					else if(judge_infop.GameRobotStatus.robot_level==3)
					{
						ciecle_speed=4200;
						Chassis_speed_proportion=2500;
					}
			}
			break;
			default: {break;}
		}
	  osDelayUntil(&previousWakeTime,100);
  }
  /* USER CODE END super_cap_task */
}


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */



	 
/* USER CODE END Application */
