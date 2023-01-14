//***************************
//
//	@file 		flowmeter.h
//	@author 	Matthew Mora
//	@created	January 2nd, 2023
//	@brief		Calculates flowrate from flowmeters
//
//***************************

#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"

//avoid defining multiple times
#ifndef _FLOWMETER
#define _FLOWMETER

// function prototypes
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void getFlowrate_task();
double calculateFlowrate();
void init_getFlowrate_task();


#endif //ifndef _FLOWMETER
