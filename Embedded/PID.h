#ifndef PID_H
#define PID_H

void PID_initialize();
void PID_process();
int PID_set_position(int position);
int PID_stop();
int PID_start();
int PID_set_Kp(double Kp);
int PID_set_Ki(double Ki);
int PID_set_Kd(double Kd);
int PID_get_parameters();

#endif