#include "pid.h"
#include "can.h"
#include "rc.h"
#include "imu_data_decode.h" 
#include "math.h"

float change_gy;
float change_degree;
float change_ecd;
float Degree_po=0;
extern RC_Ctl_t RC_CtrlData;
extern uint8_t MODE;
extern uint8_t STOP;
extern int16_t L_Y;
extern int16_t L_X;
extern int16_t R_Y;
extern int16_t R_X;
extern int dial_number;
extern int dial_number1;
extern int dial_number2;
extern KEY	KEY_Date;

float last_store[3];	//储存陀螺仪测量的角度

int16_t store[3]; 			//储存陀螺仪测量的角速度

void Incremental_PID(struct motor *motor);
void location_PID(struct motor *motor);
void Gimbal_inner_PID(struct gimbal_inner_pid *motor);
void Gimbal_outer_PID(struct gimbal_outer_pid *motor);
/*
typedef struct
{
	float rin;
	float lastRin;
	float perrRin;
}FFC;

float FeedforwardController(FFC *vFFC)
{
	float result;
	
	
	result=a*(vFFC->rin-vFFC->lastRin)+b*(vFFC->rin-2*vFFC->lastRin+vFFC->lastRin+vFFC->perrRin);
	
	vFFC->perrRin=vFFC->lastRin;
	vFFC->lastRin=vFFC->rin;
	
	return result;
}
*/
void PID_init()            //集合PID初始化
{ 
   wheel_pid_init();      //轮子pid初始化
   dial_pid_init();       //拨盘初始化
   Gimbal_PID_init();     //云台初始化
   friction_PID_init();   //摩擦轮初始化

}

void dial_pid_init()       //拨盘PID初始化
{
   dial_motor.target_speed=0.0;
   dial_motor.ActualSpeed=0.0;

   dial_motor.kp=6;
   dial_motor.ki=0.1;
   dial_motor.kd=0;

   dial_motor.Pr_Error=0;
   dial_motor.thisError=0.0;
   dial_motor.Last_Error=0.0;
   dial_motor.increment=0;
   
   dial_inner_pid.target=0.0;
   dial_inner_pid.integral=0.0;
   dial_inner_pid.judge=3;

   dial_outer_pid.target=0.0;
   dial_outer_pid.integral=0.0;
   dial_outer_pid.judge=3;

   dial_inner_pid_init();
   dial_outer_pid_init();
   dial_data.state=0;
}

void wheel_pid_init()      //轮子PID初始化
{
   int motor_id=0;

      wheel_moter[0].kp=10.5;
      wheel_moter[0].ki=0.33;
      wheel_moter[0].kd=0.01;

      wheel_moter[1].kp=10.5;
      wheel_moter[1].ki=0.35;
      wheel_moter[1].kd=0.01;

      wheel_moter[2].kp=10.5;
      wheel_moter[2].ki=0.33;
      wheel_moter[2].kd=0.01;

      wheel_moter[3].kp=10.5;
      wheel_moter[3].ki=0.38;
      wheel_moter[3].kd=0.01;
	
   for(motor_id=0;motor_id<4;motor_id++)
   {
      wheel_moter[motor_id].voltage=0;
      wheel_moter[motor_id].ActualSpeed=0;
      wheel_moter[motor_id].target_speed=0;
      wheel_moter[motor_id].Pr_Error=0;
      wheel_moter[motor_id].Last_Error=0;
      wheel_moter[motor_id].thisError=0;
      wheel_moter[motor_id].integral=0;
      wheel_moter[motor_id].increment=0;
      wheel_moter[motor_id].out=0;
   }
	 
	 
	 
}

void Gimbal_PID_init()     //云台PID初始化 
{
   gimbal_yaw_pid_init();
   gimbal_pitch_pid_init();
}
void gimbal_yaw_pid_init()                   //yaw内外环PID初始化
{
   gimbal_yaw_inner_pid_init();
   gimbal_yaw_outer_pid_init();
}
void gimbal_yaw_outer_pid_init()             //yaw轴外环PID初始化
{
   yaw_outer_pid.target=0.0;
   yaw_outer_pid.ActualSpeed=0.0;

   yaw_outer_pid.kp=10;
   yaw_outer_pid.ki=0;
   yaw_outer_pid.kd=0;
   yaw_outer_pid.integral=0;

   yaw_outer_pid.voltage=0;
   yaw_outer_pid.thisError=0.0;
   yaw_outer_pid.Last_Error=0.0;
   yaw_outer_pid.judge=1;
   yaw_outer_pid.out=0;
}
void gimbal_yaw_inner_pid_init()             //yaw轴内环PID初始化
{
   yaw_inner_pid.target=0.0;
   yaw_inner_pid.ActualSpeed=0.0;

   yaw_inner_pid.kp=200.0;
   yaw_inner_pid.ki=0.2;
   yaw_inner_pid.kd=0;
   yaw_inner_pid.integral=0;

   yaw_inner_pid.voltage=0;
   yaw_inner_pid.thisError=0.0;
   yaw_inner_pid.Last_Error=0.0;
}

