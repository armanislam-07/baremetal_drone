#ifndef MPU6050_H
#define MPU6050_H
#include <stdint.h>
typedef struct { int16_t ax, ay, az, gx, gy, gz; } mpu6050_sample_t;
int mpu6050_init(void);
int mpu6050_read(mpu6050_sample_t *sample);
#endif
