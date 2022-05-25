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
float last_store[3];	//���������ǲ����ĽǶ�

int16_t store[3]; 			//���������ǲ����Ľ��ٶ�

void Incremental_PID(struct motor *motor);
void location_PID(struct motor *motor);
void Gimbal_inner_PID(struct gimbal_inner_pid *motor);
void Gimbal_outer_PID(struct gimbal_outer_pid *motor);



void PID_init()            //����PID��ʼ��
{ 
   wheel_pid_init();      //����pid��ʼ��
   dial_pid_init();       //���̳�ʼ��
   Gimbal_PID_init();     //��̨��ʼ��
   friction_PID_init();   //Ħ���ֳ�ʼ��

}

void dial_pid_init()       //����PID��ʼ��
{
   dial_motor.target_speed=0.0;
   dial_motor.ActualSpeed=0.0;

   dial_motor.kp=3.4;
   dial_motor.ki=0.013;
   dial_motor.kd=0.015;

   dial_motor.Pr_Error=0;
   dial_motor.thisError=0.0;
   dial_motor.Last_Error=0.0;
   dial_motor.increment=0;
   dial_inner_pid_init();
   dial_outer_pid_init();
   dial_data.state=0;
}

void wheel_pid_init()      //����PID��ʼ��
{
   int8_t motor_id;

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
      wheel_moter[motor_id].target_speed=0.0;
      wheel_moter[motor_id].ActualSpeed=0.0;

      wheel_moter[motor_id].Pr_Error=0;
      wheel_moter[motor_id].thisError=0.0;
      wheel_moter[motor_id].Last_Error=0.0;
   }
}

void Gimbal_PID_init()     //��̨PID��ʼ�� 
{
   gimbal_yaw_pid_init();
   gimbal_pitch_pid_init();
}
void gimbal_yaw_pid_init()                   //yaw���⻷PID��ʼ��
{
   gimbal_yaw_inner_pid_init();
   gimbal_yaw_outer_pid_init();
}
void gimbal_yaw_outer_pid_init()             //yaw���⻷PID��ʼ��
{
   yaw_outer_pid.target=0.0;
   yaw_outer_pid.ActualSpeed=0.0;

   yaw_outer_pid.kp=10.0;
   yaw_outer_pid.ki=0;
   yaw_outer_pid.kd=0;
   yaw_outer_pid.integral=0;

   yaw_outer_pid.voltage=0;
   yaw_outer_pid.thisError=0.0;
   yaw_outer_pid.Last_Error=0.0;
   yaw_outer_pid.judge=1;
   yaw_outer_pid.out=0;
}
void gimbal_yaw_inner_pid_init()             //yaw���ڻ�PID��ʼ��
{
   yaw_inner_pid.target=0.0;
   yaw_inner_pid.ActualSpeed=0.0;

   yaw_inner_pid.kp=300.0;
   yaw_inner_pid.ki=0.07;
   yaw_inner_pid.kd=5;
   yaw_inner_pid.integral=0;

   yaw_inner_pid.voltage=0;
   yaw_inner_pid.thisError=0.0;
   yaw_inner_pid.Last_Error=0.0;
}

void gimbal_pitch_pid_init()                   //pitch���⻷PID��ʼ��
{
   gimbal_pitch_inner_pid_init();
   gimbal_pitch_outer_pid_init();
}
void gimbal_pitch_outer_pid_init()             //pitch���⻷PID��ʼ��
{
   pitch_outer_pid.target=0.0;
   pitch_outer_pid.ActualSpeed=0.0;

   pitch_outer_pid.kp=10.0;
   pitch_outer_pid.ki=0;
   pitch_outer_pid.kd=0;
   pitch_outer_pid.integral=0;

   pitch_outer_pid.voltage=0;
   pitch_outer_pid.thisError=0.0;
   pitch_outer_pid.Last_Error=0.0;
   pitch_outer_pid.judge=0;
   pitch_outer_pid.out=0;
}
void gimbal_pitch_inner_pid_init()             //pitch���ڻ�PID��ʼ��
{
   pitch_inner_pid.target=0.0;
   pitch_inner_pid.ActualSpeed=0.0;

   pitch_inner_pid.kp=129.0;
   pitch_inner_pid.ki=1.24;
   pitch_inner_pid.kd=2;
   pitch_inner_pid.integral=0;

   pitch_inner_pid.voltage=0;
   pitch_inner_pid.thisError=0.0;
   pitch_inner_pid.Last_Error=0.0;
}

