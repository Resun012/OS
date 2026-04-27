/*
 * BlinkAgent.cpp
 *
 *  Created on: 15 Aug 2022
 *      Author: jondurrant
 */

#include "BlinkAgent.h"
#include "stdio.h"

#define DELAY_MS 500

BlinkAgent::BlinkAgent(uint8_t gp) {
	xLedPad = gp;
}

BlinkAgent::~BlinkAgent() {
	stop();
}

void BlinkAgent::run(){
	printf("Blink Started\n");

	gpio_init(xLedPad);
	gpio_set_dir(xLedPad, GPIO_OUT);

	while (true) {
		gpio_put(xLedPad, 1);
		vTaskDelay(pdMS_TO_TICKS(DELAY_MS));

		gpio_put(xLedPad, 0);
		vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
	}
}

configSTACK_DEPTH_TYPE BlinkAgent::getMaxStackSize(){
	return 256;
}
