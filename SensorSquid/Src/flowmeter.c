#include "flowmeter.h"

volatile int pulse_count = 0;
const int PPL = 2200;	//pulse per liter
const int DELAY = 500;	//in ms
static const double conversionFactor = (1000/DELAY)*(1.0/1000.0); //convert L/s to m^3/s


//might need to make atomic
//gets flowrate as m^3/s
double calculateFlowrate(){
	volatile double flowrate;

	flowrate = pulse_count*conversionFactor/PPL;
	pulse_count = 0;

	return flowrate;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t ICValue;
	if(htim->Instance == TIM12)
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
	volatile double flowrate;

	for (;;){
		vTaskDelay(pdMS_TO_TICKS(DELAY));

		flowrate = calculateFlowrate();
	}
}

void init_getFlowrate_task(){
	xTaskCreate(&getFlowrate_task, "getFlowrate_task", 512, ( void * ) 1, 3, NULL);
}
