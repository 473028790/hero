/*
 * @Description: Sending data through serial port
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-26 21:37:45
 */
#include "Vofa.h"

extern float pitch_out_targer;
extern float pitch_eular_ActualSpeed;

/**
 * @brief: Vofa Task
 * @param {void*} argument
 * @return {*}
 */
void vofa__Task(void const * argument)
{
	static float data[2];
	uint32_t PreviousWaketime = osKernelSysTick();
	for(;;)
	{
		data[0]=pitch_out_targer;
		data[1]=pitch_eular_ActualSpeed;
		f_Serial_Write((char*)data,8);
		char tail[4] = {0x00,0x00,0x80,0x7f};
		f_Serial_Write(tail,4);	
		
		osDelayUntil(&PreviousWaketime,2);
	}
}
/**
 * @brief: Send specified content through serial port
 * @param {int} len
 * @return void
 */
void f_Serial_Write(char*buf,int len)
{
	int i;
	for(i = 0;i < len;i++)
	{
		HAL_UART_Transmit(&huart4,(uint8_t*)&buf[i],1,1000);
		while (!__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TXE));
	}
}