/*
 * CounterAgent.cpp
 *
 *  Created on: 22 Aug 2022
 *      Author: jondurrant
 */

#include "CounterAgent.h"
#include "stdio.h"

#define COUNT_LEDS 8
#define COUNT_QUEUE_LEN 5
#define COUNT_BLINK_DELAY pdMS_TO_TICKS(500)

enum CounterAction {CounterOff, CounterOn, CounterBlink};

struct CounterCmd {
	CounterAction action;
	uint8_t count;
};

typedef struct CounterCmd CounterCmdT;

CounterAgent::CounterAgent(uint8_t gp1, uint8_t gp2, uint8_t gp3, uint8_t gp4,
						   uint8_t gp5, uint8_t gp6, uint8_t gp7, uint8_t gp8) {

	pLedPads[0] = gp1;
	pLedPads[1] = gp2;
	pLedPads[2] = gp3;
	pLedPads[3] = gp4;
	pLedPads[4] = gp5;
	pLedPads[5] = gp6;
	pLedPads[6] = gp7;
	pLedPads[7] = gp8;

	xCmdQ = xQueueCreate(COUNT_QUEUE_LEN, sizeof(CounterCmdT));
	if (xCmdQ == NULL){
		printf("ERROR: Unable to create Queue\n");
	}
}

CounterAgent::~CounterAgent() {
	stop();
	if (xCmdQ != NULL){
		vQueueDelete(xCmdQ);
	}
}

void CounterAgent::init(){
	for (int i = 0 ; i < COUNT_LEDS; i++){
		gpio_init(pLedPads[i]);
		gpio_set_dir(pLedPads[i], GPIO_OUT);
		gpio_put(pLedPads[i], 0);
	}
}

void CounterAgent::run(){
	BaseType_t res;
	printf("Count Started\n");
	init();

	CounterAction action = CounterOff;
	uint8_t count = 0;
	bool blinkOn = false;
	CounterCmdT cmd;
	bool change;

	if (xCmdQ == NULL){
		return;
	}

	while (true) {
		res = xQueueReceive(xCmdQ, (void *)&cmd, COUNT_BLINK_DELAY);

		if (res == pdTRUE){
			action = cmd.action;
			count = cmd.count;
			change = true;
		} else {
			change = false;
		}

		switch(action){
		case CounterOff:
			if (change) setLeds(0);
			break;

		case CounterOn:
			if (change) setLeds(count);
			break;

		case CounterBlink:
			blinkOn = !blinkOn;
			setLeds(blinkOn ? count : 0);
			break;
		}
	}
}

configSTACK_DEPTH_TYPE CounterAgent::getMaxStackSize(){
	return 256;
}

void CounterAgent::setLeds(uint8_t count){
	for (int i = 0; i < COUNT_LEDS; i++){
		uint8_t m = 1 << i;
		gpio_put(pLedPads[i], (count & m) ? 1 : 0);
	}
}

void CounterAgent::on(uint8_t count){
	CounterCmdT cmd = {CounterOn, count};
	xQueueSendToBack(xCmdQ, &cmd, 0);
}

void CounterAgent::off(){
	CounterCmdT cmd = {CounterOff, 0};
	xQueueSendToBack(xCmdQ, &cmd, 0);
}

void CounterAgent::blink(uint8_t count){
	CounterCmdT cmd = {CounterBlink, count};
	xQueueSendToBack(xCmdQ, &cmd, 0);
}
