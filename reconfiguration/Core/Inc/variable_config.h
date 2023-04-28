#ifndef VARIABLE_CONFIG_H_
#define VARIABLE_CONFIG_H_


/* --------------------------------RC -------------------------------------- */
#define RC_uart USART2
/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
/* ----------------------- RC Mode Definition----------------------------- */
#define RC_MODE ((uint16_t)1)
#define OFFICE_MODE ((uint16_t)3)
#define MOUSE_MODE ((uint16_t)2)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)
/* -------------------------------Gimbal ----------------------------------- */
/*RC*/
#define RC_mode 1
#define RC_X_ratio 0.03
#define RC_Y_ratio 0.006
#define degree_k   0.01
/*Key*/
#define KEY_mode 2
#define Key_X_ratio 0.6
#define Key_Y_ratio 0.5
/* ---------------------------------IMU------------------------------------ */
#define Imu_uart UART5
typedef struct
{
	int16_t acc[3];
	int16_t gyro[3];
	float eular[3];
}imu_data_t;

/* ---------------------------------Dial------------------------------------ */
#define cascade 1
/* ---------------------------------Ranging--------------------------------- */
#define Rang_uart USART6
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
#endif
