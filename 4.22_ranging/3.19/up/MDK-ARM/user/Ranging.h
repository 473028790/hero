#ifndef __RANGING_H
#define __RANGING_H
/*---------------------INCLUDES----------------------*/
#include "main.h"
#include "math.h"
#include "usart.h"
#include "stdio.h"

void f_ranging_init(void);
void f_fastConti_Cmd(void); 
void f_clear_L1Mod_Usart(void);
void f_conti_cmd(void);
void f_ranging_state(void);
void f_ranging_data_handle(void);
void distance_test(void);





#include "arm_math.h"

/*---------------------VARIABLES---------------------*/
#define SERVO_ERROR 12

#define iLD_1 7
#define iLD_0 6
#define iCM 2
#define iACM 3
#define iFACM 4
#define iHALT 5
#define iINT 10

#define LASER_BUF_NUM 8
#define LASER_BUF_LEN 8
#define LASER_FIFO_BUF_NUM 32
#define LASER_FIFO_BUF_LEN 32

#define AMMOK 0.00469f //0.0027
#define AMMOG 9.7803f  //深圳重力加速度
#define AMMOM 0.043f   //发光弹丸质量
#define AMMO1ONE 1.0f  //1

//180
#ifndef PI
#define PI 3.14159265358979f
#endif
//90
#ifndef PIp2
#define PIp2 1.57079632679489f
#endif
//45
#ifndef PIp4
#define PIp4 0.78539816339744f
#endif
//-90
#ifndef PIp2_negative
#define PIp2_negative -1.5707963267948f
#endif
//-15
#ifndef PIp12_negative
#define PIp12_negative -0.2617993877991f
#endif
//30
#ifndef PIp6
#define PIp6 0.52359877559829f
#endif
//-30
#ifndef PIp6_negative
#define PIp6_negative -0.5235987755982f
#endif
//1 rad
#ifndef ONERAD
#define ONERAD 57.2957795130823f
#endif

/*---------------------FUNCTIONS---------------------*/
//数据结构体
typedef struct
{

  float GL_angel;        //激光对水平角度 角度值
  float Caluculat_angel; //解算角,炮管对水平 角度制
  float TOA;             //炮弹飞行时间

  float L1_Distance;  //L1测得的角度
  float L1_Angel;     //L1测得数据时的夹角 角度值
  float L1_AmmoSpeed; //弹速

  float GB_angel;       //枪管对水平角度 角度值
  float BL_angel;       //激光对枪管角度（中位90°） 角度值
  float Delta_B;        //实时运算 计算炮管角度与实际角度差 上正
  uint8_t L1_ACM_State; //测量是否可信
  uint8_t L1_Operation; //指令

  uint8_t E_FPS; //有效帧数
  uint8_t tE_FPS;

} L1_DATA_T;
extern L1_DATA_T L1_Data;


void distance_test(void);
#endif


