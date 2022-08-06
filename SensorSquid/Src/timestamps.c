/*
 * timestamps.c
 *
 *  Created on: Jul 31, 2022
 *      Author: Blane
 */

#include "timestamps.h"

//Timer 1 is setup to autoreload at a period of 1Hz



// volatile because we will have an ISR update it.
// unsigned becuase we can't go back in time from when the car starts
static volatile time_delta time_delta_var = {0};		// Time delta since the

static time_delta prev_time_delta;			// Save the previous time to error check
											// a case where the time just rolls over when getting the time


// Return the time delta at the moment the function is called
time_delta getTime(){

	time_delta time_delta_var_local;

	//Ensure we don't get pre-empted here
	portENTER_CRITICAL();

	time_delta_var_local = time_delta_var;		// Copy the seconds value
	time_delta_var_local.subseconds = (float) TIM1->CNT/TIM1->ARR;  // CNT divided by the auto reload would be the

	portEXIT_CRITICAL();

	//assert(time_delta_var_local.seconds>-1);
	//assert(time_delta_var_local.subseconds<1);

	// Error! Timer CNT rollover during reading
	if(	prev_time_delta.seconds == time_delta_var_local.seconds
		&& prev_time_delta.subseconds > time_delta_var_local.subseconds){
		time_delta_var_local.seconds++;
	}

	prev_time_delta = time_delta_var_local;

	return time_delta_var_local;				// Return the copied value

}

// Update the seconds
// We Elapse a Period at a frequency of 1Hz
void HAL_TimestampUpdate_Callback(TIM_HandleTypeDef *htim){

	if(htim == &htim1){
		time_delta_var.seconds++;
	}

}

