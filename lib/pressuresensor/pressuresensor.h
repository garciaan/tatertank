#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include "../adc/adc.h"

//#define PSENSOR_MIN 0.4736328		//voltage at 0 ft (just below water)
volatile float PSENSOR_MIN;

#define PSENSOR_PIN	0		//PORTF pin that is attach to sensor

double get_depth_mpa();
double get_depth_feet();
void calibrate_pressure_sensor();

#endif
