#ifndef _CHASSIS_MODE_H_
#define _CHASSIS_MODE_H_
#include "stdint.h"


#define cheassis_follow 0
#define Small_gyroscope 1


void Function_Choose(void);
void CtoG(int16_t right_X,int16_t right_Y);
void Circle(int16_t right_Y,int16_t right_X);
void separate(int16_t right_X,int16_t right_Y);

#endif
