#ifndef __IMU_DATA_DECODE_H__
#define __IMU_DATA_DECODE_H__

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "packet.h"


int imu_data_decode_init(void);
int get_raw_acc(int16_t* a);
int get_raw_gyo(int16_t* g);
int get_raw_mag(int16_t* m);
int get_id(uint8_t *user_id);
int get_eular(float* e);
int get_quat(float* q);

static Packet_t RxPkt; /* used for data receive */
static int16_t acc[3];
static int16_t gyo[3];  //½ǋٶȊstatic int16_t mag[3];
static int16_t mag[3];
static float eular[3];
static float quat[4];
static uint8_t id;
extern int16_t cc;
#endif


