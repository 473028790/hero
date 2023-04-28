#include "Can_Transfer.h"

float CAN1_0x1FF[4];
float CAN2_ID200[4];
float CAN2_ID1FF[4];

/* USER CODE BEGIN CAN1-Transfer */
void CAN1_0x1FF_TX(float*data)//CAN1-ID0x1FF-Transfer
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
/* USER CODE END CAN1-Transfer */

/* USER CODE BEGIN CAN2-Transfer */
void CAN2_0x200_TX(float*data)//CAN2-ID0x200-Transfer
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
void CAN2_0x1FF_TX(float*data)//CAN2-ID0x1FF-Transfer
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
/* USER CODE END CAN2-Transfer */

