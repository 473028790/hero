#ifndef RC_H_
#define RC_H_
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)
#define Chassis_speed_proportion (4000)//右边控制底盘最大速度的比例
/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01<<5)
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01<<6)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01<<7)
#define RC_FRAME_LENGTH 18u
#define SBUS_RX_BUF_NUM 36u
#define wheel_KEY_speed_max 1

typedef  struct
{
 struct
 {
 int16_t ch0;
 int16_t ch1;
 int16_t ch2;
 int16_t ch3;
 uint8_t s1;
 uint8_t s2;
 }rc;
 struct
 {
 int16_t x;
 int16_t y;
 int16_t z;
 uint8_t press_l;
 uint8_t press_r;
 }mouse;
 struct
 {
		uint16_t v;
 }key;
}RC_Ctl_t;

typedef  struct
{
	uint16_t W;
	uint16_t Q;
	uint16_t E;
	uint16_t A;
	uint16_t S;
	uint16_t D;
	uint16_t Shift;
	uint16_t Ctrl;

	int16_t R_Y;
	int16_t R_X;
	int16_t L_Y;
	int16_t L_X;

}KEY;

extern uint8_t MODE;
extern uint8_t STOP;
extern int16_t L_Y;
extern int16_t L_X;
extern int16_t R_Y;
extern int16_t R_X;

void ReadRc_Chassis(void);
void ReadRc_Gimbal(void);


extern void RemoteDataProcess(uint8_t *pData);
extern uint8_t RC_data_is_error(void);
extern void (*dma_M0_rx_callback)(void);
extern void (*dma_M1_rx_callback)(void);
extern void slove_RC_lost(void);
extern void slove_data_error(void);
extern void RC_restart(uint16_t dma_buf_num);
extern void slove_RC_offline(void);
void ReadRc_dial(void);
extern uint8_t GYR_data_is_error(void);
extern void slove_FRI_lost(void);
extern uint8_t Friction_data_is_errorv(void);
#endif
