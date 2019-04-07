#define RTE_DEVICE_STDPERIPH_EXTI
#define RTE_DEVICE_STDPERIPH_FRAMEWORK
#include "stm32f10x.h"
#include "stm32f10x_it.h"

static int ctr = 0;

void EXTI0_IRQHandler(void)
{
		//Check if EXTI_Line0 is asserted
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
    }
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        //Handle the interrupt
				if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
						++ctr;
				} else {
					  --ctr;
				}
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void encoder_initialize() {
		EXTI_InitTypeDef   EXTI_InitStructure;
		GPIO_InitTypeDef   GPIO_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
	
		/* Enable GPIOA clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		/* Configure PA.00 pin as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Enable AFIO clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

		/* Connect EXTI0 Line to PA.00 pin */
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

		/* Configure EXTI0 line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set EXTI0 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* Enable GPIOB clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		
		/* Configure PB.09 pin as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		/* Enable AFIO clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		/* Connect EXTI2 Line to PB.02 pin */
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
		
		/* Configure EXTI9 line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		/* Enable and set EXTI9_5 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		
		NVIC_Init(&NVIC_InitStructure);
}

int encoder_get_position() {
		return ctr;
}