void gimbal_pitch_pid_init()                   //pitch内外环PID初始化
{
   gimbal_pitch_inner_pid_init();
   gimbal_pitch_outer_pid_init();
}
void gimbal_pitch_outer_pid_init()             //pitch轴外环PID初始化
{
   pitch_outer_pid.target=0.0;
   pitch_outer_pid.ActualSpeed=0.0;

   pitch_outer_pid.kp=10.6;
   pitch_outer_pid.ki=0;
   pitch_outer_pid.kd=0;
   pitch_outer_pid.integral=0;

   pitch_outer_pid.voltage=0;
   pitch_outer_pid.thisError=0.0;
   pitch_outer_pid.Last_Error=0.0;
   pitch_outer_pid.judge=0;
   pitch_outer_pid.out=0;
}
void gimbal_pitch_inner_pid_init()             //pitch轴内环PID初始化
{
   pitch_inner_pid.target=0.0;
   pitch_inner_pid.ActualSpeed=0.0;

   pitch_inner_pid.kp=150;
   pitch_inner_pid.ki=1.4;
   pitch_inner_pid.kd=0;
   pitch_inner_pid.integral=0;

   pitch_inner_pid.voltage=0;
   pitch_inner_pid.thisError=0.0;
   pitch_inner_pid.Last_Error=0.0;
}

void dial_outer_pid_init()             //拨盘外环PID初始化
{
   dial_outer_pid.target=0.0;
   dial_outer_pid.ActualSpeed=0.0;

   dial_outer_pid.kp=5.3;
   dial_outer_pid.ki=0;
   dial_outer_pid.kd=0;
   dial_outer_pid.out=0;

   dial_outer_pid.integral=0;
   dial_outer_pid.voltage=0;
   dial_outer_pid.thisError=0.0;
   dial_outer_pid.Last_Error=0.0;
   dial_outer_pid.judge=3;
}
void dial_inner_pid_init()             //拨盘内环PID初始化
{
   dial_inner_pid.target=0.0;
   dial_inner_pid.ActualSpeed=0.0;

   dial_inner_pid.kp=15.0;
   dial_inner_pid.ki=0.40;
   dial_inner_pid.kd=0;
   dial_inner_pid.integral=0;
   dial_inner_pid.out=0;

	dial_inner_pid.judge=3;
   dial_inner_pid.voltage=0;
   dial_inner_pid.thisError=0.0;
   dial_inner_pid.Last_Error=0.0;
}


