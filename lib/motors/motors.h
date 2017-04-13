#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//Prescaler for PWM signals from the atmega128 manual
#define PRESCALER 8

/**********************
*   T100 Thruster Information
*   Left Motor Signal:  PB5
*   Right Motor Signal: PB6
*   Z Motor Signal:     PB7
***********************/
#define STOP 1500   //1500 usec pulse for calibration
#define MAX_SPEED   3800  //in timer steps -- 1900usec @ 30.517578Hz
#define MIN_SPEED   2200  //in timer steps -- 1100usec @ 30.517578Hz
#define SPEED_RANGE MAX_SPEED - MIN_SPEED
#define STOP_SPEED  SPEED_RANGE/2 + MIN_SPEED



/***********************
*   Remote Controller Information
*   These are the saturation values. 
*   Saturation difference will make it so the values calculated cannot 
*       go below MIN_INPUT + SATURATION_DIFFERENCE
*       and above MAX_INPUT - SATURATION_DIFFERENCE
*   This will essentially limit motor power by SATURATION_DIFFERENCE*2 percent
*************************/
#define MIN_INPUT   0
#define MAX_INPUT   100
#define SATURATE_DIFFERENCE 0 //takes this value away from the max and min
                                //NOTE: This is 100% - SATURATE_DIFFERENCE*2 for
                                //      max power

//Number of OCR1x steps per remote control step
#define STEP        (double)(MAX_INPUT - MIN_INPUT)/((double)(MAX_SPEED - MIN_SPEED))


#define DIRECTION11 (1 << 4)	//PORTE
#define DIRECTION12 (1 << 5)	//PORTE
#define DIRECTION21 (1 << 2)	//PORTE
#define DIRECTION22	(1 << 3)	//PORTE
#define DIRECTION31 (1 << 5)	//PORTE
#define DIRECTION32 (1 << 4)	//PORTE

/*************************
*   PWM and Motor Control Functions
**************************/
void TIM16_WriteTCNT1( unsigned int i );
void move(float left, float right, float z);
void set_16bitPWM1();
void init_esc();
void init_motors();
