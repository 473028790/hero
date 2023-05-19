/*
 * @Description: Launch mission
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-27 14:36:52
 */
#include "Shoot.h"
#include <string.h>

extern PI_t PI_Fric_201;
extern PI_t PI_Fric_202;
extern Info_3508_t Friction_motor1;
extern Info_3508_t Friction_motor2;
extern Info_3508_t Dial_motor;

static RC_Ctl_t RC_CtrlData;
static dial_data_t dial_data;
static uint8_t dial_sign=0;
static uint8_t dial_start_number_rc=0;
static uint8_t dial_start_number_mouse=0;

extern PID_t dial_outer_pid;
extern PID_t dial_inner_pid;
/**
 * @brief: Shoot Task
 * @param {void*} argument
 * @return {*}
 */
void Shoot_task(void const * argument)
{
	uint32_t PreviousWaketime = osKernelSysTick();
	for(;;)
	{
		f_get_RC(&RC_CtrlData);
		f_Friction_control();
		f_ReadRc_dial();
		f_dial_PID();
		osDelayUntil(&PreviousWaketime,2);
	}
}
/**
 * @brief: Control of friction wheels
 * @return void
 */
void f_Friction_control(void)
{
	static float Fric_target;
	if(RC_CtrlData.rc.s1==1)
	{
		Fric_target=5700.0f;
	}
	if(RC_CtrlData.rc.s1==2)
	{
		Fric_target=5700.0f;
	}
	Incremental_PI(&PI_Fric_201,-Fric_target,Friction_motor1.Speed);
	Incremental_PI(&PI_Fric_202,Fric_target,Friction_motor2.Speed);

	f_can_assignment(2,0X200,0,PI_Fric_201.out);
	f_can_assignment(2,0X200,1,PI_Fric_202.out);
}
/**
 * @brief: Calculate the number of turns the dial rotates
 * @return {*}
 */
void f_ReadRc_dial(void)
{
	if(RC_CtrlData.rc.s1==1)
	{
		if(RC_CtrlData.rc.s2==1)
		{
			dial_start_number_rc++;
			if(dial_start_number_rc==1)
			{
				dial_data.angle=Dial_motor.ecd;
				f_get_moto_offset(&dial_data);
			}
			if(dial_start_number_rc>3) dial_start_number_rc=3;
		}
		dial_data.angle=Dial_motor.ecd;
		f_get_total_angle(&dial_data);

		if(RC_CtrlData.rc.s2==3)
		{
			f_dial_finish();
		}
	}
	if(RC_CtrlData.rc.s1==2)
	{
		if(RC_CtrlData.mouse.press_l==1)
		{
			dial_sign=1;
		}
		if(dial_sign==1)
		{
			dial_start_number_mouse++;
			if(dial_start_number_mouse==1)
			{
				f_get_moto_offset(&dial_data);
			}
			if(dial_start_number_mouse>3) dial_start_number_mouse=3;

			f_get_total_angle(&dial_data);
		}
	}

}
static float dial_number_target;
void f_dial_PID(void)
{
	static float dial_total_angle_Act,pitch_eular_ActualSpeed,dial_inner_Act;

	dial_number_target=dial_data.angle_set;
	dial_total_angle_Act=dial_data.total_angle;
	dial_inner_Act=Dial_motor.Speed;

	Position_PID(&dial_outer_pid,dial_number_target,dial_total_angle_Act);
	Position_PID(&dial_inner_pid,dial_outer_pid.out,dial_inner_Act);
	//f_can_assignment(1,0X1FF,2,dial_inner_pid.out);
	if(RC_CtrlData.rc.s1==1)
	{
		if(fabs(dial_total_angle_Act-dial_number_target)<15)
		{
			dial_data.total_angle=0;
			dial_data.angle_first=0;
			dial_data.angle_set=0;
		}
	}
	else if(RC_CtrlData.rc.s1==2)
	{
		if(fabs(dial_total_angle_Act-dial_number_target)<15)
		{
			dial_sign=0;
			dial_start_number_mouse=0;
			dial_start_number_rc=0;

			dial_data.total_angle=0;
			dial_data.angle_first=0;
			dial_data.angle_set=0;
		}
	}
}
/**
 * @brief: Calculate the number of revolutions the motor has made
 * @param {dial_data_t} *p
 * @return {void}
 */
void f_get_total_angle(dial_data_t *p)
{
	int res1, res2, delta;
	if(p->angle < p->last_angle){			//可能的情况
		res1 = p->angle + 8192 - p->last_angle;	//正转，delta=+
		res2 = p->angle - p->last_angle;				//反转	delta=-
	}else{	//angle > last
		res1 = p->angle - 8192 - p->last_angle ;//反转	delta -
		res2 = p->angle - p->last_angle;				//正转	delta +
	}
	//不管正反转，肯定是转的角度小的那个是真的
	if(ABS(res1)<ABS(res2))
		delta = res1;
	else
		delta = res2;
   delta=(360*delta)/8191;
 
	p->total_angle += delta;
	p->last_angle = p->angle;
}

/**
 * @brief: Determine the number of turns the dial rotates
 * @param {dial_data_t} *ptr
 * @return {void}
 */
void f_get_moto_offset(dial_data_t *ptr)
{
	static float dial_turns=1.5f;
	ptr->angle_first=ptr->angle;
	ptr->angle_set=((int)(dial_turns*(31593)))-ptr->angle_first;
	ptr->angle_set=(360*(ptr->angle_set))/8191;
}
/**
 * @brief: End of dial operation
 * @return {void}
 */
void f_dial_finish(void)
{
	dial_start_number_rc=0;
	memset(&dial_data, 0, sizeof(dial_data));
	dial_number_target=0;
	dial_outer_pid.integral=0;

	dial_outer_pid.out=0;
	dial_inner_pid.integral=0;
}