void dial_outer_pid_init()             //�����⻷PID��ʼ��
{
   dial_outer_pid.target=0.0;
   dial_outer_pid.ActualSpeed=0.0;

   dial_outer_pid.kp=4.8;
   dial_outer_pid.ki=0;
   dial_outer_pid.kd=0;
   dial_outer_pid.out=0;

   dial_outer_pid.integral=0;
   dial_outer_pid.voltage=0;
   dial_outer_pid.thisError=0.0;
   dial_outer_pid.Last_Error=0.0;
   dial_outer_pid.judge=3;
}
void dial_inner_pid_init()             //�����ڻ�PID��ʼ��
{
   dial_inner_pid.target=0.0;
   dial_inner_pid.ActualSpeed=0.0;

   dial_inner_pid.kp=10.0;
   dial_inner_pid.ki=0.09;
   dial_inner_pid.kd=0;
   dial_inner_pid.integral=0;
   dial_inner_pid.out=0;

	dial_inner_pid.judge=3;
   dial_inner_pid.voltage=0;
   dial_inner_pid.thisError=0.0;
   dial_inner_pid.Last_Error=0.0;
}


void friction_PID_init()   //Ħ����PID��ʼ��
{
   int motor=0;

      Friction_motor[0].kp=5.0;
      Friction_motor[0].ki=0.03;
      Friction_motor[0].kd=0;

      Friction_motor[1].kp=5.0;
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

void Chassis_PID()          //����4���������pid����
{
	int8_t motor_id;
   for(motor_id=0;motor_id<4;motor_id++)
   {
      Incremental_PID(&wheel_moter[motor_id]);
   }
}

void Gimbal_PID()           //��̨PID����
{
   get_eular(last_store);
   get_raw_gyo(store);
   pitch_outer_pid.ActualSpeed=last_store[1];     //�����ĽǶ�����Ϊpitch�⻷��ʵ��ֵ
   pitch_inner_pid.ActualSpeed=store[0]*0.1;   //����x��ת���Ľ��ٶȣ�pitch�ᣩ��ֵ�ṹ���ڻ�
   yaw_outer_pid.ActualSpeed=last_store[2];     //�����ĽǶ�����Ϊ�⻷��ʵ��ֵ
   yaw_inner_pid.ActualSpeed=store[2]*0.1;   //����z��ת���Ľ��ٶȣ�yaw�ᣩ��ֵ�ṹ���ڻ�

   if(pitch_outer_pid.target>=25.0f+Degree_po) pitch_outer_pid.target=25.0f+Degree_po;         //�������޷�
   else if(pitch_outer_pid.target<=-14.0f+Degree_po) pitch_outer_pid.target=-14.0f+Degree_po;

   //yaw��Ĵ���PID

   Gimbal_outer_PID(&yaw_outer_pid);
   yaw_inner_pid.target=yaw_outer_pid.out;
   Gimbal_inner_PID(&yaw_inner_pid);

   //pitch��Ĵ���PID

   Gimbal_outer_PID(&pitch_outer_pid);
   pitch_inner_pid.target=pitch_outer_pid.out;
   Gimbal_inner_PID(&pitch_inner_pid);

}

void Friction_PID(void)         //Ħ����PID����
{ 
   //����Ħ������
   
   if(MODE==1)
   {
      Friction_motor[0].target_speed=-Friction;              //ͷ�ļ�������ٶȳ���
      Friction_motor[1].target_speed=Friction;
      //Friction_motor[1].target_speed=(-Friction);
   }
   if(MODE==3)
	{
		Friction_motor[0].target_speed=0;
		Friction_motor[1].target_speed=0;
	}
   

   //Incremental_PID(&Friction_motor[0]);
   location_PID(&Friction_motor[0]);
   location_PID(&Friction_motor[1]);
   //Incremental_PID(&Friction_motor[1]);
}

#define ABS(x)	( (x>0) ? (x) : (-x) )
int cnt21=0;
//����Ȧ���Ĵ���
void get_total_angle(struct dial_data *p)
{
	int res1, res2, delta;
	if(p->angle < p->last_angle){			//���ܵ����
		res1 = p->angle + 8192 - p->last_angle;	//��ת��delta=+
		res2 = p->angle - p->last_angle;				//��ת	delta=-
	}else{	//angle > last
		res1 = p->angle - 8192 - p->last_angle ;//��ת	delta -
		res2 = p->angle - p->last_angle;				//��ת	delta +
	}
	//��������ת���϶���ת�ĽǶ�С���Ǹ������
	if(ABS(res1)<ABS(res2))
		delta = res1;
	else
		delta = res2;
	cnt21+=delta;
   delta=(360*delta)/8191;
 
   p->total_angle += delta;
	p->last_angle = p->angle;
}

//ȷ������Ŀ��Ȧ��
void get_moto_offset(struct dial_data *ptr)
{
	ptr->angle_first=ptr->angle;
	ptr->angle_set=68550-ptr->angle_first;
   ptr->angle_set=(360*(ptr->angle_set))/8191;
}
int dial_mode;
extern int dial_sign;
void dial_PID()             //����PID����
{
	dial_mode=hero;//����ģʽѡ��
	
	if(dial_mode==hero)
	{
         dial_outer_pid.target=dial_data.angle_set;
         dial_outer_pid.ActualSpeed=dial_data.total_angle;
         dial_inner_pid.ActualSpeed=dial_motor.ActualSpeed;

         Gimbal_outer_PID(&dial_outer_pid);
         dial_inner_pid.target=dial_outer_pid.out;
         Gimbal_inner_PID(&dial_inner_pid);

         if(fabs(dial_outer_pid.ActualSpeed-dial_outer_pid.target)<10)     //ȷ�����������н�������
         {
            dial_sign=0;
            dial_number=0;
            dial_number1=0;
            dial_data.total_angle=0;
            dial_data.angle_first=0;
            dial_data.angle_set=0;
            dial_data.last_angle=0;
            dial_data.llast_angle=0;

            dial_finish();
         }

   }

	if(dial_mode==infantry)
	{
      Incremental_PID(&dial_motor);
   }
}

void Incremental_PID(struct motor *motor) //����ʽPIDͨ��
{
   motor->thisError=(motor->target_speed-motor->ActualSpeed);

   motor->increment=motor->kp*(motor->thisError-motor->Last_Error)+motor->ki*motor->thisError
   +motor->kd*(motor->thisError-2*motor->Last_Error+motor->Pr_Error);

   motor->Pr_Error=motor->Last_Error;
   motor->Last_Error=motor->thisError;

   motor->out+=motor->increment;
   if(motor->out>=15000)   motor->out=15000;
   if(motor->out<=-15000)  motor->out=-15000;
}

void location_PID(struct motor *motor) //λ��ʽPIDͨ��
{
   motor->thisError=(motor->target_speed-motor->ActualSpeed);
   motor->integral+=motor->thisError;
   if(motor->integral>=10000) motor->integral=10000;
   if(motor->integral<=-10000) motor->integral=-10000;

   motor->voltage=motor->kp*motor->thisError+motor->ki*motor->integral+
                  motor->kd*(motor->thisError-motor->Last_Error);
   motor->Last_Error=motor->thisError;
   motor->out=motor->voltage;

   if(motor->out>=15000)   motor->out=15000;
   if(motor->out<=-15000)  motor->out=-15000;
}
void Gimbal_outer_PID(struct gimbal_outer_pid *motor) //��̨�⻷λ��ʽPIDר��
{
   motor->thisError=(motor->target-motor->ActualSpeed);

   if(motor->judge==1)
   {
      if(motor->thisError>179.9f &&motor->target<=179.9f&&motor->target>=-179.9f)
      {
         motor->thisError-=360;        //�߽紦��
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
   if(motor->out>=6000)   motor->out=6000;
   if(motor->out<=-6000)  motor->out=-6000;
   }
}

void Gimbal_inner_PID(struct gimbal_inner_pid *motor) //��̨�ڻ�λ��ʽPIDר��
{
   motor->thisError=(motor->target-motor->ActualSpeed);

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


