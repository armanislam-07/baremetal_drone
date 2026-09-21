#include <stdint.h>
#include "drivers/io.h"
#include "drivers/pwm.h"
#include "drivers/i2c.h"
#include "drivers/mpu6050.h"
#include "drivers/registers.h"
#include "app/autolevel.h"
/* Set to 1 only after wiring, motor mapping, PID gains and throttle are verified. */
#ifndef DRONE_AUTOSTART
#define DRONE_AUTOSTART 0
#endif
static volatile uint32_t milliseconds;
void SysTick_Handler(void) { ++milliseconds; }
int main(void)
{
    io_init();
    motor_init();
    flight_init();
    SYST_RVR = 72000u - 1u;
    SYST_CVR = 0;
    SYST_CSR = 7u;
    i2c_init();
    if (!mpu6050_init()) { flight_fault(); for (;;) { } }
    uint32_t next = milliseconds;
#if DRONE_AUTOSTART
    int mission_started = 0;
#endif
    for (;;) {
        uint32_t now = milliseconds;
        if ((int32_t)(now - next) < 0) continue;
        next = now + 5u;
        mpu6050_sample_t sample;
        if (!mpu6050_read(&sample)) { flight_fault(); for (;;) { } }
        if (flight_state() == FLIGHT_DISARMED) {
            if (flight_calibrate(&sample)) {
#if DRONE_AUTOSTART
                if (!mission_started) mission_started = flight_arm(now);
#endif
            }
        } else {
            flight_step(&sample, now);
        }
        /* A late loop invalidates the fixed 5 ms controller timestep. */
        if ((int32_t)(milliseconds - next) > 0) { flight_fault(); for (;;) { } }
    }
}
