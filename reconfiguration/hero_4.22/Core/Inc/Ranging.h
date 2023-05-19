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
#endif


