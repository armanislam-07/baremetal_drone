#ifndef AUTOLEVEL_H
#define AUTOLEVEL_H
#include <stdint.h>
#include "../drivers/mpu6050.h"
typedef enum { FLIGHT_DISARMED, FLIGHT_RUNNING, FLIGHT_LANDING, FLIGHT_FAULT } flight_state_t;
void flight_init(void);
int flight_calibrate(const mpu6050_sample_t *sample);
int flight_arm(uint32_t now_ms);
void flight_step(const mpu6050_sample_t *sample, uint32_t now_ms);
void flight_fault(void);
flight_state_t flight_state(void);
#endif
