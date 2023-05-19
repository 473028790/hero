/*
 * @Description: Imu data processing
 * @Version: V1.0
 * @Author: QIAN
 * @IDE: Visual Studio Code
 * @Date: 2023-04-24 20:17:30
 */
#include "imu_data_decode.h"
#include "variable_config.h"
#include "string.h"

static id0x92_t IMU_data= {0};
/**
 * @brief: get imu acc
 * @param {float*} a
 * @return {*}
 */
int get_raw_acc(float* a)
{
    memcpy(a, IMU_data.acc, sizeof(IMU_data.acc));
    return 0;
}
/**
 * @brief: get imu gyr
 * @param {float*} g
 * @return {*}
 */
int get_raw_gyr(float* g)
{
    memcpy(g, IMU_data.gyr, sizeof(IMU_data.gyr));
    return 0;
}
/**
 * @brief: get imu mag
 * @param {float*} m
 * @return {*}
 */
int get_raw_mag(float* m)
{
    memcpy(m, IMU_data.mag, sizeof(IMU_data.mag));
    return 0;
}
/**
 * @brief: get imu eular
 * @param {float*} e
 * @return {*}
 */
int get_eular(float* e)
{
    memcpy(e, IMU_data.eul, sizeof(IMU_data.eul));
    return 0;
}

static void crc16_update(uint16_t *currect_crc, const uint8_t *src, uint32_t len)
{
    uint32_t crc = *currect_crc;
    uint32_t j;
    for (j=0; j < len; ++j)
    {
        uint32_t i;
        uint32_t byte = src[j];
        crc ^= byte << 8;
        for (i = 0; i < 8; ++i)
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    } 
    *currect_crc = crc;
};
/**
 * @brief: imu data decode
 * @param {uint8_t} *p
 * @return {*}
 */
static void IMU_Pack_Decode(uint8_t *p)
{
	uint16_t CRCRecive=0,CRCCalculate=0;
	uint16_t DataLen =0 ;
	uint16_t Datacounter=0;
	if((p[0] == 0x5A )&& (p[1] == 0xA5))
	{
		DataLen   = p[2] | p[3]<<8;
		CRCRecive = p[4] | p[5]<< 8 ;
		crc16_update(&CRCCalculate,p,4);
		crc16_update(&CRCCalculate,p+6,DataLen);
		if(CRCCalculate == CRCRecive)
		{
			if(p[6+Datacounter] == 0x90 ) 
			{
				IMU_data.id = p[6+Datacounter+1];
				Datacounter += 2;
			};
			if(p[6+Datacounter] == 0xA0)
			{
				IMU_data.acc[0] = (int16_t)(p[6+Datacounter+1] | p[6+Datacounter+2]<<8 )/1000.0f;
				IMU_data.acc[1] = (int16_t)(p[6+Datacounter+3] | p[6+Datacounter+4]<<8 )/1000.0f;
				IMU_data.acc[2] = (int16_t)(p[6+Datacounter+5] | p[6+Datacounter+6]<<8 )/1000.0f;
				Datacounter += 7;
			};
			if(p[6+Datacounter] == 0xB0)
			{
				IMU_data.gyr[0] = (int16_t)(p[6+Datacounter+1] | p[6+Datacounter+2]<<8 )/10.0f;
				IMU_data.gyr[1] = (int16_t)(p[6+Datacounter+3] | p[6+Datacounter+4]<<8 )/10.0f;
				IMU_data.gyr[2] = (int16_t)(p[6+Datacounter+5] | p[6+Datacounter+6]<<8 )/10.0f;
				Datacounter += 7;
			};
			if(p[6+Datacounter] == 0xC0)
			{
				IMU_data.mag[0] = (int16_t)(p[6+Datacounter+1] | p[6+Datacounter+2]<<8 )/10.0f;
				IMU_data.mag[1] = (int16_t)(p[6+Datacounter+3] | p[6+Datacounter+4]<<8 )/10.0f;
				IMU_data.mag[2] = (int16_t)(p[6+Datacounter+5] | p[6+Datacounter+6]<<8 )/10.0f;
				Datacounter += 7;
			};
			if(p[6+Datacounter] == 0xD0)
			{
				IMU_data.eul[1] = (int16_t)(p[6+Datacounter+1] | p[6+Datacounter+2]<<8 )/100.0f;
				IMU_data.eul[0] = (int16_t)(p[6+Datacounter+3] | p[6+Datacounter+4]<<8 )/100.0f;
				IMU_data.eul[2] = (int16_t)(p[6+Datacounter+5] | p[6+Datacounter+6]<<8 )/10.0f;
				Datacounter += 7;
			};
		};
	};
};
/**
 * @brief: imu recive
 * @param {uint8_t} *p
 * @return {*}
 */
void IMU_Pack_Recive(uint8_t *p)
{
	IMU_Pack_Decode(p);
};