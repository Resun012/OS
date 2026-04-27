/***
 * Demo program to light 4 LEDs as binary random value
 * Uses FreeRTOS Task
 * Jon Durrant
 * 15-Aug-2022
 */


#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

#include "BlinkAgent.h"
#include "CounterAgent.h"

#define TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

// Blue LED (Pico onboard)
#define LED_PAD 25

// 8 LEDs
#define LED1_PAD 2
#define LED2_PAD 3
#define LED3_PAD 4
#define LED4_PAD 5
#define LED5_PAD 6
#define LED6_PAD 7
#define LED7_PAD 8
#define LED8_PAD 9

void mainTask(void *params){
	BlinkAgent blink(LED_PAD);

	CounterAgent counter(
		LED1_PAD, LED2_PAD, LED3_PAD, LED4_PAD,
		LED5_PAD, LED6_PAD, LED7_PAD, LED8_PAD
	);

	printf("Main task started\n");

	blink.start("Blink", TASK_PRIORITY);
	counter.start("Counter", TASK_PRIORITY);

	srand(time_us_32());

	while (true) {
		uint8_t r = rand() & 0xFF;
		counter.blink(r);

		printf("Blinking R=0x%X\n", r);

		vTaskDelay(pdMS_TO_TICKS(3000));
	}
}

void vLaunch(void) {
	TaskHandle_t task;
	xTaskCreate(mainTask, "MainThread", 1024, NULL, TASK_PRIORITY, &task);
	vTaskStartScheduler();
}

int main(void)
{
	stdio_init_all();
	sleep_ms(2000);

	printf("GO\n");
	printf("Starting FreeRTOS on core 0\n");

	vLaunch();

	return 0;
}