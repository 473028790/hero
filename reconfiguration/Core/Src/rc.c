#include "rc.h"
#include "main.h"

RC_Ctl_t RC_CtrlData;
KEY	KEY_Date;


uint8_t STOP=3;//s1,1--control   2--KEY
uint8_t MODE=3;//s2
int16_t L_X;
int16_t L_Y;
int16_t R_Y;
int16_t R_X;

static int16_t RC_abs(int16_t value);


void RC_data_init(void)
{
    RC_CtrlData.rc.ch0 = 0;
    RC_CtrlData.rc.ch1 = 0;
    RC_CtrlData.rc.ch2 = 0;
    RC_CtrlData.rc.ch3 = 0;
    RC_CtrlData.rc.s1 = RC_SW_MID;
    RC_CtrlData.rc.s2 = RC_SW_MID;
    RC_CtrlData.mouse.x = 0;
    RC_CtrlData.mouse.y = 0;
    RC_CtrlData.mouse.z = 0;
    RC_CtrlData.mouse.press_l = 0;
    RC_CtrlData.mouse.press_r = 0;
    RC_CtrlData.key.v = 0;
}
static int16_t RC_abs(int16_t value)
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
void RemoteDataProcess(uint8_t *pData)
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

	STOP=RC_CtrlData.rc.s1;
	MODE=RC_CtrlData.rc.s2;

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
}
void Soft_restart(void)
{
	/*
	__set_FAULTMASK(1);
	NVIC_SystemReset();
	*/
}