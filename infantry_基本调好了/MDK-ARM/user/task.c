#include "task.h"
#include "pid.h"
#include "can.h"
#include "rc.h"
int cnt5=0;
int16_t chassis_power_max;
void Offline_task()                 //离线
{
    cnt5++;
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
        pitch_outer_pid.target=0;
        pitch_outer_pid.integral=0;
        pitch_outer_pid.voltage=0;
        pitch_outer_pid.thisError=0;
        pitch_outer_pid.Last_Error=0;

        yaw_outer_pid.out=0;
        yaw_outer_pid.target=0;
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

void CAN1_send()                    //CAN1
{
    //CAN1_0x200_TX(wheel_moter[0].out,wheel_moter[1].out,wheel_moter[2].out,wheel_moter[3].out);
    //CAN1_0x200_TX(0,0,0,wheel_moter[3].out);
    //测试yaw
    CAN1_0x1FF_TX(yaw_inner_pid.out, 0, 0, 0);

}
void CAN2_send()                    //CAN2
{
    
    //CAN2_0x200_TX(Friction_motor[0].out,Friction_motor[1].out, 0, 0);
	/*
    if(dial_mode_choose==hero)
    {
        CAN2_0x1FF_TX(0,pitch_inner_pid.out, dial_inner_pid.out, 0);
    }
    if(dial_mode_choose==infantry)
    {
        CAN2_0x1FF_TX(0,pitch_inner_pid.out, dial_motor.out, 0);
    }
    */ 

    //测试拨盘用
    //CAN2_0x1FF_TX(0,0, dial_inner_pid.out, 0);

    //测试left摩擦轮用
    //CAN2_0x200_TX(Friction_motor[0].out,0, 0, 0);
    //测试right摩擦轮用
    //CAN2_0x200_TX(0,Friction_motor[1].out, 0, 0);

    //测试pitch
    //CAN2_0x1FF_TX(0,-(pitch_inner_pid.out), 0, 0);
}

