#include "PID.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "encoder.h"
#include "motor.h"

static double Kp = 4;
static double Ki = 0.005;
static double Kd = 150;
static int integral_error = 0;
static int prev_error = 0;
static char running = 1;
static SemaphoreHandle_t xSemaphore;
static int target_position = 0;

void PID_initialize()
{
		xSemaphore = xSemaphoreCreateMutex();
}

void PID_process() {
		static int pwm_signal;
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				if (running) {					
						const int position = encoder_get_position();
						const int error = target_position - position;
						const double derror = (double)(error - prev_error) / 1;
					  prev_error = error;
					  const unsigned char saturates = pwm_signal <= -pwm_limit || pwm_signal >= pwm_limit;
					  const unsigned char same_sign = (error < 0 && pwm_signal < 0) || (error > 0 && pwm_signal > 0);
					
						if (saturates && same_sign) {
								// clamping
							  integral_error = integral_error;
						} else {
								integral_error += error * 1;
						}
						
						const int pwm_signal_new = Kp * error + Ki * integral_error + Kd * derror;
										
						if (pwm_signal != pwm_signal_new) {
								pwm_signal = pwm_signal_new;
								motor_set_pwm(pwm_signal);
						}
				}
				xSemaphoreGive( xSemaphore );
		}
}

// Interface functions
int PID_set_position(int position) {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				target_position = position;
			
				xSemaphoreGive( xSemaphore );
				return 0;
		} else {
				return -1;
		}
}

int PID_stop() {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				running = 0;
				motor_set_pwm(0);
			
				// allow some time for the motor to stop
				const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
				vTaskDelay( xDelay );
			
				integral_error = 0;
			  prev_error = 0;
				target_position = encoder_get_position();
			
				xSemaphoreGive( xSemaphore );
				return 0;
		} else {
				return -1;
		}
}

int PID_start() {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				running = 1;
			
				xSemaphoreGive( xSemaphore );
				return 0;
		} else {
				return -1;
		}
}

int PID_set_Kp(double kp) {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				Kp = kp;
			
				xSemaphoreGive( xSemaphore );
				return 0;
		} else {
				return -1;
		}
}

int PID_set_Ki(double ki) {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				Ki = ki;
			
				xSemaphoreGive( xSemaphore );
				return 0;
		} else {
				return -1;
		}
}

int PID_set_Kd(double kd) {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				Kd = kd;
			
				xSemaphoreGive( xSemaphore );
				return 0;
		} else {
				return -1;
		}
}

int PID_get_parameters(double *kp, double *ki, double *kd) {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
				*kp = Kp;
				*ki = Ki;
				*kd = Kd;
			
				xSemaphoreGive( xSemaphore );
				return 0;
		} else {
				return -1;
		}
}