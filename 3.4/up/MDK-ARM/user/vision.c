#include "vision.h"
#include "usart.h"
extern UART_HandleTypeDef huart1;
uint8_t UART1_Rx_Buf[6];
extern float last_store[3];

union
{
    float value;
    uint8_t sendbuf[4];
}vision_data_send[2];

Vision_Data_S Vision_Data;
Vision_Data_R Vision_Data_r;

//uint8_t data[14]={0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f};
void vision_Send(void)
{	
    uint8_t head  =0x80;
    uint8_t tail  =0x7F;
    uint16_t speed=4000;
    int colour=0;
    int mode=0;
    uint8_t data[16]={0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f};
    int h,m;
    vision_data_send[0].value=last_store[2];
    for(h = 1; h < 5; h++)
    {
        data[h]=vision_data_send[0].sendbuf[h-1];
    }
    vision_data_send[1].value=last_store[0];
    for(h = 5; h < 9; h++)
    {
        data[h]=vision_data_send[1].sendbuf[h-5];
    }
    data[9]=speed>>8;
    data[10]=speed;
    data[11]=(uint8_t)colour;
    data[12]=(uint8_t)mode;

    HAL_UART_Transmit(&huart1,(uint8_t *)data,14,0x10);

}


void vision_handle(void)
{
    if(UART1_Rx_Buf[0]==0x80&&UART1_Rx_Buf[5]==0x7f)
    {    
        Vision_Data_r.MPU_yaw_increase=(float)(int16_t)(UART1_Rx_Buf[1]<<8|UART1_Rx_Buf[2]);
        Vision_Data_r.MPU_pitch_increase=(float)(int16_t)(UART1_Rx_Buf[3]<<8|UART1_Rx_Buf[4]);
    }
}