void friction_PID_init()   //摩擦轮PID初始化
{
   int motor=0;

      Friction_motor[0].kp=3.8;
      Friction_motor[0].ki=0.017;
      Friction_motor[0].kd=0;

      Friction_motor[1].kp=3.8;
      Friction_motor[1].ki=0.02;
      Friction_motor[1].kd=0;

   for(motor=0;motor<2;motor++)
   {
      Friction_motor[motor].voltage=0;
      Friction_motor[motor].ActualSpeed=0;
      Friction_motor[motor].target_speed=0;
      Friction_motor[motor].Pr_Error=0;
      Friction_motor[motor].Last_Error=0;
      Friction_motor[motor].thisError=0;
      Friction_motor[motor].integral=0;
      Friction_motor[motor].increment=0;
      Friction_motor[motor].out=0;
   }
}
extern int16_t CAN1_0X200[4];
void Chassis_PID()          //底盘4个电机进行pid运算
{
	
		Incremental_PID(&wheel_moter[0]);
		Incremental_PID(&wheel_moter[1]);
		Incremental_PID(&wheel_moter[2]);
		Incremental_PID(&wheel_moter[3]);
	
		//location_PID(&wheel_moter[0]);
		//location_PID(&wheel_moter[1]);
		//location_PID(&wheel_moter[2]);
		//location_PID(&wheel_moter[3]);
	
	
		CAN1_0X200[0]=wheel_moter[0].out;
		CAN1_0X200[1]=wheel_moter[1].out;
		CAN1_0X200[2]=wheel_moter[2].out;
		CAN1_0X200[3]=wheel_moter[3].out;
	
}
int GYR_first=0;
void Gimbal_PID()           //云台PID运算
{
   get_eular(last_store);
   get_raw_gyo(store);
   pitch_outer_pid.ActualSpeed=last_store[0];     //反馈的角度设置为pitch外环的实际值
   pitch_inner_pid.ActualSpeed=store[1]*0.1;   //将绕x轴转动的角速度（pitch轴）赋值结构体内环
   yaw_outer_pid.ActualSpeed=last_store[2];     //反馈的角度设置为外环的实际值
   yaw_inner_pid.ActualSpeed=store[2]*0.1;   //将绕z轴转动的角速度（yaw轴）赋值结构体内环

	 GYR_first++;
	if(GYR_first<5)
	{
		pitch_outer_pid.target=pitch_outer_pid.ActualSpeed;
		yaw_outer_pid.target=yaw_outer_pid.ActualSpeed;
	}
	else if(GYR_first>5) GYR_first=6;
	
   if(pitch_outer_pid.target>=35.0f+Degree_po) pitch_outer_pid.target=35.0f+Degree_po;         //俯仰角限幅
   else if(pitch_outer_pid.target<=-25.0f+Degree_po) pitch_outer_pid.target=-25.0f+Degree_po;

   //yaw轴的串级PID

   Gimbal_outer_PID(&yaw_outer_pid);
   yaw_inner_pid.target=yaw_outer_pid.out;
   Gimbal_inner_PID(&yaw_inner_pid);

   //pitch轴的串级PID

   Gimbal_outer_PID(&pitch_outer_pid);
   pitch_inner_pid.target=pitch_outer_pid.out;
   Gimbal_inner_PID(&pitch_inner_pid);

}

void Friction_PID(void)         //摩擦轮PID运算
{ 
   //测试摩擦轮用
   if(STOP==1)
   {
         Friction_motor[0].target_speed=-Friction;              //头文件里面改速度常量
         Friction_motor[1].target_speed=Friction;
		 /*
      if(MODE==1)
      {
         Friction_motor[0].target_speed=-Friction;              //头文件里面改速度常量
         Friction_motor[1].target_speed=Friction;
         //Friction_motor[1].target_speed=(-Friction);
      }
      if(MODE==3)
      {
				
         Friction_motor[0].target_speed=0;
         Friction_motor[1].target_speed=0;
      }
		 */
   }
   if(STOP==2)
   {
         Friction_motor[0].target_speed=-Friction;              //头文件里面改速度常量
         Friction_motor[1].target_speed=Friction;      
   }

   //Incremental_PID(&Friction_motor[0]);
   location_PID(&Friction_motor[0]);
   location_PID(&Friction_motor[1]);
   //Incremental_PID(&Friction_motor[1]);
}

#define ABS(x)	( (x>0) ? (x) : (-x) )
int cnt21=0;
//计算圈数的代码
void get_total_angle(struct dial_data *p)
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
	cnt21+=delta;
   delta=(360*delta)/8191;
 
   p->total_angle += delta;
	p->last_angle = p->angle;
}

float dial_turns=1.6;
//确定拨盘目标圈数
void get_moto_offset(struct dial_data *ptr)
{
	ptr->angle_first=ptr->angle;
	ptr->angle_set=((int)(dial_turns*(31593)))-ptr->angle_first;
   ptr->angle_set=(360*(ptr->angle_set))/8191;
}


//确定拨盘反转目标圈数
void get_back_offset(struct dial_data *ptr)
{
	ptr->angle_first=ptr->angle;
	ptr->angle_set=-8191+ptr->angle_first;
   ptr->angle_set=(360*(ptr->angle_set))/8191;
}



//int dial_mode=infantry;
int dial_mode=hero;
extern int dial_sign;
extern int dial_sign1;
extern int infra_red_GPIO;
void dial_PID()             //拨盘PID运算
{
	
	if(dial_mode==hero)
	{
         dial_outer_pid.target=dial_data.angle_set;
         dial_outer_pid.ActualSpeed=dial_data.total_angle;
         dial_inner_pid.ActualSpeed=dial_motor.ActualSpeed;

         Gimbal_outer_PID(&dial_outer_pid);
         dial_inner_pid.target=dial_outer_pid.out;
         Gimbal_inner_PID(&dial_inner_pid);
		
		
		if(STOP==1)
		{
         if(fabs(dial_outer_pid.ActualSpeed-dial_outer_pid.target)<15)     //确定结束后，运行结束程序
         {
				dial_data.total_angle=0;
				dial_data.angle_first=0;
				dial_data.angle_set=0;

         }
			
		}
		else if(STOP==2)
		{
         if(fabs(dial_outer_pid.ActualSpeed-dial_outer_pid.target)<15)     //确定结束后，运行结束程序
         {
            dial_sign1=0;
            dial_number1=0;
            

				dial_data.total_angle=0;
				dial_data.angle_first=0;
				dial_data.angle_set=0;

         }
		}
   }

	if(dial_mode==infantry)
	{
      Incremental_PID(&dial_motor);
   }
}

