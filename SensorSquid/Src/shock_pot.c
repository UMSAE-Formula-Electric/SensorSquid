#include "shock_pot.h"
//#include "math.h"
#include "stdio.h"
#include "adc.h"
#include "sd_card.h"
#include <stdio.h>
#include <string.h>

extern uint32_t res[16];

extern const double ADC_TO_Voltage;
extern const float vDD;
const float MAX_DIST = 50;
volatile int newData_dist;
double volatile dist[16];

double getDistance(double voltage){
	double dist = MAX_DIST * voltage / vDD;
	return dist;
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//	newData = 1;
//	double voltages[16];
//	double dist[16];
//	// Conversion Complete & DMA Transfer Complete As Well
//	for(int i = 0; i < 16; i++) {
//		res1[i] = 0;
//	}
//	HAL_ADC_Start_DMA(&hadc1, res1, 16);
//
//	for(int i = 0; i < 16; i++) {
//		voltages[i] = ADC_TO_Voltage * res1[i];
//		dist[i] = getDistance(voltages[i]);
//		printf("%f", dist[i]);
//	}
//}

void readDist_task(){
	char msg[512];
	char msgDist[20];
	float voltages[16];
	char distMsg[50];

	for (;;){

		HAL_ADC_Start_DMA(&hadc1, res, 16);
		while(!newData_dist) {}
		newData_dist = 0;

		for(int i = 0; i < 16; i++) {
			  voltages[i] = ADC_TO_Voltage * res[i];
			  dist[i] = getDistance(voltages[i]);
			  sprintf(msgDist, "ADC %d %.5f \n", i, voltages[i]);
			  strcat(msg,msgDist);
		}
		sprintf(distMsg, "Distance: %f\r\n", dist[0]);
		HAL_UART_Transmit(&huart1, (uint8_t *) distMsg, strlen(distMsg), 10);

		SD_Log(msg, -1);
		memset(msg, 0, sizeof msg);
		memset(msgDist, 0, sizeof msgDist);
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void init_readDist_task(){
	xTaskCreate(&readDist_task, "readDist_task", 512, ( void * ) 1, 3, NULL);
}
