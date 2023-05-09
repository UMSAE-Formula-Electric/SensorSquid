//********************************************************************
//
//	@file 		shock_pot.c
//	@author 	Matthew Mora
//	@created	Nov 19, 2022
//	@brief		Calculates flowrate from flowmeters
//
//	@datasheet	https://drive.google.com/file/d/1g9wjH6BT5--y21_IYlu2G4MbX3KbiAo5/view?usp=share_link
//
//*********************************************************************

#include "shock_pot.h"
#include "stdio.h"
#include "adc.h"
#include "sd_card.h"
#include <stdio.h>
#include <string.h>

// variables defined in thermistor.c
extern uint32_t res[16];
extern const double ADC_TO_Voltage;
extern const float vDD;

const float MAX_DIST = 50;	// max travel of shock potentiometer in mm
volatile int newData_dist;	// flag to determine if the ADC has finished a read
volatile double dist[16];	// holds distances read from each ADC input (note: not all inputs are necessarily connected to a shock pot

//*********************************************************************
// getDistance
//
// PURPOSE: This function translates voltages to distance in millimeters (mm)
//
// INPUT PARAMTERS:
//			voltage - voltage measured across the shock pot
//
// RETURN:	distance in mm of type double
//*********************************************************************
double getDistance(double voltage){
	double dist = MAX_DIST * voltage / vDD;
	return dist;
}


// callback entered after ADC conversion (in thermistor.c)
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

//*********************************************************************
// readDist_task
//
// PURPOSE: Main loop for freeRTOS thread. Waits for ADC conversion and
//			translates voltages into distance
//*********************************************************************
void readDist_task(){
	char msg[512];
	char msgDist[20];
	float voltages[16];
	char distMsg[50];

	for (;;){

		HAL_ADC_Start_DMA(&hadc1, res, 16);	// get values from ADC
		while(!newData_dist) {}				// wait until ADC finishes conversion
		newData_dist = 0;					// reset ADC conversion flag

		// calculate distances for each ADC channel
		for(int i = 0; i < 16; i++) {
			  voltages[i] = ADC_TO_Voltage * res[i];
			  dist[i] = getDistance(voltages[i]);
			  sprintf(msgDist, "ADC %d %.5f \n", i, voltages[i]);
			  strcat(msg,msgDist);
		}

		// add ADC channel 0 to message
		sprintf(distMsg, "Distance: %f\r\n", dist[0]);
		HAL_UART_Transmit(&huart1, (uint8_t *) distMsg, strlen(distMsg), 10);

		// log message to SD card
		SD_Log(msg, -1);
		memset(msg, 0, sizeof msg);
		memset(msgDist, 0, sizeof msgDist);

		// wait 10ms
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

//*********************************************************************
// init_readDist_task
//
// PURPOSE: Initializes and starts the freeRTOS task which handles
//			shock pot reading
//*********************************************************************
void init_readDist_task(){
	xTaskCreate(&readDist_task, "readDist_task", 512, ( void * ) 1, 3, NULL);
}
