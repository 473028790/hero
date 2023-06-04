/*
 * @Description: Main functions of infrared laser ranging
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-28 21:45:23
 */
#include "Ranging.h"
#include "variable_config.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

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
    memset(&g_L1Mod_node, 0, 256);
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



#include <stdio.h>
#include <stdint.h>
#include <math.h>

double getTheta(int num,ranging_distance_t *ranging_distance);

double final_theta;
double distance_x;
void distance_test()
{
    ranging_distance_t ranging_distance;
    ranging_distance.G=9.7;
    ranging_distance.K=0.0046;
    ranging_distance.M=	0.041;
    ranging_distance.V=15.5;
    ranging_distance.X=distance_x;
    ranging_distance.Y=0.2;

    final_theta=getTheta(10,&ranging_distance);
}
double getTheta(int num,ranging_distance_t *ranging_distance) {
    double theta1 = -0.5235987755982f;
    double theta2 = 1.57079632679489f;
    double theta_mid;
    double final_theta;
    double y1 = 0;
    double y2 = 0;
    int i=0;
    for(i=0; i<num;i++){
        theta_mid = (theta1 + theta2) / 2.0f;
        y1 = (ranging_distance->M*ranging_distance->G/ranging_distance->K) * ranging_distance->X/(ranging_distance->V* cos(theta1)) + sin(theta1)/ cos(theta1)*ranging_distance->X + ranging_distance->M*ranging_distance->M*ranging_distance->G/(ranging_distance->K * ranging_distance->K) * log(1 - ranging_distance->K*ranging_distance->X/(ranging_distance->M*ranging_distance->V* cos(theta1))) - ranging_distance->Y;
        y2 = (ranging_distance->M*ranging_distance->G/ranging_distance->K) * ranging_distance->X/(ranging_distance->V* cos(theta_mid)) + sin(theta_mid)/ cos(theta_mid)*ranging_distance->X + ranging_distance->M*ranging_distance->M*ranging_distance->G/(ranging_distance->K * ranging_distance->K) * log(1 - ranging_distance->K*ranging_distance->X/(ranging_distance->M*ranging_distance->V* cos(theta_mid))) - ranging_distance->Y;

        double test =  ranging_distance->K*ranging_distance->X/(ranging_distance->M*ranging_distance->V* cos(theta1));
        if((y1 * y2) == 0)
        {
            if(y1 == 0)
            {
                final_theta = theta1;
            }
            else
            {
                final_theta = theta2;
            }
            break;
        }
        else if((y1 * y2) < 0)
        {
            theta2 = theta_mid;
        }
        else if(y1 < 0)
        {
            theta1 = theta_mid;
        }

    }
    final_theta = (theta1 + theta2)/2.0f;

    return final_theta;
}
