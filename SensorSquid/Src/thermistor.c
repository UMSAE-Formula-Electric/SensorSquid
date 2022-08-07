#include "thermistor.h"
#include "math.h"
#include "stdio.h"

uint32_t res[16];

const double A = 1.2794639360 * pow(10, -3);
const double B = 2.6408831422 * pow(10, -4);
const double C = 1.3679771000 * pow(10, -7);

//const double A = 0.011729356353712072;
//const double B = -0.0019172353478349494;
//const double C = 0.000013994069528328213;

const double ADC_TO_Voltage = 3.3 / 4098;
const float vDD = 3.3;
const uint32_t constResistance = 5000;

double get_NTC_Resistance(double voltageReading){
	double R_NTC = (voltageReading / (vDD - voltageReading)) * constResistance;
	return R_NTC;
}

double getTemperature(double voltageReading){		// USING STEINHART & HART EQUATION
	double R_NTC = get_NTC_Resistance(voltageReading);
	double naturalLogR = log(R_NTC);
	double temperature = -273.15 + (1 / (A + B * naturalLogR + C * pow(naturalLogR, 3)));
	return temperature;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	double voltages[16];
	double temperatures[16];
  // Conversion Complete & DMA Transfer Complete As Well
  for(int i = 0; i < 16; i++) {
	res[i] = 0;
  }
  HAL_ADC_Start_DMA(&hadc1, res, 16);

  for(int i = 0; i < 16; i++) {
	  voltages[i] = ADC_TO_Voltage * res[i];
	  temperatures[i] = getTemperature(voltages[i]);
	  printf("%f", temperatures[i]);
  }
}
