/*
 * @Description: Main functions of infrared laser ranging
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-28 21:45:23
 */
#include "Ranging.h"
#include "variable_config.h"
#include <string.h>
#include <stdio.h>
static uint8_t ranging_usart_sign;
extern uint8_t ranging_rc_buffer[8];
extern uint32_t ranging_distance;
usart_buf_t g_L1Mod_node;
/**
 * @brief: Ranging Init
 * @return:void
 * @note: Only after initialization can it be received normally
 */
void f_ranging_init(void)
{
    /*Cleaning the serial port buffer*/
    f_clear_L1Mod_Usart();
    /*Send signal to start measurement*/
    f_fastConti_Cmd();
}

/**
 * @brief: Cleaning the serial port buffer
 * @return: void
 * @note: 
 */
void f_clear_L1Mod_Usart(void)
{
    memset(&g_L1Mod_node, 0, sizeof(g_L1Mod_node));
    g_L1Mod_node.valid  = false;
}
/**
 * @brief: Send a single measurement command
 * @return void
 * @note: 
 */
void f_single_cmd(void)             
{
	unsigned char cmd[5] = {0XA5, 0X5A, 0X02, 0X00, 0XFD};
	HAL_UART_Transmit(&huart6,(uint8_t *)&cmd,sizeof(cmd),0x10);
}

/**
 * @brief: Send continuous measurement command
 * @return void
 * @note: 
 */
void f_conti_cmd(void)             
{
    unsigned char cmd[5] = {0XA5, 0X5A, 0X03, 0X00, 0XFC};
	HAL_UART_Transmit(&huart6,(uint8_t *)&cmd,sizeof(cmd),0x10);
	g_L1Mod_node.type = 2;
}
/**
 * @brief: Send fast continuous measurement commands
 * @return void
 * @note: 
 */
void f_fastConti_Cmd(void)          
{
	unsigned char cmd[5] = {0XA5, 0X5A, 0X04, 0X00, 0XFB};
	HAL_UART_Transmit(&huart6,(uint8_t *)&cmd,sizeof(cmd),0x10);
	g_L1Mod_node.type = 2;
}

void f_ranging_state(void)
{
    ranging_usart_sign=1;
}

void f_ranging_data_handle(void)
{
    if(ranging_usart_sign==1)
    {
        if(ranging_rc_buffer[2]==RANG_FAST_CONTINUE)
        {
        for(int i=3;i<=6;i++)
        {
            //ranging_distance=(ranging_distance << 8) | ranging_rc_buffer[i];
        }
        //L1_Data.L1_Distance=ranging_distance;
        }
        ranging_usart_sign=0;
    }
}
