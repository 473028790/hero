/* Includes ------------------------------------------------------------------*/
#include  "judge_protocol.h"
/* Private typedef -----------------------------------------------------------*/
/*全局变量*/
judge_info_t judge_infop;
/* Private functions ---------------------------------------------------------*/

/**
 *	@brief	裁判系统数据更新
 */
void judge_update(uint8_t *rxBuf)
{
    uint16_t frame_length;
    uint16_t cmd_id;
    judge_info_t *judge_info=&judge_infop;
    memcpy(&judge_info->FrameHeader, rxBuf, LEN_FRAME_HEAD);

    /* 帧首字节是否为0xA5 */
    if(rxBuf[J_SOF] == 0xA5)
    {
        /* 帧头CRC8校验 */
        if(Verify_CRC8_Check_Sum(rxBuf, LEN_FRAME_HEAD) == true)
        {
            /* 统计一帧的总数据长度，用于CRC16校验 */
            frame_length = LEN_FRAME_HEAD + LEN_CMD_ID + judge_info->FrameHeader.data_length + LEN_FRAME_TAIL;
            judge_info->frame_length = frame_length;

            if(Verify_CRC16_Check_Sum(rxBuf, frame_length) == true)
            {
                judge_info->offline_cnt = 0;
                cmd_id = (rxBuf[CMD_ID+1] << 8 | rxBuf[CMD_ID]);
                judge_info->cmd_id = cmd_id;

                switch(cmd_id)
                {
							 case ID_GAME_STATUS: /*0x0001*/
                    memcpy(&judge_info->GameStatus, (rxBuf+DATA_SEG), LEN_GAME_STATUS);            
                break;
							 
							 case ID_GAME_RESULT: /*0x0002*/
                    memcpy(&judge_info->GameResult,  (rxBuf+DATA_SEG), LEN_GAME_RESULT);                
                break;
							 
                case ID_GAME_ROBOT_HP: /*0x0003*/
										memcpy(&judge_info->GameStatus, (rxBuf+DATA_SEG), LEN_GAME_STATUS);                
                break;       
								
                case ID_EVENT_DATA: /*0x0101*/
									  memcpy(&judge_info->EventData, (rxBuf+DATA_SEG), LEN_EVENT_DATA);                
                break;
								
							 case ID_SUPPLY_PROJECTILE_ACTION: /*0x0102*/
                    memcpy(&judge_info->SupplyProjectileAction, (rxBuf+DATA_SEG), LEN_SUPPLY_PROJECTILE_ACTION);
                    judge_info->supply_data_update = true;	// 补给站数据更新                
							 break;
								 
                case ID_GAME_ROBOT_STATUS: /*0x0201*/
									  memcpy(&judge_info->GameRobotStatus, (rxBuf+DATA_SEG), LEN_GAME_ROBOT_STATUS);                
                break;
								
                case ID_POWER_HEAT_DATA: /*0x0202*/
									  memcpy(&judge_info->PowerHeatData, (rxBuf+DATA_SEG), LEN_POWER_HEAT_DATA); 							                 
                break;
								
							 case ID_GAME_ROBOT_POS: /*0x0203*/
                    memcpy(&judge_info->GameRobotPos, (rxBuf+DATA_SEG), LEN_GAME_ROBOT_POS);               
                break;

                case ID_BUFF: /*0x0204*/
                    memcpy(&judge_info->Buff, (rxBuf+DATA_SEG), LEN_BUFF);                
                break;
								
                case ID_ROBOT_HURT: /*0x0206*/
									  memcpy(&judge_info->RobotHurt, (rxBuf+DATA_SEG), LEN_ROBOT_HURT);               
                break;
								
                case ID_SHOOT_DATA: /*0x0207*/
									  memcpy(&judge_info->ShootData, (rxBuf+DATA_SEG), LEN_SHOOT_DATA);                
                break;
								
                case ID_BULLET_REMAINING: /*0x0208*/
										memcpy(&judge_info->BulletRemaining, (rxBuf+DATA_SEG), LEN_BULLET_REMAINING);		              
                break;			
								
							 case ID_RFID_STATUS: /*0x0209*/
                    memcpy(&judge_info->RfidStatus, (rxBuf+DATA_SEG), LEN_RFID_STATUS);                
                break;

                case ID_COMMUNICATION: /*0x0301*/
                    memcpy(&judge_info->AerialData, (rxBuf+DATA_SEG), LEN_AERAL_DATA);  //云台手数据
					         judge_info->communication_data_update = true;					                
                break;
				
							 case ID_COMMAND:/*0x0303*/
									memcpy(&judge_info->command, (rxBuf+DATA_SEG), 15);  
									judge_info->command_data_update = true;
                break;							 
                }
            }
        }
        /* 帧尾CRC16下一字节是否为0xA5 */
        if(rxBuf[ frame_length ] == 0xA5)
        {
            /* 如果一个数据包出现了多帧数据就再次读取 */
            judge_update( &rxBuf[frame_length]);
        }
    }
}
