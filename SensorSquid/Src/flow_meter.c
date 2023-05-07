/*
 * flow_meter.c
 *
 *  Created on: Aug 7, 2022
 *      Author: Brett
 */

#include "flow_meter.h"
#include "timestamps.h"
#include "sd_card.h"

int pulses_count = 0;
float prev_delta;

static TaskHandle_t	flowRatePost_taskHandle;

//timer 2 interrupt handler
void HAL_Flow_Meter_Callback(void) {

	pulses_count++;
}

float pulses_to_ml(){
	return (pulses_count / ppl) * 1000;
}


void flowRatePostStateTask(){
	for(;;){
		vTaskDelay(pdMS_TO_TICKS(1/fm_post_rate));
		char msg[256] =  {0};
		time_delta td;
		float timedelt;
		float rate;

		td = getTime();
		timedelt = (float)td.seconds + td.subseconds;

		rate = pulses_to_ml()/(timedelt - prev_delta);

		sprintf(msg, "Delta: %f, FlowRate: %f", timedelt, rate);

		SD_Log(msg, -1);

		pulses_count = 0;
		prev_delta = timedelt;
	}
}

void init_flowRate_post_task(){
	prev_delta = 0;
	xTaskCreate(&flowRatePostStateTask, "flowRatePostTask", 200, NULL, 3, &flowRatePost_taskHandle);
}
