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
#include "offline.h"
#include "pid.h"
#include "can.h"
#include "vision.h"
#include "Report.h"
#include "referee.h"
#include "detect_task.h"
#include "chassis_mode.h"
#include "Ranging.h"
#include "kalman.h"

extern void Chassis_PID(void);
extern void Gimbal_PID(void);
extern void dial_PID(void);
extern void Friction_PID(void);
extern int infra_red_GPIO;
extern int infra_red_MODE;
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
osThreadId led__taskHandle;
osThreadId Gimbal__TaskHandle;
osThreadId can__TaskHandle;
osThreadId vofa__TaskHandle;
osThreadId Detect__TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void led_task(void const * argument);
void Gimbal_Task(void const * argument);
void can_Task(void const * argument);
void vofa_Task(void const * argument);
void Detect_Task(void const * argument);

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
  /* definition and creation of led__task */
  osThreadDef(led__task, led_task, osPriorityNormal, 0, 128);
  led__taskHandle = osThreadCreate(osThread(led__task), NULL);

  /* definition and creation of Gimbal__Task */
  osThreadDef(Gimbal__Task, Gimbal_Task, osPriorityBelowNormal, 0, 512);
  Gimbal__TaskHandle = osThreadCreate(osThread(Gimbal__Task), NULL);

  /* definition and creation of can__Task */
  osThreadDef(can__Task, can_Task, osPriorityAboveNormal, 0, 512);
  can__TaskHandle = osThreadCreate(osThread(can__Task), NULL);

  /* definition and creation of vofa__Task */
  osThreadDef(vofa__Task, vofa_Task, osPriorityLow, 0, 128);
  vofa__TaskHandle = osThreadCreate(osThread(vofa__Task), NULL);

  /* definition and creation of Detect__Task */
  osThreadDef(Detect__Task, Detect_Task, osPriorityBelowNormal, 0, 512);
  Detect__TaskHandle = osThreadCreate(osThread(Detect__Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_led_task */
/**
  * @brief  Function implementing the led__task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_led_task */
void led_task(void const * argument)
{
  /* USER CODE BEGIN led_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END led_task */
}

/* USER CODE BEGIN Header_Gimbal_Task */
/**
* @brief Function implementing the Gimbal__Task thread.
* @param argument: Not used
* @retval None
*/
extern L1_DATA_T L1_Data;
int cnt629=0;
/* USER CODE END Header_Gimbal_Task */
void Gimbal_Task(void const * argument)
{
  /* USER CODE BEGIN Gimbal_Task */
	static portTickType PreviousWakeTime1;
	const portTickType TimeIncrement=pdMS_TO_TICKS(2);
	PreviousWakeTime1=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		cnt629++;

		ReadRc_Gimbal();
		Gimbal_PID();
		Friction_PID();

		ReadRc_dial();
		dial_PID();


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
extern KEY	KEY_Date;
int chasses_pcb_rst=0;
extern RC_Ctl_t RC_CtrlData;
extern int FRI_slove_sign;
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
		if(KEY_Date.Ctrl==1) chasses_pcb_rst=1;
		else if (KEY_Date.Ctrl==0) chasses_pcb_rst=0;
	  CAN1_send();
	  CAN2_send();
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
int cnt16=0;
extern 	int16_t left_X;
extern 	int16_t left_Y;
extern RC_Ctl_t RC_CtrlData;
extern uint8_t ranging_flag;
extern uint8_t ranging_rc_buffer[8];
uint32_t ranging_distance;
double ranging_x;
int L1_Distance;
extern int dial_sign1;
/* USER CODE END Header_vofa_Task */
void vofa_Task(void const * argument)
{
  /* USER CODE BEGIN vofa_Task */
  static portTickType PreviousWakeTime1;
	const portTickType TimeIncrement=pdMS_TO_TICKS(2);
	PreviousWakeTime1=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		if(ranging_flag==1)
		{
			if(ranging_rc_buffer[2]==3 ||ranging_rc_buffer[2]==4)
			{
				for(int i=3;i<=6;i++)
				{
					ranging_distance=((ranging_distance << 8) | ranging_rc_buffer[i]);
				}
				ranging_x=ranging_distance;
			}
			ranging_flag=0;
		}
		
		cnt16++;
		CAN1_0x748_TX(RC_CtrlData.rc.s1,RC_CtrlData.rc.s2,RC_CtrlData.mouse.press_l,FRI_slove_sign,RC_CtrlData.rc.ch0,RC_CtrlData.rc.ch1);
		CAN1_0x478_TX(0,RC_CtrlData.key.v,RC_CtrlData.mouse.x,RC_CtrlData.rc.ch2,RC_CtrlData.rc.ch3);

		report_SendData(pitch_outer_pid.target,pitch_outer_pid.ActualSpeed,left_X,dial_motor.ActualSpeed,dial_motor.target_speed);
		vTaskDelayUntil(&PreviousWakeTime1,TimeIncrement);
  }
  /* USER CODE END vofa_Task */
}

/* USER CODE BEGIN Header_Detect_Task */
/**
* @brief Function implementing the Detect__Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Detect_Task */
void Detect_Task(void const * argument)
{
  /* USER CODE BEGIN Detect_Task */

  detect_init();
  
  static portTickType PreviousWakeTime1;
  const portTickType TimeIncrement=pdMS_TO_TICKS(57);
  PreviousWakeTime1=xTaskGetTickCount();
  

  /* Infinite loop */
  for(;;)
  {
    	DetectTask();
			vTaskDelayUntil(&PreviousWakeTime1,TimeIncrement);
   		vTaskDelay(DETECT_TASK_INIT_TIME);
  }
  /* USER CODE END Detect_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */



	 
/* USER CODE END Application */
