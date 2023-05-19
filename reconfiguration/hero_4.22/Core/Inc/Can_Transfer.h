/*
 * @Description: 
 * @Version: 
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-25 10:42:33
 */
#ifndef __CAN_TRAMSFER_H
#define __CAN_TRAMSFER_H
#include "main.h"
#include "can.h"
#include "cmsis_os.h"
#include "Gimbal.h"


void f_can1_tx_0x1ff(float*data);
void f_can2_tx_0x200(float*data);
void f_can2_tx_0x1ff(float*data);
void f_can_all_tx_offline(void);
void f_can_assignment(uint8_t Canx,uint16_t StdId,uint8_t Number,float Value);
#endif


