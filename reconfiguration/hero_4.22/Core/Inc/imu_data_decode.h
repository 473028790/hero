#ifndef __IMU_DATA_DECODE_H__
#define __IMU_DATA_DECODE_H__

#include <stdint.h>
#include <stdbool.h>


int get_raw_acc(float* a);
int get_raw_gyr(float* g);
int get_raw_mag(float* m);
int get_eular(float* e);

void IMU_Pack_Recive(uint8_t *p);
#endif


