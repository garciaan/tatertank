#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define LASER_SENSOR    1   //PORTF PIN 1
#define LASER           0   //PORTB PIN 0


/*************************
*   Distance Sensing Functions
*   To be used with the laser and photoresistor
**************************/
double get_distance(unsigned int pin);
double print_distance(unsigned int pin);
void enable_laser_sensor();
void disable_laser_sensor();
void laser_on();
void laser_off();