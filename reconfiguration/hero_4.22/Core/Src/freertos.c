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
#include "Ranging.h"
#include "Laser_operation.h"
#include "Gimbal.h"
#include "Can_Transfer.h"
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
osThreadId RunningHandle;
osThreadId Gimbal__TaskHandle;
osThreadId can__TaskHandle;
osThreadId Shoot__TaskHandle;
osThreadId vofa__taskHandle;
osSemaphoreId Imu_flagHandle;
osSemaphoreId Rc_flagHandle;
osSemaphoreId Ranging_flagHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Task_Is_Running(void const * argument);
extern void Gimbal_Task(void const * argument);
extern void can_Task(void const * argument);
extern void Shoot_task(void const * argument);
extern void vofa__Task(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of Imu_flag */
  osSemaphoreDef(Imu_flag);
  Imu_flagHandle = osSemaphoreCreate(osSemaphore(Imu_flag), 1);

  /* definition and creation of Rc_flag */
  osSemaphoreDef(Rc_flag);
  Rc_flagHandle = osSemaphoreCreate(osSemaphore(Rc_flag), 1);

  /* definition and creation of Ranging_flag */
  osSemaphoreDef(Ranging_flag);
  Ranging_flagHandle = osSemaphoreCreate(osSemaphore(Ranging_flag), 1);

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
  /* definition and creation of Running */
  osThreadDef(Running, Task_Is_Running, osPriorityIdle, 0, 128);
  RunningHandle = osThreadCreate(osThread(Running), NULL);

  /* definition and creation of Gimbal__Task */
  osThreadDef(Gimbal__Task, Gimbal_Task, osPriorityNormal, 0, 512);
  Gimbal__TaskHandle = osThreadCreate(osThread(Gimbal__Task), NULL);

  /* definition and creation of can__Task */
  osThreadDef(can__Task, can_Task, osPriorityBelowNormal, 0, 512);
  can__TaskHandle = osThreadCreate(osThread(can__Task), NULL);

  /* definition and creation of Shoot__Task */
  osThreadDef(Shoot__Task, Shoot_task, osPriorityNormal, 0, 512);
  Shoot__TaskHandle = osThreadCreate(osThread(Shoot__Task), NULL);

  /* definition and creation of vofa__task */
  osThreadDef(vofa__task, vofa__Task, osPriorityLow, 0, 128);
  vofa__taskHandle = osThreadCreate(osThread(vofa__task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task_Is_Running */
/**
* @brief Function implementing the Running thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_Is_Running */
void Task_Is_Running(void const * argument)
{
  /* USER CODE BEGIN Task_Is_Running */
  uint32_t PreviousWaketime = osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2);
    osDelayUntil(&PreviousWaketime,400);
  }
  /* USER CODE END Task_Is_Running */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
