#include "Revolver.h"
#include "main.h"
#include "can.h"
#define    	AN_BULLET         (3012.81f)		//单个子弹电机位置增加值

/*****************************卡弹处理**************************************/

#define Stuck_Revol_PIDTerm   4000      //PID输出大于这个数则认为有可能卡弹
#define Stuck_Speed_Low       60       //测量速度低于这个数,则认为有可能卡弹

#define Stuck_SpeedPID_Time   100       //速度连续 ms过小,PID连续  ms过大
#define Stuck_TurnBack_Time   100       //倒转时间,时间越长倒得越多
uint32_t Stuck_Posit_Sum = 0;//计算卡弹次数,位置环


/*
  * @brief  位置环式卡弹处理
  * @param  void
  * @retval void
  * @attention  卡住就反转n格
  */
	
void REVOL_PositStuck(void)
{
	static uint16_t  stuck_time      = 0;//卡弹计时
	static uint16_t  turnback_time   = 0;//倒转计时
	static bool Revol_Posit_ifStuck = FALSE;//卡弹判断
	
	if (Revol_Posit_ifStuck == TRUE)//卡弹后开始倒转计时
	{
		//卡弹了则在判断是否卡弹这段时间内鼠标按下的指令都清零
		//Key_ShootNum = 0;
		
		turnback_time++;//倒转计时,1ms一次
		if (turnback_time > Stuck_TurnBack_Time)//倒转时间够了
		{
			dial_data.angle_set = dial_data.total_angle;//正常旋转,旋转回本来想要它转到的位置
			//Revolver_Buff_Target_Sum = dial_data.angle_set;
			Revol_Posit_ifStuck = FALSE;//认为此时不再卡弹了
			turnback_time = 0;//倒转时间清零,为下次倒转做准备	
		}
	}
	else
	{
		if ( abs(dial_inner_pid.out)  >= Stuck_Revol_PIDTerm //PID输出过大
				&& abs(dial_motor.ActualSpeed) <= Stuck_Speed_Low  )//速度过低
		{
			stuck_time++;//统计卡了多长时间
		}
		else
		{
			stuck_time = 0;//不卡了,时间清零
		}
		
		if (stuck_time > Stuck_SpeedPID_Time)//卡太久了,提示要倒转
		{
			//倒转不能放在Revol_Posit_ifStuck == TRUE中,否则就不是堵一次倒转1/2格了
			dial_data.angle_set = dial_data.total_angle - AN_BULLET ;//倒转 1格  
			//Revolver_Buff_Target_Sum = Revolver_Angle_Target_Sum;
			
			Stuck_Posit_Sum++;//卡弹计数,给机械组的大兄弟点面子
			stuck_time = 0;
			Revol_Posit_ifStuck = TRUE;//用来标记倒转计时开启	
		}
	}
}



