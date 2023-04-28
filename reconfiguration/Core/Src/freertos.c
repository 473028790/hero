/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Report.h"
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
osThreadId vofa__taskHandle;
osThreadId Gimbal__TaskHandle;
osThreadId can__TaskHandle;
osThreadId Shoot__TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void vofa__Task(void const * argument);
void Gimbal_Task(void const * argument);
void can_Task(void const * argument);
void Shoot_task(void const * argument);

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
  /* definition and creation of vofa__task */
  osThreadDef(vofa__task, vofa__Task, osPriorityLow, 0, 128);
  vofa__taskHandle = osThreadCreate(osThread(vofa__task), NULL);

  /* definition and creation of Gimbal__Task */
  osThreadDef(Gimbal__Task, Gimbal_Task, osPriorityNormal, 0, 512);
  Gimbal__TaskHandle = osThreadCreate(osThread(Gimbal__Task), NULL);

  /* definition and creation of can__Task */
  osThreadDef(can__Task, can_Task, osPriorityBelowNormal, 0, 512);
  can__TaskHandle = osThreadCreate(osThread(can__Task), NULL);

  /* definition and creation of Shoot__Task */
  osThreadDef(Shoot__Task, Shoot_task, osPriorityNormal, 0, 512);
  Shoot__TaskHandle = osThreadCreate(osThread(Shoot__Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_vofa__Task */
/**
  * @brief  Function implementing the vofa__task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_vofa__Task */
void vofa__Task(void const * argument)
{
  /* USER CODE BEGIN vofa__Task */
  uint32_t PreviousWaketime = osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
    report_SendData(0,0,0,0,0);
 		osDelayUntil(&PreviousWaketime,2);
  }
  /* USER CODE END vofa__Task */
}

/* USER CODE BEGIN Header_Gimbal_Task */
/**
* @brief Function implementing the Gimbal__Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Gimbal_Task */
void Gimbal_Task(void const * argument)
{
  /* USER CODE BEGIN Gimbal_Task */
  uint32_t PreviousWaketime = osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
//    /*Gimbal target value*/
//    ReadRc_Gimbal();
//    /*Gimbal PID count*/
//    Gimbal_PID();
    
 		osDelayUntil(&PreviousWaketime,2);
  }
  /* USER CODE END Gimbal_Task */
}

/* USER CODE BEGIN Header_can_Task */
/**
* @brief Function implementing the can__Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_Task */
void can_Task(void const * argument)
{
  /* USER CODE BEGIN can_Task */
  uint32_t PreviousWaketime = osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
//    /*YAW Dial motor data transmission*/
//    Yaw_Dial_Can_Send();
//    /*Pitch motor data transmission*/
//    Pitch_Can_Send();
//    /*Friction motor data transmission*/
//    Friction_Can_Send();

 		osDelayUntil(&PreviousWaketime,2);
  }
  /* USER CODE END can_Task */
}

/* USER CODE BEGIN Header_Shoot_task */
/**
* @brief Function implementing the Shoot__Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Shoot_task */
void Shoot_task(void const * argument)
{
  /* USER CODE BEGIN Shoot_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Shoot_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
