#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>



//#define VREF        2.56    //Vref voltage for ADC
#define VREF            5
void enable_adc();
int read_adc(int pin);
double get_voltage(int adc);

