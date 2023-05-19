#include <string.h>
#include <stdio.h>
#include "packet.h"
#include "usart.h"
/*
uint32_t imu_time;
void f_record_imu(void)
{
    imu_time = xTaskGetTickCount();
}
uint32_t systemTime;
void f_imu_detect(void)
{
    //static uint32_t systemTime;
    systemTime = xTaskGetTickCount();
    if(f_imu_abs(systemTime-imu_time)>10)
    {
        f_Soft_restart();
    }
}
static int16_t f_imu_abs(int16_t value)
{
    if (value > 0)
    {
        return value;
    }
    else
    {
        return -value;
    }
}
void f_imu_state(void)
{
    imu_usart_sign=1;
}
int ffsi=0;
void f_imu_data_handle(void)
{
    if(imu_usart_sign==1)
    {
        ffsi++;
        f_record_imu();
		Packet_Decode(imu_data);
        HAL_UART_Receive_IT(&huart5,&imu_data,1);
        imu_usart_sign=0;
    }
}


*/