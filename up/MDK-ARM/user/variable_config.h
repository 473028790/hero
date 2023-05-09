#ifndef VARIABLE_CONFIG_H_
#define VARIABLE_CONFIG_H_
#include <stdbool.h>


/* ---------------------------------Ranging--------------------------------- */
#define Rang_uart USART6
#define RANG_SINGLE 02U
#define RANG_CONTINUE 03U
#define RANG_FAST_CONTINUE 04U
typedef struct
{
	unsigned char buf[256];
	int length;
	bool valid;
	int type;
}usart_buf_t;
typedef struct Distance_s
{
    double V; //m/s
    double K; //风阻 0.0026 0.0046
    double M; //质量  17mm为3.2g 42mm为41g 单位kg
    double G; //重力加速度 9.8
    double X; //m
    double Y; //m
}ranging_distance_t;

#endif
