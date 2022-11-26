/*
 * thermistor.h
 *
 *  Created on: Aug 6, 2022
 *      Author: tonyz
 */

#include "adc.h"

#ifndef THERMISTOR_H_
#define THERMISTOR_H_



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void init_readTemp_task();

#endif /* THERMISTOR_H_ */
