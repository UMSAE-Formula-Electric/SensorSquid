//***************************
//
//	@file 		flowmeter.c
//	@author 	Matthew Mora
//	@created	January 2nd, 2023
//	@brief		Calculates flowrate from flowmeters
//
//	@datasheet	https://drive.google.com/file/d/1Gfxb1W2j5wihmmY6867XOjltrr32dhPV/view?usp=share_link
//
//***************************

#include "flowmeter.h"

volatile int pulse_count = 0;											// tracks number of rising edges from flowmeter
const int PPL = 2200;													// pulse per liter
const int DELAY = 500;													// time in ms
static const double conversionFactor = (1000/DELAY)*(1.0/1000.0);		// convert L/s to m^3/s


// gets flowrate as m^3/s
double calculateFlowrate(){
	volatile double flowrate;

	flowrate = pulse_count*conversionFactor/PPL;	// calculate flowrate
	pulse_count = 0;								// reset pulse_count

	return flowrate;
}

// function called when timer interrupt occurs
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t ICValue;

	// when interrupt is caused by timer 12
	if(htim->Instance == TIM12)
	{

		ICValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

		// check if timer is initialized
		if(ICValue != 0)
		{
			pulse_count++;	// increment pulse_count
		}
	}
}

// calculates flowrate every DELAY ms
void getFlowrate_task(){
	volatile double flowrate;

	for (;;){
		vTaskDelay(pdMS_TO_TICKS(DELAY));	// wait DELAY ms

		flowrate = calculateFlowrate();		// calculates flowrate
	}
}

// creates FreeRTOS task
void init_getFlowrate_task(){
	xTaskCreate(&getFlowrate_task, "getFlowrate_task", 512, ( void * ) 1, 3, NULL);
}
