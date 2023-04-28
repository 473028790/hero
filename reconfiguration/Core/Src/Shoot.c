#include "Shoot.h"
#include "pid.h"
#include "variable_config.h"

/*friction*/
extern PI_t PI_Fric_201;
extern PI_t PI_Fric_202;
extern Info_3508_t Friction_motor1;
extern Info_3508_t Friction_motor2;
extern RC_Ctl_t RC_CtrlData;
extern Info_3508_t Dial_motor;
Info_Dial_angle_t dial_totel;

extern float CAN2_ID200[4];
/*
void Friction_PID(void)
{ 
    static FRI_target=5700;
   //测试摩擦轮用
   if(STOP!=OFFICE_MODE)
   {
        Incremental_PI(PI_Fric_201,FRI_target,Friction_motor1.Speed);
        Incremental_PI(PI_Fric_202,FRI_target,Friction_motor2.Speed);
        CAN2_ID200[0]=PI_Fric_201.out;
        CAN2_ID200[1]=PI_Fric_202.out;
   }
}
void shoot_stop(void)
{
    CAN2_ID200[0]=0;
    CAN2_ID200[1]=0;
}
//计算圈数的代码
void get_total_angle(Info_Dial_angle_t *p)
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

void dial_control(void)
{
    static int dial_mode,dial_number,dial_fist_angle;
	if(dial_mode==cascade)
	{
		if(STOP==1)
		{
			if(MODE==1)
			{
				dial_number++;
				if(dial_number==1)
				{
					dial_fist_angle=Dial_motor.ecd;
				}
				if(dial_number>3) dial_number=3;
			}
			get_total_angle(&dial_totel);
			if(MODE==3)
			{
				dial_finish();
			}
		}
		if(STOP==2)
		{
				if(RC_CtrlData.mouse.press_l==1)
				{
					dial_sign=1;
				}
				if(dial_sign==1)
				{
					dial_number1++;
					if(dial_number1==1)
					{
						get_moto_offset(&dial_data);
					}
					if(dial_number1>3) dial_number1=3;

					get_total_angle(&dial_data);
				}
			if(RC_CtrlData.mouse.press_r==1)
			{
				dial_back_sign=1;
			}
			if(dial_back_sign==1)
			{
				dial_number2++;
				if(dial_number2==1)
				{
					get_back_offset(&dial_data);
				}
				if(dial_number2>3) dial_number2=3;

				get_total_angle(&dial_data);

			}		
		}
}
}
*/