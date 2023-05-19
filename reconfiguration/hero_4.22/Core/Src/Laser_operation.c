/*
 * @Description: Compensation Calculation of Laser Ranging
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-30 15:34:53
 */
#include "Laser_operation.h"
//安装误差修正

float PIT_CORRECT = -0.05f;

L1_ITERATION_T L1_Iteration;

int L1_Out_Angel;

L1_DATA_T L1_Data = {0.0f};

uint8_t Laser_buf[LASER_BUF_NUM];

char L1_TX_BUF[20] = {0};

//常数变量化，以便arm_math库调用
float AmmoK = AMMOK;          //0.0027
float AmmoG = AMMOG;          //深圳重力加速度
float AmmoM = AMMOM;          //发光弹丸质量
float AmmoMG = AMMOM * AMMOG; //发光弹丸重力
float AmmoM2GpK2 = AMMOM * AMMOM * AMMOG / (AMMOK * AMMOK);
float AmmoMGpK = AMMOM * AMMOG / AMMOK;
float AmmoKpM = AMMOK / AMMOM;
float AmmoMpK_negative = -AMMOM / AMMOK;
float Ammo1One = 1.0f;
float OneRad = ONERAD;
float OneRad_e = 0.0174532f;
float precision = 0.01f;

/***********************************************************************
** 函 数 名： Angel_approx(L1_DATA_T *L1_Data,L1_ITERATION_T *L1_Iteration)
** 函数说明： 计算θ仰角
** 注    释： 弹道计算 
**---------------------------------------------------------------------
** 输入参数： 数据结构体 迭代结构体
** 返回参数： 无
***********************************************************************/
void Angel_approx(L1_DATA_T *L1_Data, L1_ITERATION_T *L1_Iteration)
{ /**@brief超越式									  	
											 2
		  mg	   x		 sinθ			m g				    kx
	y =  ---- * -------- + -------- * x + ------- * ln( 1 - --------- )
		   k	V0*cosθ	 cosθ			 2				 mV0cosθ
											k 				 	
	
			x			   mg				
	exp1 = ----    exp2 = ---- * exp1   [P1]
			Vx				k

						  x					 sinθ
	exp3 = exp1 * Vy = -------- * V0sinθ = ------ * x [P2]
						V0cosθ				 cosθ
						
			k	   x		  kx					 k	    x		        kx							   kx
	exp4 = --- * ------  = ---------     exp5 = 1 -	--- * ------  = 1 -  ---------		exp6 = ln( 1 - --------- )				
			m	 V0cosθ	mV0cosθ				 m	  V0cosθ	      mV0cosθ						mV0cosθ
	
			 2
			m g				  kx
	exp7 = ----- * ln( 1 - --------- )  [P3]   exp8 = exp1 + exp3    	C_Dy = exp7 + exp8 
			  2				mV0cosθ
			 k 
	*/
  //函数运算全部采用arm_math库加速
  //theta角是待求角。alpha角是激光与水平夹角

  float Vx;
  float Vy;
  float Dx;
  float Dy;
  float C_Dy;

  float theta[2] = {PIp6_negative, PIp2};
  float theta_mid = 0.0f;

  float Sin_alpha;
  float Cos_alpha;
  float Sin_theta;
  float Cos_theta;
  //第一个分式
  float exp1;          // x/vo(cos(theta)) 慢！***无法加速***
  float exp2;          // (mg/k)*(exp1) P1
  float exp3;          // exp1*Vy  P2
  float exp4;          // k/m*exp1
  float exp5;          // 1-exp4
  float exp6;          //ln(exp5)
  float exp7;          // exp6^(m*m*G/(k*k)) P3
  float exp8;          //P1+P2
  float result = 0.0f; //P1+P2
  float abs_result = 0.0f;


  Sin_alpha = arm_sin_f32(L1_Data->L1_Angel);
  Cos_alpha = arm_cos_f32(L1_Data->L1_Angel);


  arm_mult_f32(&L1_Data->L1_Distance, &Cos_alpha, &Dx, 1);
  arm_mult_f32(&L1_Data->L1_Distance, &Sin_alpha, &Dy, 1);
  arm_add_f32(&Dy, &PIT_CORRECT, &Dy, 1); //修正距离
  L1_Data->L1_AmmoSpeed = 15.0f;
  if (L1_Data->L1_AmmoSpeed <= 1)
  {
    L1_Data->L1_AmmoSpeed = 14.2f;
  }

  //二分法取边界
  //当角度大于-15度时，定义域为-30 到 90度
  //当角度小于-15度时，定义域为-90 到 30度

  if (L1_Data->L1_Angel >= PIp12_negative)
  {
  } //这里啥也不用干，前面theta系列已经做好了赋值了
  else
  {
    theta[0] = PIp2_negative;
    theta[1] = PIp6;
  } //-15度时，定义域为-30 到 30度

  do
  {
    arm_mean_f32(theta, 2, &theta_mid); //确定新的运算核心

    Sin_theta = arm_sin_f32(theta_mid);
    Cos_theta = arm_cos_f32(theta_mid);
    arm_mult_f32(&L1_Data->L1_AmmoSpeed, &Cos_theta, &Vx, 1); //Vx	V0Costheat
    arm_mult_f32(&L1_Data->L1_AmmoSpeed, &Sin_theta, &Vy, 1); //Vy	V0Sintheta
    exp1 = Dx / Vx;                                           //exp1
    arm_mult_f32(&AmmoMGpK, &exp1, &exp2, 1);                 //exp2
    arm_mult_f32(&exp1, &Vy, &exp3, 1);                       //exp3
    arm_mult_f32(&AmmoKpM, &exp1, &exp4, 1);                  //exp4

    arm_sub_f32(&Ammo1One, &exp4, &exp5, 1);    //exp5
    exp6 = logf(exp5);                          //exp6
    arm_mult_f32(&AmmoM2GpK2, &exp6, &exp7, 1); //exp7
    arm_add_f32(&exp2, &exp3, &exp8, 1);        //exp8
    arm_add_f32(&exp7, &exp8, &C_Dy, 1);        //C_Dy
    L1_Iteration->TotalCalcu_Numbers++;
    arm_sub_f32(&C_Dy, &Dy, &result, 1);
    arm_abs_f32(&result, &abs_result, 1);
    if (abs_result < precision)
    {
      if (theta_mid > PITCH_MAX || theta_mid < PITCH_MIN)
      {
        //L1_Data->Caluculat_angel = Info_imu.eular[0];
      }
      else
      {
        //角度值 云台抬高后角度  被注了就是弧度制
        arm_mult_f32(&theta_mid, &OneRad, &L1_Data->Caluculat_angel, 1); //角度用°表示
        arm_mult_f32(&AmmoMpK_negative, &exp6, &L1_Data->TOA, 1);        //弹道飞行时间计算
        L1_Iteration->Pfps++;
      }
      L1_Iteration->TotalCalcu_last_numbers = L1_Iteration->TotalCalcu_Numbers;
      L1_Iteration->TotalCalcu_Numbers = 0;
      return;
    }
    else if (result > 0)
    {
      theta[1] = theta_mid;
    }
    else
    {
      theta[0] = theta_mid;
    }

    L1_Iteration->TotalCalcu_Numbers++;
  } while (L1_Iteration->TotalCalcu_Numbers < 15);

  L1_Iteration->TotalCalcu_last_numbers = L1_Iteration->TotalCalcu_Numbers;
  L1_Iteration->TotalCalcu_Numbers = 0;
  //L1_Data->Caluculat_angel = 2333;
}

