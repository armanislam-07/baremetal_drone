#ifndef PWM_H
#define PWM_H
#include <stdint.h>
enum { MOTOR_COUNT = 4 };
void motor_init(void);
void motor_set(unsigned motor, uint16_t permille);
void motor_stop_all(void);
#endif
