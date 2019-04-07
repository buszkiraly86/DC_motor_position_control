#include "comms.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "FreeRTOS.h"
#include "task.h"

#define RTE_DEVICE_STDPERIPH_USART
#define RTE_DEVICE_STDPERIPH_GPIO
#define RTE_DEVICE_STDPERIPH_RCC

#include "stm32f10x.h"

#define MAX_NR_ARGS 5

extern command commands[];
extern unsigned int nr_of_commands;

static char cmd_ready = 0;
static char buffer[256];
static char buf_pos = 0;

int fputc(int ch, FILE *f)
{
		f = f ; // unused warning prevention

		USART_SendData(USART1, ch);
	
		/* Wait until the byte has been transmitted */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

		return ch;
}

void execute_command(const unsigned char argc, const char** args) {
    const command *p_cmd = 0;
    for (unsigned long i = 0; i < nr_of_commands; ++i) {
        if (!strcmp(args[0], commands[i].command_name)) {
            p_cmd = &commands[i];
            break;
        }
    }

    if (p_cmd) {
        p_cmd->command_fn(argc, args);
    } else {
        printf("unknown command: ");
			  for (unsigned char i = 0; i < argc; ++i) {
						printf("%s ", args[i]);
				}
				printf("\n");
    }
}

int comms_initialize() {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
     
    /* Enable peripheral clocks for USART1 on GPIOA */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_USART1 |
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_AFIO, ENABLE);
						
		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
        
    /* Configure PA9 and PA10 as USART1 TX/RX */
    
    /* PA9 = alternate function push/pull output */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* PA10 = floating input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		  
    /* Configure and initialize usart... */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        
    USART_Init(USART1, &USART_InitStructure);
		
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		
		/* Enable USART1 */
    USART_Cmd(USART1, ENABLE); 
	
		return 0;
}

void USART1_IRQHandler(void)
{
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
				uint16_t ch = USART_ReceiveData(USART1);
				buffer[buf_pos++] = ch;
		}
		
		if (buf_pos && (buffer[buf_pos - 1] == '\r' || buffer[buf_pos - 1] == '\n')) {
				buffer[buf_pos - 1] = 0;
			  cmd_ready = 1;
		}
}


int comms_process() {
		taskENTER_CRITICAL();
		if (cmd_ready) {
				comms_add_command(buffer);
				buf_pos = 0;
			  buffer[buf_pos] = 0;
			  cmd_ready = 0;
		}
		taskEXIT_CRITICAL();

		return 0;
}

int comms_add_command(const char* cmd) {
    const char *args[MAX_NR_ARGS];
    const char* start_position = cmd;
    unsigned char arg_ctr = 0;

    if (strlen(cmd) == 0) return -1;

    char *pch = strtok (cmd, " ");
    while (pch != NULL && arg_ctr < MAX_NR_ARGS) {
        args[arg_ctr] = pch;
        ++arg_ctr;
        pch = strtok (NULL, " ");
    }
    execute_command(arg_ctr, args);
		
		return 0;
}
