#include "flowmeter.h"

static int pulse_count = 0;
const int PPL = 2200;
static const int conversionFactor = 60*1000; //convert L/ms to L/min
const TickType_t xDelay = 10 / portTICK_PERIOD_MS;	//delay 10ms

//might need to make atomic
float calculateFlowrate(){
	float flowrate;

	flowrate = pulse_count/10.0*conversionFactor;
	pulse_count = 0;

	return flowrate;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t ICValue;
	if(htim->Instance == TIM12 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		ICValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

		//count time
		if(ICValue != 0)
		{
			pulse_count++;
		}
	}
}

void getFlowrate_task(){
	float flowrate;

	for (;;){
		vTaskDelay(xDelay);

		flowrate = calculateFlowrate();
	}
}

void init_getFlowrate_task(){
	xTaskCreate(&getFlowrate_task, "getFlowrate_task", 512, ( void * ) 1, 3, NULL);
}
