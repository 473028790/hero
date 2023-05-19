#include "Report.h"
union
{
	float value;
	uint8_t sendbuf[4];
}vofa_data[5];

void report_SendData(float data1,float data2,float data3,float data4,float data5)
{	
		uint8_t tail  =0x80;
		uint8_t tail_2=0x7f;
		uint8_t tail_3=0x00;
		vofa_data[0].value = data1;
		vofa_data[1].value = data2;
		vofa_data[2].value = data3;
		vofa_data[3].value = data4;
		vofa_data[4].value = data5;

		int h,m;
		for(h = 0; h < 5; h++ )
		{
			for(m = 0; m < 4; m++ )
			{
				HAL_UART_Transmit(&huart3,(uint8_t *)&vofa_data[h].sendbuf[m],sizeof(vofa_data[h].sendbuf[m]),0x10);
			}
		}
		HAL_UART_Transmit(&huart3,(uint8_t *)&tail_3,1,0x10);
		HAL_UART_Transmit(&huart3,(uint8_t *)&tail_3,1,0x10);
		HAL_UART_Transmit(&huart3,(uint8_t *)&tail,1,0x10);
		HAL_UART_Transmit(&huart3,(uint8_t *)&tail_2,1,0x10);
};
