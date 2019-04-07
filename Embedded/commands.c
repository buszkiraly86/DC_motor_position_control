#include "comms.h"
#include "encoder.h"
#include "motor.h"
#include "PID.h"

static void get_position(const unsigned char argc, const char** argv) {
		printf("pos: %d\r\n", encoder_get_position());
}

static void set_position(const unsigned char argc, const char** argv) {
		int target_position;
		sscanf(argv[1], "%d", &target_position);
	
		printf("setting target position to %d...\r\n", target_position);
		if (!PID_set_position(target_position)) {
				printf("done\r\n");
		} else {
				printf("failed!\r\n");
		}
}

static void set_pwm(const unsigned char argc, const char** argv) {
		int target_pwm;
		sscanf(argv[1], "%d", &target_pwm);
	
		motor_set_pwm(target_pwm);
	
		printf("pwm set\r\n");
}

static void stop_PID(const unsigned char argc, const char** argv) {
		if (!PID_stop())
				printf("PID stopped\r\n");
		else
				printf("PID could not be stopped\r\n");
}

static void start_PID(const unsigned char argc, const char** argv) {
		if (!PID_start())
				printf("PID started\r\n");
		else
				printf("PID could not be started\r\n");
}

static void set_Kp(const unsigned char argc, const char** argv) {
		double Kp;
		sscanf(argv[1], "%lf", &Kp);
		if (!PID_set_Kp(Kp))
				printf("PID Kp set to %lf\r\n", Kp);
		else
				printf("Failed to set Kp\r\n");
}

static void set_Ki(const unsigned char argc, const char** argv) {
		double Ki;
		sscanf(argv[1], "%lf", &Ki);
		if (!PID_set_Ki(Ki))
				printf("PID Ki set to %lf\r\n", Ki);
		else
				printf("Failed to set Kp\r\n");
}

static void set_Kd(const unsigned char argc, const char** argv) {
		double Kd;
		sscanf(argv[1], "%lf", &Kd);
		if (!PID_set_Kd(Kd))
				printf("PID Kd set to %lf\r\n", Kd);
		else
				printf("Failed to set Kp\r\n");
}

static void PID_get_parameters_fn(const unsigned char argc, const char** argv) {
		double Kp, Ki, Kd;
		if (!PID_get_parameters(&Kp, &Ki, &Kd))
				printf("PID parameters: Kp: %lf, Ki: %lf, Kd: %lf\r\n", Kp, Ki, Kd);
		else
				printf("PID parameters could not be retrieved\r\n");
}

const command commands[] = {
    {"get_position", get_position},
    {"set_position", set_position},
    {"set_pwm", set_pwm},
    {"stop_PID", stop_PID},
    {"start_PID", start_PID},	
    {"set_Kp", set_Kp},
    {"set_Ki", set_Ki},
    {"set_Kd", set_Kd},
    {"PID_get_parameters", PID_get_parameters_fn},
};

unsigned int nr_of_commands = sizeof(commands)/sizeof(command);