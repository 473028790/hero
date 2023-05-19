#ifndef __DIAL_H
#define __DIAL_H
#include "main.h"
#include "Can_Transfer.h"
#include "pid.h"
#include "variable_config.h"
#include "main.h"

#define ABS(x)	( (x>0) ? (x) : (-x) )

void f_Friction_control(void);
void f_get_total_angle(dial_data_t *p);
void f_get_moto_offset(dial_data_t *ptr);
void f_dial_PID(void);
void f_ReadRc_dial(void);
void f_dial_finish(void);
#endif


