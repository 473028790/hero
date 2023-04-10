#ifndef __VISION_TASK_H_
#define __VISION_TASK_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "pid.h"
/* Exported types ------------------------------------------------------------*/
/*自瞄模式*/
typedef enum
{
 AIMBOT_MODE_OFF=0,
 LOCK_ENEMY=1,
	
}Aimbot_mode_t;

/*自瞄数据接收*/
typedef struct
{
  float aimbot_yaw_angle;
	float aimbot_pit_angle;
}aimbot_rec_data_t;

/*视觉总结构体*/
typedef struct 
{
  Aimbot_mode_t Aimbot_mode;/*自瞄模式*/
	aimbot_rec_data_t aimbot_rec_data;/*视觉回传数据*/
	float aimbot_lpf;/*低通滤波*/
	
}vision_info_t;


/* Exported functions --------------------------------------------------------*/
void StartVisionTask(void const * argument);
void Aimbot_Receive(uint8_t *aimbol_buffer);
#endif
