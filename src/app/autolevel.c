#include "autolevel.h"
#include "../drivers/pwm.h"
#include <math.h>
// Motor order: PA0 front-left, PA10 front-right, PA6 rear-right, PA7 rear-left.

#define PI_F 3.14159265f
#define RAD_TO_DEG (180.0f / PI_F)
#define SAMPLE_COUNT 200u
#define BASE_THROTTLE 350.0f // Tune it later on once actual drone is assmebled
#define MISSION_MS 30000u
#define LAND_MS 3000u
static flight_state_t state;
static uint32_t start_ms;
static unsigned calibration_count;
static float gyro_bias_x, gyro_bias_y, gyro_bias_z;
static float roll, pitch;
static int calibrated;
static float clamp(float x, float low, float high) {
  return x < low ? low : (x > high ? high : x);
}
void flight_init(void) {
  state = FLIGHT_DISARMED;
  calibration_count = 0;
  calibrated = 0;
  gyro_bias_x = gyro_bias_y = gyro_bias_z = roll = pitch = 0.0f;
  motor_stop_all();
}
int flight_calibrate(const mpu6050_sample_t *s) {
  if (!s || calibrated || state != FLIGHT_DISARMED)
    return calibrated;
  /* Keep the airframe level and motionless during these 200 samples. */
  if (fabsf((float)s->gx) > 1000.0f || fabsf((float)s->gy) > 1000.0f ||
      fabsf((float)s->gz) > 1000.0f || fabsf((float)s->ax) > 4000.0f ||
      fabsf((float)s->ay) > 4000.0f || s->az < 12000 || s->az > 20000) {
    calibration_count = 0;
    gyro_bias_x = gyro_bias_y = gyro_bias_z = 0.0f;
    return 0;
  }
  gyro_bias_x += (float)s->gx;
  gyro_bias_y += (float)s->gy;
  gyro_bias_z += (float)s->gz;
  if (++calibration_count == SAMPLE_COUNT) {
    gyro_bias_x /= SAMPLE_COUNT;
    gyro_bias_y /= SAMPLE_COUNT;
    gyro_bias_z /= SAMPLE_COUNT;
    calibrated = 1;
  }
  return calibrated;
}
int flight_arm(uint32_t now_ms) {
  if (!calibrated || state != FLIGHT_DISARMED)
    return 0;
  start_ms = now_ms;
  state = FLIGHT_RUNNING;
  return 1;
}
void flight_fault(void) {
  state = FLIGHT_FAULT;
  motor_stop_all();
}
flight_state_t flight_state(void) { return state; }
void flight_step(const mpu6050_sample_t *s, uint32_t now_ms) {
  if (!s || (state != FLIGHT_RUNNING && state != FLIGHT_LANDING))
    return;
  float ax = (float)s->ax, ay = (float)s->ay, az = (float)s->az;
  float norm = sqrtf(ax * ax + ay * ay + az * az);
  if (norm < 8192.0f || norm > 24576.0f) {
    flight_fault();
    return;
  }
  float gx = ((float)s->gx - gyro_bias_x) / 131.0f;
  float gy = ((float)s->gy - gyro_bias_y) / 131.0f;
  float gz = ((float)s->gz - gyro_bias_z) / 131.0f;
  float accel_roll = atan2f(ay, az) * RAD_TO_DEG;
  float accel_pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * RAD_TO_DEG;
  roll = 0.98f * (roll + gx * 0.005f) + 0.02f * accel_roll;
  pitch = 0.98f * (pitch + gy * 0.005f) + 0.02f * accel_pitch;
  if (fabsf(roll) > 35.0f || fabsf(pitch) > 35.0f || fabsf(gx) > 300.0f ||
      fabsf(gy) > 300.0f) {
    flight_fault();
    return;
  }
  uint32_t elapsed = now_ms - start_ms;
  if (elapsed >= MISSION_MS)
    state = FLIGHT_LANDING;
  if (elapsed >= MISSION_MS + LAND_MS) {
    state = FLIGHT_DISARMED;
    motor_stop_all();
    return;
  }
  float throttle = BASE_THROTTLE;
  if (state == FLIGHT_LANDING)
    throttle *= (float)(MISSION_MS + LAND_MS - elapsed) / LAND_MS;
  float r = clamp(-3.0f * roll - 0.9f * gx, -160.0f, 160.0f);
  float p = clamp(-3.0f * pitch - 0.9f * gy, -160.0f, 160.0f);
  float y = clamp(-0.3f * gz, -40.0f, 40.0f);
  /* X quad, opposite propeller pairs spin in opposite directions. */
  const float output[4] = {throttle + r + p + y, throttle - r + p - y,
                           throttle - r - p + y, throttle + r - p - y};
  for (unsigned i = 0; i < MOTOR_COUNT; ++i)
    motor_set(i, (uint16_t)clamp(output[i], 0.0f, 650.0f));
}
