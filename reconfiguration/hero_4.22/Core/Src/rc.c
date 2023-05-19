/*
 * @Description: Remote control data processing
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-24 00:30:04
 */
#include "rc.h"
#include "main.h"
#include <string.h>

static RC_Ctl_t RC_CtrlData;
static Key_t KEY_Date;
/**
 * @brief: get RC data
 * @param {RC_Ctl_t*} RC_Ctrl
 * @return {*}
 */
uint8_t f_get_RC(RC_Ctl_t* RC_Ctrl)
{
	RC_Ctrl->rc=RC_CtrlData.rc;
    return 0;
}
/**
 * @brief: get mouse data
 * @param {RC_Ctl_t*} RC_Ctrl
 * @return {*}
 */
uint8_t f_get_mouse(RC_Ctl_t* RC_Ctrl)
{
	RC_Ctrl->mouse=RC_CtrlData.mouse;
    return 0;
}
/**
 * @brief: get keyboard data
 * @param {Key_t*} KEY_Ctrl
 * @return {*}
 */
uint8_t f_get_key(Key_t* KEY_Ctrl)
{
	KEY_Ctrl->W=KEY_Date.W;
	KEY_Ctrl->S=KEY_Date.S;
	KEY_Ctrl->A=KEY_Date.A;
	KEY_Ctrl->D=KEY_Date.D;
	KEY_Ctrl->Shift=KEY_Date.Shift;
	KEY_Ctrl->Ctrl=KEY_Date.Ctrl;
	KEY_Ctrl->Q=KEY_Date.Q;
	KEY_Ctrl->E=KEY_Date.E;
    return 0;
}
/**
 * @brief: RC data initialization
 * @return void
 */
void f_RC_data_init(void)
{
		memset(&RC_CtrlData, 0, sizeof(RC_CtrlData));
    RC_CtrlData.rc.s1 = OFFLINE_MODE;
    RC_CtrlData.rc.s2 = OFFLINE_MODE;
}

/**
 * @brief: Take absolute value
 * @param {int16_t} value
 * @return {int16_t}
 */

/**
 * @brief: Processing the serial port data of the remote control
 * @param {uint8_t} *pData
 * @return {void}
 */
void f_RemoteDataProcess(uint8_t *pData)
{
	if(pData == NULL)
	{
		return;
	}
	RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
	RC_CtrlData.rc.ch0 -=1024;
	if(RC_CtrlData.rc.ch0<10&&RC_CtrlData.rc.ch0>-10)
	{
		RC_CtrlData.rc.ch0=0;
	}
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
	RC_CtrlData.rc.ch1 -=1024;
	if(RC_CtrlData.rc.ch1<10&&RC_CtrlData.rc.ch1>-10)
	{
		RC_CtrlData.rc.ch1=0;
	}
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch2 -=1024;
	if(RC_CtrlData.rc.ch2<10&&RC_CtrlData.rc.ch2>-10)
	{
		RC_CtrlData.rc.ch2=0;
	}
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
	RC_CtrlData.rc.ch3 -=1024;
	if(RC_CtrlData.rc.ch3<10&&RC_CtrlData.rc.ch3>-10)
	{
		RC_CtrlData.rc.ch3=0;
	}

	RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);
	RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
	RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
	RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8); 
	RC_CtrlData.mouse.press_l = pData[12];
	RC_CtrlData.mouse.press_r = pData[13];
	RC_CtrlData.key.v = ((int16_t)pData[14]);

	//W
	KEY_Date.W=((RC_CtrlData.key.v)<<15);	
	KEY_Date.W=((KEY_Date.W)>>15);	
	//S
	KEY_Date.S=((RC_CtrlData.key.v)<<14);	
	KEY_Date.S=((KEY_Date.S)>>15);	
	//D
	KEY_Date.D=((RC_CtrlData.key.v)<<12);	
	KEY_Date.D=((KEY_Date.D)>>15);
	//A
	KEY_Date.A=((RC_CtrlData.key.v)<<13);	
	KEY_Date.A=((KEY_Date.A)>>15);
	//Q
	KEY_Date.Q=((RC_CtrlData.key.v)<<9);	
	KEY_Date.Q=((KEY_Date.Q)>>15);
	//E
	KEY_Date.E=((RC_CtrlData.key.v)>>7);
	//Shift	 
	KEY_Date.Shift=((RC_CtrlData.key.v)<<11);	
	KEY_Date.Shift=((KEY_Date.Shift)>>15);
	//Ctrl
	KEY_Date.Ctrl=((RC_CtrlData.key.v)<<10);	
	KEY_Date.Ctrl=((KEY_Date.Ctrl)>>15);
	f_dbus_error_detect();
}

/**
 * @brief: soft reboot
 * @return {*}
 */
void f_Soft_restart(void)
{
	//__set_FAULTMASK(1);
	//NVIC_SystemReset();
}
/**
 * @brief: Remote control data protection
 * @return {void}
 */
void f_dbus_error_detect(void)
{
	static uint8_t RC_error=0;
	if(RC_CtrlData.rc.ch0 > 660|| RC_CtrlData.rc.ch0 < -660)
	{
		RC_error=1;
	}
	if(RC_CtrlData.rc.ch1 > 660|| RC_CtrlData.rc.ch1 < -660)
	{
		RC_error=1;
	}
	if(RC_CtrlData.rc.ch2 > 660|| RC_CtrlData.rc.ch3 < -660)
	{
		RC_error=1;
	}
	if(RC_CtrlData.rc.ch3 > 660|| RC_CtrlData.rc.ch3 < -660)
	{
		RC_error=1;
	}
	if((RC_CtrlData.rc.s1 != 1) && (RC_CtrlData.rc.s1 != 2) && (RC_CtrlData.rc.s1 != 3))
	{
		RC_error=1;
	}
	if((RC_CtrlData.rc.s2 != 1) && (RC_CtrlData.rc.s2 != 2) && (RC_CtrlData.rc.s2 != 3))
	{
		RC_error=1;
	}
	if(RC_error==1)
	{
		RC_CtrlData.rc.ch0 = 0;
		RC_CtrlData.rc.ch1 = 0;
		RC_CtrlData.rc.ch2 = 0;
		RC_CtrlData.rc.ch3 = 0;
		RC_CtrlData.rc.s1 = 3;
		RC_CtrlData.rc.s2 = 3;
		RC_error=0;
		/* !!!Attendance:If user don't want to reset mcu,user also can reset the DMA */
		f_Soft_restart();
	}	
}


