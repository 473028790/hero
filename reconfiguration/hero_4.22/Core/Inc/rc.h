#ifndef RC_H_
#define RC_H_
#include "main.h"
#include "variable_config.h"

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
}Key_t;



uint8_t f_get_RC(RC_Ctl_t * RC_Ctrl);
uint8_t f_get_mouse(RC_Ctl_t * RC_Ctrl);
uint8_t f_get_key(Key_t * KEY_Ctrl);
void f_dbus_error_detect(void);
void f_RemoteDataProcess(uint8_t *pData);
void f_RC_data_init(void);
void f_Soft_restart(void);
#endif
