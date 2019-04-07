/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "comms.h"
#include "encoder.h"
#include "motor.h"
#include "PID.h"

static void commsTask(void *pvParameters);
static void commsTask2(void *pvParameters);
static void PIDTask(void *pvParameters);

int main(void)
{
		comms_initialize();
		encoder_initialize();
		motor_initialize();
		PID_initialize();

		xTaskCreate( commsTask, "UART", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
		//xTaskCreate( commsTask2, "UART2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
		xTaskCreate( PIDTask, "PID", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

		vTaskStartScheduler();	  
}

static void commsTask(void *pvParameters) {
		const TickType_t xDelay = 5 / portTICK_PERIOD_MS;
		while (1) {
				comms_process();
			  vTaskDelay( xDelay );
    }
}

static void commsTask2(void *pvParameters) {
		while (1) {
				const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
				printf("ctr: %d\r\n", encoder_get_position());
				vTaskDelay( xDelay );
    }
}

static void PIDTask(void *pvParameters) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		const TickType_t xFrequency = 1 / portTICK_PERIOD_MS;
		while (1) {
				PID_process();
			  vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}