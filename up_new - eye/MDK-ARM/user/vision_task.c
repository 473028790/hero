/* Includes ------------------------------------------------------------------*/
#include "vision_task.h"
#include "cmsis_os.h"
#include "pid.h"
#include <math.h>
#include "kalman.h"
#include "rc.h"
/* Private variables ---------------------------------------------------------*/
uint8_t aimbot_transmit_data[5];
/* Private typedef -----------------------------------------------------------*/
extern UART_HandleTypeDef huart4;

vision_info_t vision_info=
{
	.Aimbot_mode=AIMBOT_MODE_OFF,
	.aimbot_rec_data.aimbot_yaw_angle=0,
	.aimbot_rec_data.aimbot_pit_angle=0,
	.aimbot_lpf=0.02f,
};
/* Private functions ---------------------------------------------------------*/
void Aimbot_Mode(void);
void Aimbot_Transmit(void);
/**
 * @brief �Ӿ��ܿ�
 * @param 
 */
void VISION_Ctrl(void)
{
	Aimbot_Mode();
}

/**
 *	@brief	�������ݽ���
 */
int cnt19=0;
void Aimbot_Receive(uint8_t *aimbol_buffer)
{
	if(aimbol_buffer[0]==0x80&&aimbol_buffer[5]==0x7F)
	{
		cnt19++;
		/*0-180 -> 0-65535*/
		vision_info.aimbot_rec_data.aimbot_yaw_angle=((float)(int16_t)(aimbol_buffer[3]<<8|aimbol_buffer[4])*90/32767)*vision_info.aimbot_lpf;
		vision_info.aimbot_rec_data.aimbot_pit_angle=((float)(int16_t)(aimbol_buffer[1]<<8|aimbol_buffer[2])*90/32767)*vision_info.aimbot_lpf;
	}
}


/**
 *	@brief	����ģʽѡ��
 */
void Aimbot_Mode(void)
{
	if(MODE==3)/*key R */
	{
	 vision_info.Aimbot_mode=AIMBOT_MODE_OFF;
	}
	else if(MODE==1)/*key F */
	{
	 vision_info.Aimbot_mode=LOCK_ENEMY;
	}
}


/**
* @brief �Ӿ�����
* @param void 
* @return void
* @berif  3ms
*/
void StartVisionTask(void const * argument)
{
	uint32_t previousWakeTime=osKernelSysTick();
    for(;;)
    {
      VISION_Ctrl();
		  osDelayUntil(&previousWakeTime,3);
    }
}
