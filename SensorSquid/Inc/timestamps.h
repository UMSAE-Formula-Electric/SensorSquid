/*
 * timestamps.h
 *
 *  Created on: Aug 4, 2022
 *      Author: Blane
 */

#ifndef TIMESTAMPS_H_
#define TIMESTAMPS_H_

#include "tim.h"
#include "FreeRTOS.h"
#include "assert.h"


typedef struct time_delta{
	uint64_t seconds;			// 0-Highest value of uint64 do the math
	float	subseconds;			// 0-1: Fraction of time between seconds
}time_delta;

time_delta getTime();

void HAL_TimestampUpdate_Callback(TIM_HandleTypeDef *htim);

#endif /* TIMESTAMPS_H_ */
