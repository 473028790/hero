#ifndef VARIABLE_CONFIG_H_
#define VARIABLE_CONFIG_H_
#include <stdbool.h>
#include <main.h>

/* --------------------------------RC -------------------------------------- */
#define RC_uart USART2
/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_mode 1
#define OFFLINE_MODE 3
#define KEY_mode 2
/* ----------------------- PC Key Definition-------------------------------- */
#define RC_FRAME_LENGTH 18u
#define SBUS_RX_BUF_NUM 36u

/* -------------------------------Gimbal ----------------------------------- */
/*RC*/
#define RC_X_ratio 0.03f
#define RC_Y_ratio 0.006f
#define degree_k   0.01f
/*Key*/
#define Key_X_ratio 0.6f
#define Key_Y_ratio 0.5f

#define PITCH_MAX 24.0f
#define PITCH_MIN -18.0f


/* ---------------------------------IMU------------------------------------ */
#define Imu_uart UART5
typedef  struct  id0x92_t {
        uint8_t     tag;                /* 0x91 */
        uint8_t     id;
        uint8_t     rev[6];             /* reserved */
        uint32_t    ts;                 /* timestamp */
        float       acc[3];
        float       gyr[3];
        float       mag[3];
        float       eul[3];             /* eular angles:R/P/Y */
        float       quat[4];            /* quaternion */

} id0x92_t;



/* ---------------------------------Dial------------------------------------ */
#define cascade 1
typedef struct dial_data_s
{
  float angle;
  float last_angle;
  float llast_angle;
  float total_angle;
  float angle_set;
  float angle_first;

  float state;
}dial_data_t;








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

/* ---------------------------------Motor------------------------------------ */
typedef struct{
	int16_t ecd;  //Mechanical Angle 0-8191
	int16_t Speed;  //Rotating Speed
	int16_t Current;//Torque Current
	uint8_t Temp;   //Motor Temperature	
}Info_3508_t;
typedef struct{
	int16_t ecd;  //Mechanical Angle 0-8191
	int16_t Speed;  //Rotating Speed
	int16_t Current;//Torque Current
	uint8_t Temp;   //Motor Temperature	
}Info_6020_t;
typedef struct{
	float angle;  		//Angle now
	float last_angle;
	float total_angle;
}Info_Dial_angle_t;

/* ----------------------------------PID------------------------------------- */
typedef struct
{
    float integral;
    float integral_max;
    float Deadzone;

    float kp;
    float ki;
    float kd;

    float P_out;
    float I_out;
    float D_out;

    float thisError;
    float Last_Error;
    float voltage;

    float out;
    float out_max;
}PID_t;

typedef struct
{
    float kp;
    float ki;
    
    float thisError;
    float Last_Error;

    float P_out;
    float I_out;

    float out;
    float out_max;
}PI_t;


#endif
