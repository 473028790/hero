#include "offline.h"
#include "pid.h"
#include "can.h"
#include "rc.h"
#include "referee.h"
int16_t chassis_power_max;
extern int infra_red_GPIO;
extern int dial_sign1;
extern int dial_red_sign;
extern int dial_number1;
extern int dial_sign;
extern int dial_red;
extern int infra_red_MODE;
extern ext_game_robot_status_t My_status;//自身状态
void Offline_task()                 //离线
{
    int8_t motor_id;
    int8_t friction_id;
    if(STOP==3)
    {
        for(motor_id=0;motor_id<4;motor_id++)
        {
            wheel_moter[motor_id].target_speed=0;
            wheel_moter[motor_id].out=0;
            wheel_moter[motor_id].increment=0;
            wheel_moter[motor_id].thisError=0;
            wheel_moter[motor_id].Last_Error=0;
            wheel_moter[motor_id].Pr_Error=0;
        }
				dial_sign=0;
				dial_red=0;
				infra_red_MODE=0;
				
				
					dial_sign1=0;
					dial_red_sign=0;
					dial_number1=0;
        
					dial_data.total_angle=0;
					dial_data.angle_first=0;
					dial_data.angle_set=0;
				
				
        dial_outer_pid.out=0;
        dial_outer_pid.integral=0;
        dial_outer_pid.voltage=0;
        dial_outer_pid.thisError=0;
        dial_outer_pid.Last_Error=0;

        dial_inner_pid.out=0;
		dial_inner_pid.target=0;
        dial_inner_pid.integral=0;
        dial_inner_pid.voltage=0;
        dial_inner_pid.thisError=0;
        dial_inner_pid.Last_Error=0;

        dial_motor.out=0;
        dial_motor.ActualSpeed=0;
        dial_motor.target_speed=0;
        dial_motor.increment=0;
        dial_motor.Last_Error=0;
        dial_motor.Pr_Error=0;

        pitch_outer_pid.out=0;
        //pitch_outer_pid.target=0;
        pitch_outer_pid.integral=0;
        pitch_outer_pid.voltage=0;
        pitch_outer_pid.thisError=0;
        pitch_outer_pid.Last_Error=0;

        yaw_outer_pid.out=0;
        //yaw_outer_pid.target=0;
        yaw_outer_pid.integral=0;
        yaw_outer_pid.voltage=0;
        yaw_outer_pid.thisError=0;
        yaw_outer_pid.Last_Error=0;

        pitch_inner_pid.out=0;
        pitch_inner_pid.target=0;
        pitch_inner_pid.integral=0;
        pitch_inner_pid.voltage=0;
        pitch_inner_pid.thisError=0;
        pitch_inner_pid.Last_Error=0;

        yaw_inner_pid.out=0;
        yaw_inner_pid.integral=0;
        yaw_inner_pid.Last_Error=0;
        yaw_inner_pid.voltage=0;
        yaw_inner_pid.target=0;
        yaw_inner_pid.thisError=0;

        for(friction_id=0;friction_id<2;friction_id++)
        {
        Friction_motor[friction_id].integral=0;
        Friction_motor[friction_id].out=0;
        Friction_motor[friction_id].target=0;
        Friction_motor[friction_id].integral=0;
        Friction_motor[friction_id].voltage=0;
        Friction_motor[friction_id].thisError=0;
        Friction_motor[friction_id].Last_Error=0;
        }
    }
}
int16_t CAN1_0X200[4];

int16_t CAN1_0X1FF[4];

int16_t CAN2_0X200[4];
int16_t CAN2_0X1FF[4];
extern int dial_mode;
extern int chasses_pcb_rst;
extern double ranging_x;
extern RC_Ctl_t RC_CtrlData;
void CAN1_send()                    //CAN1
{
    //CAN1_0x200_TX(CAN1_0X200[0],CAN1_0X200[1],CAN1_0X200[2],CAN1_0X200[3]);
    //CAN1_0x200_TX(wheel_moter[0].out,wheel_moter[1].out,wheel_moter[2].out,wheel_moter[3].out);
    //CAN1_0x200_TX(0,0,0,wheel_moter[3].out);
	
		
		CAN1_0x642_TX(chasses_pcb_rst,((int16_t)ranging_x),RC_CtrlData.mouse.press_r,0,0);
		if(dial_mode==infantry) CAN1_0x1FF_TX(-(yaw_inner_pid.out), 0, dial_motor.out, 0);
		else if(dial_mode==hero) CAN1_0x1FF_TX(-(yaw_inner_pid.out), 0, dial_inner_pid.out, 0);
	
	/*
	if(dial_mode==infantry) CAN1_0x1FF_TX(-(yaw_inner_pid.out), 0, 0, 0);
	else if(dial_mode==hero) CAN1_0x1FF_TX(-(yaw_inner_pid.out), 0, 0, 0);
	*/
	

}
void CAN2_send()                    //CAN2
{
    
   CAN2_0x200_TX(Friction_motor[0].out,Friction_motor[1].out, 0, 0);

	
    //测试left摩擦轮用
    //CAN2_0x200_TX(Friction_motor[0].out,0, 0, 0);
    //测试right摩擦轮用
    //CAN2_0x200_TX(0,Friction_motor[1].out, 0, 0);

    //测试pitch
    CAN2_0x1FF_TX(0,-(pitch_inner_pid.out), 0, 0);
}

void CAN1_super()
{
	if(My_status.robot_level==1)
	{
		CAN1_0x333_TX(52);
	}
	else if(My_status.robot_level==2)
	{
		CAN1_0x333_TX(57);
	}
	else if(My_status.robot_level==3)
	{
		CAN1_0x333_TX(62);
	}
	else
	{
		CAN1_0x333_TX(48);
	}

}