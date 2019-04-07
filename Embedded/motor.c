#include "motor.h"
#include "stm32f10x.h"
#include "encoder.h"

int pwm_limit = 100;
static unsigned int period = 500;

void motor_initialize()
{	
		// Initialize Timer
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
		uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = PrescalerValue;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = period;
    timerInitStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM3, &timerInitStructure);
	
		// Initialize pins
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIOA->BSRR = GPIO_Pin_4;
	
		// Initialize PWM
		TIM_OCInitTypeDef outputChannelInit = {0,};
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = 125;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
 
    TIM_OC1Init(TIM3, &outputChannelInit);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
		
		
    TIM_Cmd(TIM3, ENABLE);
}

static void set_direction(int direction) {
		if (direction == 1) {
				GPIOA->BSRR = GPIO_Pin_4;
				GPIOA->BRR = GPIO_Pin_5;
		} else if (direction == -1) {
				GPIOA->BRR = GPIO_Pin_4;
				GPIOA->BSRR = GPIO_Pin_5;			
		}
}

void motor_set_pwm(int pwm_signal) {
	  int direction = 1;
		if (pwm_signal < 0) {
				pwm_signal = -1 * pwm_signal;
				direction = -1;
		}
		
		if (pwm_signal > pwm_limit) {
			  pwm_signal = pwm_limit;
		}
		
		double dPWM = (double)pwm_signal * period/100;
		
		set_direction(direction);
		
		TIM_OCInitTypeDef outputChannelInit = {0,};
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = dPWM;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC1Init(TIM3, &outputChannelInit);		
}