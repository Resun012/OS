/*
 * Agent.cpp
 * Abstract agent interface to an active agent object that runs as
 * FreeRTOS task
 *  Created on: 15 Aug 2022
 *      Author: jondurrant
 */

#include "Agent.h"
#include <string.h>

Agent::Agent() {
}

Agent::~Agent() {
	stop();
}

void Agent::stop(){
	if (xHandle != NULL){
		vTaskDelete(xHandle);
		xHandle = NULL;
	}
}

unsigned int Agent::getStakHighWater(){
	if (xHandle != NULL)
		return uxTaskGetStackHighWaterMark(xHandle);
	else
		return 0;
}

TaskHandle_t Agent::getTask(){
	return xHandle;
}

bool Agent::start(const char *name, UBaseType_t priority){
	BaseType_t res;

	if (strlen(name) >= MAX_NAME_LEN){
		memcpy(pName, name, MAX_NAME_LEN);
		pName[MAX_NAME_LEN-1]=0;
	} else {
		strcpy(pName, name);
	}

	res = xTaskCreate(
		Agent::vTask,
		pName,
		getMaxStackSize(),
		(void *) this,
		priority,
		&xHandle
	);

	return (res == pdPASS);
}

void Agent::vTask(void * pvParameters){
	Agent *task = (Agent *) pvParameters;
	if (task != NULL){
		task->run();
	}
}