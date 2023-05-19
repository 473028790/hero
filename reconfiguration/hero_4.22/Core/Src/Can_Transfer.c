/*
 * @Description:CAN transmission
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-25 10:41:31
 */
#include "Can_Transfer.h"
#include "main.h"
#include <string.h>

float CAN1_ID1FF[4];
static float CAN2_ID200[4];
static float CAN2_ID1FF[4];
static RC_Ctl_t RC_CtrlData;
/**
 * @brief: Can Task
 * @param {void*} argument
 * @return {*}
 */
void can_Task(void const * argument)
{
  /* USER CODE BEGIN can_Task */
	uint32_t PreviousWaketime = osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
    f_can_all_tx_offline();	
    //f_can1_tx_0x1ff(CAN1_ID1FF);//YAW,Dial
    //f_can2_tx_0x200(CAN2_ID200);//FRI
    f_can2_tx_0x1ff(CAN2_ID1FF);//PITCH
	osDelayUntil(&PreviousWaketime,2);
  }
  /* USER CODE END can_Task */
}

/**
 * @brief: CAN1 sending
 * @return void
 */
void f_can1_tx_0x1ff(float*data)
{	  
    CAN_TxHeaderTypeDef tx;

    tx.StdId = 0x1FF;
    tx.RTR = CAN_RTR_DATA;
    tx.IDE = CAN_ID_STD;
    tx.DLC = 8;
    uint8_t Tx_Data_v[8];
    Tx_Data_v[0] = (uint8_t)((int16_t)data[0] >> 8);
    Tx_Data_v[1] = (uint8_t)(int16_t)data[0];
    Tx_Data_v[2] = (uint8_t)((int16_t)data[1] >> 8);
    Tx_Data_v[3] = (uint8_t)(int16_t)data[1];
    Tx_Data_v[4] = (uint8_t)((int16_t)data[2] >> 8);
    Tx_Data_v[5] = (uint8_t)(int16_t)data[2];
    Tx_Data_v[6] = (uint8_t)((int16_t)data[3] >> 8);
    Tx_Data_v[7] = (uint8_t)(int16_t)data[3];
    HAL_CAN_AddTxMessage(&hcan1,&tx,Tx_Data_v,(uint32_t*)CAN_TX_MAILBOX0);
}
/**
 * @brief: CAN2 sending
 * @return void
 */
void f_can2_tx_0x200(float*data)
{
    CAN_TxHeaderTypeDef CAN2_ID200_t;

    CAN2_ID200_t.StdId = 0x200;
    CAN2_ID200_t.RTR = CAN_RTR_DATA;
    CAN2_ID200_t.IDE = CAN_ID_STD;
    CAN2_ID200_t.DLC = 0x08;
    uint8_t Tx_Data_v[8];
    Tx_Data_v[0] = (uint8_t)((int16_t)data[0] >> 8);
    Tx_Data_v[1] = (uint8_t)(int16_t)data[0];
    Tx_Data_v[2] = (uint8_t)((int16_t)data[1] >> 8);
    Tx_Data_v[3] = (uint8_t)(int16_t)data[1];
    Tx_Data_v[4] = (uint8_t)((int16_t)data[2] >> 8);
    Tx_Data_v[5] = (uint8_t)(int16_t)data[2];
    Tx_Data_v[6] = (uint8_t)((int16_t)data[3] >> 8);
    Tx_Data_v[7] = (uint8_t)(int16_t)data[3];
    HAL_CAN_AddTxMessage(&hcan2,&CAN2_ID200_t,Tx_Data_v,(uint32_t*)CAN_TX_MAILBOX0);
}
/**
 * @brief: CAN2 sending
 * @return void
 */
void f_can2_tx_0x1ff(float*data)
{
    CAN_TxHeaderTypeDef CAN2_ID1FF_t;

    CAN2_ID1FF_t.StdId = 0x1FF;
    CAN2_ID1FF_t.RTR = CAN_RTR_DATA;
    CAN2_ID1FF_t.IDE = CAN_ID_STD;
    CAN2_ID1FF_t.DLC = 0x08;
    uint8_t Tx_Data_v[8];
    Tx_Data_v[0] = (uint8_t)((int16_t)data[0] >> 8);
    Tx_Data_v[1] = (uint8_t)(int16_t)data[0];
    Tx_Data_v[2] = (uint8_t)((int16_t)data[1] >> 8);
    Tx_Data_v[3] = (uint8_t)(int16_t)data[1];
    Tx_Data_v[4] = (uint8_t)((int16_t)data[2] >> 8);
    Tx_Data_v[5] = (uint8_t)(int16_t)data[2];
    Tx_Data_v[6] = (uint8_t)((int16_t)data[3] >> 8);
    Tx_Data_v[7] = (uint8_t)(int16_t)data[3];
    HAL_CAN_AddTxMessage(&hcan2,&CAN2_ID1FF_t,Tx_Data_v,(uint32_t*)CAN_TX_MAILBOX0);  
}
/**
 * @brief: Offline mode
 * @return void
 */
void f_can_all_tx_offline(void)
{
    f_get_RC(&RC_CtrlData);
	if(RC_CtrlData.rc.s1==OFFLINE_MODE)
	{
        memset(&CAN1_ID1FF, 0, sizeof(CAN1_ID1FF));
        memset(&CAN2_ID200, 0, sizeof(CAN2_ID200));
        memset(&CAN2_ID1FF, 0, sizeof(CAN2_ID1FF));
        f_Gimbal_set_offline();
    }
}
/**
 * @brief: Assign values to data sent by CAN
 * @param {uint8_t} Canx
 * @param {uint16_t} StdId
 * @param {uint8_t} Number
 * @param {float} Value
 * @return {void}
 */
void f_can_assignment(uint8_t Canx,uint16_t StdId,uint8_t Number,float Value)
{
    if(Canx==1)
    {
        if(StdId==0x1FF)
        {
            CAN1_ID1FF[Number]=Value;
        }
    }
    else if(Canx==2)
    {
        if(StdId==0x200)
        {
            CAN2_ID200[Number]=Value;
        }
        else if(StdId==0x1FF)
        {
            CAN2_ID1FF[Number]=Value;
        }
    }
}