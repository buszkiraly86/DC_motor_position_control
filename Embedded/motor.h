#ifndef MOTOR_H
#define MOTOR_H

extern int pwm_limit;

void motor_initialize();
void motor_set_pwm(int pwm_signal);

#endif