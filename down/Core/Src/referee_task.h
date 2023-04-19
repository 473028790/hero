#ifndef __REFEREE_TASK_H_
#define __REFEREE_TASK_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>
#include  "judge_protocol.h"
/* Exported macro ------------------------------------------------------------*/
#define JUDGE_FRAME_HEADER 0xA5           //֡ͷ
#define Power_priority 1
#define Health_priority 0

#define outburst_priority 1
#define speed_priority 0
/* Exported types ------------------------------------------------------------*/

typedef struct
{
	uint8_t char_send_times :4;
}UI_char_t;

/* 
	ѧ�������˼�ͨ�� cmd_id 0x0301������ data_ID:0x0200~0x02FF
	�������� �����˼�ͨ�ţ�0x0301��
	����Ƶ�ʣ����������кϼƴ������� 5000 Byte�� �����з���Ƶ�ʷֱ𲻳���30Hz��
 * +------+------+-------------+------------------------------------+
 * | byte | size |    breif    |            note                    |
 * |offset|      |             |                                    |
 * +------+------+-------------+------------------------------------+
 * |  0   |  2   | 	 data_ID   | 0x0200~0x02FF,��������Щ ID ��ѡȡ |
 * |      |      |             | ����ID�����ɲ������Զ���           |
 * +------|------|-------------|------------------------------------|
 * |  2   |  2   | 	sender_ID  | ��ҪУ�鷢���ߵ� ID ��ȷ��					|
 * +------|------|-------------|------------------------------------|
 * |  4   |  2   | receiver_ID | ��ҪУ������ߵ� ID ��ȷ��					|
 * |      |      |             | ���粻�ܷ��͵��жԻ����˵�ID				| 
 * +------|------|-------------|------------------------------------|
 * |  6   |  n   | 		Data     | n ��ҪС�� 113 										|
 * +------+------+-------------+------------------------------------+
*/

/******************************�ͻ��˽�������**************************************/
enum
{
	//0x200-0x02ff 	�����Զ������� ��ʽ  INTERACT_ID_XXXX
	INTERACT_ID_delete_graphic 			= 0x0100,	/*�ͻ���ɾ��ͼ��*/
	INTERACT_ID_draw_one_graphic 		= 0x0101,	/*�ͻ��˻���һ��ͼ��*/
	INTERACT_ID_draw_two_graphic 		= 0x0102,	/*�ͻ��˻���2��ͼ��*/
	INTERACT_ID_draw_five_graphic 	= 0x0103,	/*�ͻ��˻���5��ͼ��*/
	INTERACT_ID_draw_seven_graphic 	= 0x0104,	/*�ͻ��˻���7��ͼ��*/
	INTERACT_ID_draw_char_graphic 	= 0x0110,	/*�ͻ��˻����ַ�ͼ��*/
	INTERACT_ID_bigbome_num					= 0x02ff
};
//��λ���ֽڣ�
enum
{
	LEN_INTERACT_delete_graphic     = 8,  //ɾ��ͼ�� 2(��������ID)+2(������ID)+2��������ID��+2���������ݣ�  
	LEN_INTERACT_draw_one_graphic   = 21, // ����2+2+2+15
	LEN_INTERACT_draw_two_graphic   = 36, //6+15*2
	LEN_INTERACT_draw_five_graphic  = 81, //6+15*5
	LEN_INTERACT_draw_seven_graphic = 111,//6+15*7
	LEN_INTERACT_draw_char_graphic  = 51, //6+15+30���ַ������ݣ�
};
//****************************��ͼ�����ݶ�����****************************/

typedef __packed struct//ͼ��
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; //ֱ��  ����  ��Բ  ��Բ  Բ��  ����  ����  �ַ�
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  //��    ��    ��    ��    �Ƕ�  ��С  ��С  ��С
	uint32_t end_angle:9;    //��    ��    ��    ��          λ��  ��    ����
	uint32_t width:10;       
	uint32_t start_x:11;     //���  ���  Բ��  Բ��  Բ��  ���  ���  ���
	uint32_t start_y:11;     //
	uint32_t radius:10;      //��    ��    �뾶  ��    ��    ��    ��    ��
	uint32_t end_x:11;       //�յ�  �Զ�  ��    ����  ����  ��    ��    ��
	uint32_t end_y:11;       //                              ��    ��    ��
} graphic_data_struct_t;

typedef __packed struct//Char��
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3;
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  
	uint32_t end_angle:9;    
	uint32_t width:10;       
	uint32_t start_x:11;     
	uint32_t start_y:11;     
} Char_data_struct_t;
typedef __packed struct//Char��
{
	graphic_data_struct_t grapic_data_struct;
	uint8_t data[30];
} ext_client_string_t;

//bit 0-2
typedef enum
{
	NONE   = 0,/*�ղ���*/
	ADD    = 1,/*����ͼ��*/
	MODIFY = 2,/*�޸�ͼ��*/
	DELETE = 3,/*ɾ��ͼ��*/
}Graphic_Operate;

/*ͼ�����*/
//bit3-5
typedef enum
{
	LINE      = 0,//ֱ��
	RECTANGLE = 1,//����
	CIRCLE    = 2,//��Բ
	OVAL      = 3,//��Բ
	ARC       = 4,//Բ��
	FLOAT     = 5,//������
	INT       = 6,//������
	CHAR      = 7 //�ַ�
}Graphic_Type;

/*ͼ������*/
//bit 6-9ͼ���� ���Ϊ9����С0
//bit 10-13��ɫ
typedef enum
{
	RED_BLUE  = 0,//������ɫ	
	YELLOW    = 1,
	GREEN     = 2,
	ORANGE    = 3,
	FUCHSIA   = 4,	/*�Ϻ�ɫ*/
	PINK      = 5,
	CYAN_BLUE = 6,	/*��ɫ*/
	BLACK     = 7,
	WHITE     = 8
}Graphic_Color;

/*ͼ����ɫ����*/
//bit 14-31 �Ƕ� [0,360]
/**********************************�ͻ��˻�ͼ************************************************/

//�����ַ���
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			//֡ͷ
	uint16_t  CmdID;										//������
	ext_aerial_data_t   dataFrameHeader;//���ݶ�ͷ�ṹ
	ext_client_string_t clientData;//���ݶ�
	uint16_t	FrameTail;								//֡β
}ext_charstring_data_t;

//��������
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_aerial_data_t   dataFrameHeader;
	graphic_data_struct_t Line_data_struct;	
	uint16_t	FrameTail;								
}ext_line_one_data_t;

/* Exported functions --------------------------------------------------------*/
void Client_Line_update(void);
void Client_Char_update(void);
void Client_int_update(void);
void acknowledge_client_id(void);
void  UI_Ctrl(void);
void chassis_mode_judge(void);
void super_cap_buffer(void);
int hero_refree_shoot_number(void);
#endif