void Incremental_PID(struct motor *motor) //增量式PID通用
{
   motor->thisError=(motor->target_speed-motor->ActualSpeed);

   motor->increment=motor->kp*(motor->thisError-motor->Last_Error)+motor->ki*motor->thisError
   +motor->kd*(motor->thisError-2*motor->Last_Error+motor->Pr_Error);

   motor->Pr_Error=motor->Last_Error;
   motor->Last_Error=motor->thisError;

   motor->out+=motor->increment;
   if(motor->out>=10000)   motor->out=10000;
   if(motor->out<=-10000)  motor->out=-10000;
}

void location_PID(struct motor *motor) //位置式PID通用
{
   motor->thisError=(motor->target_speed-motor->ActualSpeed);
   motor->integral+=motor->thisError;
   if(motor->integral>=10000) motor->integral=10000;
   if(motor->integral<=-10000) motor->integral=-10000;

   motor->voltage=motor->kp*motor->thisError+motor->ki*motor->integral+
                  motor->kd*(motor->thisError-motor->Last_Error);
   motor->Last_Error=motor->thisError;
   motor->out=motor->voltage;

   if(motor->out>=10000)   motor->out=10000;
   if(motor->out<=-10000)  motor->out=-10000;
}


void Gimbal_outer_PID(struct gimbal_outer_pid *motor) //云台外环位置式PID专用
{
   motor->thisError=(motor->target-motor->ActualSpeed);

   if(motor->judge==1)
   {
      if(motor->thisError>179.9f &&motor->target<=179.9f&&motor->target>=-179.9f)
      {
         motor->thisError-=360;        //边界处理
      }
      else if(motor->thisError<-179.9f &&motor->target<=179.9f&&motor->target>=-179.9f) 
      {
         motor->thisError+=360;
      }
   }

   motor->integral+=motor->thisError;

   if(motor->integral>=25000) motor->integral=25000;
	if(motor->integral<=-25000)  motor->integral=-25000;

   motor->voltage=motor->kp*motor->thisError
   +motor->ki*motor->integral+motor->kd*(motor->thisError-motor->Last_Error);

   motor->Last_Error=motor->thisError;

   motor->out=motor->voltage;
   if(motor->out>=30000)   motor->out=30000;
   if(motor->out<=-30000)  motor->out=-30000;
	 
   if(motor->judge==3)
   {
   if(motor->out>=2000)   motor->out=2000;
   if(motor->out<=-2000)  motor->out=-2000;
   }
}

void Gimbal_inner_PID(struct gimbal_inner_pid *motor) //云台内环位置式PID专用
{
   motor->thisError=(motor->target-motor->ActualSpeed);

   motor->integral+=motor->thisError;

   if(motor->integral>=25000) motor->integral=25000;
	if(motor->integral<=-25000)  motor->integral=-25000;

	 if(motor->judge==3)
   {
   if(motor->integral>=10000)   motor->integral=10000;
   if(motor->integral<=-10000)  motor->integral=-10000;
   }

   motor->voltage=motor->kp*motor->thisError
   +motor->ki*motor->integral+motor->kd*(motor->thisError-motor->Last_Error);

   motor->Last_Error=motor->thisError;

   motor->out=motor->voltage;
   if(motor->out>=30000)   motor->out=30000;
   if(motor->out<=-30000)  motor->out=-30000;
	
	 if(motor->judge==3)
   {
   if(motor->out>=10000)   motor->out=10000;
   if(motor->out<=-10000)  motor->out=-10000;
   }
}

void dial_finish(void)
{
   dial_number=0;
   dial_data.total_angle=0;
   dial_data.angle_first=0;
   dial_data.angle_set=0;

   dial_inner_pid.target=0.0;
   dial_inner_pid.integral=0.0;
   dial_inner_pid.judge=3;

   dial_outer_pid.target=0.0;
   dial_outer_pid.integral=0.0;
   dial_outer_pid.judge=3;
}


