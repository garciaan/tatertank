#ifndef	PRESSURESENSOR_C
#define	PRESSURESENSOR_C

#include "pressuresensor.h"

void calibrate_pressure_sensor(){
	PSENSOR_MIN = get_voltage(PSENSOR_PIN);;
}

double get_depth_mpa(){
	return 1.2 * (get_voltage(PSENSOR_PIN) - PSENSOR_MIN)/(4.5-PSENSOR_MIN);
}
double get_depth_feet(){
	return 334.56229215 * get_depth_mpa();
}



#endif