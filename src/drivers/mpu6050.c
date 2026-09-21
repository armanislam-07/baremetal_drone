#include "mpu6050.h"
#include "i2c.h"
#define MPU_ADDR 0x68u /* AD0 must be tied low. */
static int read_word(uint8_t reg, int16_t *value)
{
    uint8_t hi, lo;
    if (!i2c_read_reg(MPU_ADDR, reg, &hi) || !i2c_read_reg(MPU_ADDR, reg + 1u, &lo)) return 0;
    *value = (int16_t)((uint16_t)((uint16_t)hi << 8) | lo);
    return 1;
}
int mpu6050_init(void)
{
    uint8_t id;
    if (!i2c_read_reg(MPU_ADDR, 0x75u, &id) || id != MPU_ADDR) return 0;
    return i2c_write_reg(MPU_ADDR, 0x6bu, 0x01u) /* PLL with X gyro */
        && i2c_write_reg(MPU_ADDR, 0x19u, 0x04u) /* 200 Hz */
        && i2c_write_reg(MPU_ADDR, 0x1au, 0x03u) /* 44 Hz DLPF */
        && i2c_write_reg(MPU_ADDR, 0x1bu, 0x00u) /* +/-250 dps */
        && i2c_write_reg(MPU_ADDR, 0x1cu, 0x00u); /* +/-2 g */
}
int mpu6050_read(mpu6050_sample_t *sample)
{
    if (!sample) return 0;
    return read_word(0x3bu, &sample->ax) && read_word(0x3du, &sample->ay)
        && read_word(0x3fu, &sample->az) && read_word(0x43u, &sample->gx)
        && read_word(0x45u, &sample->gy) && read_word(0x47u, &sample->gz);
}
