#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"

//avoid defining multiple times
#ifndef _FLOWMETER
#define _FLOWMETER

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void getFlowrate_task();
double calculateFlowrate();
void init_getFlowrate_task();


#endif //ifndef _FLOWMETER
