#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../../lib/uart/ascii_special.h"
#include "../../lib/adc/adc.h"
#include "../../lib/motors/motors.h"
#include "../../lib/uart/uart.h"
#include "../../lib/magnometer/magnometer.h"
#include "../../lib/lcd/lcd.h"


//Some easy to read defines
#define PATH1 1
#define PATH2 2
#define PATH3 3
#define MOVE_SPEED 50
#define TURN_SPEED 15



/************************
*   High level movement functions
*************************/
void path1();
void path2();
void path3();
void turn(int degrees);
void forward();
void reverse();

void fire();
#define FIRE		(1 << 4)	//PORTB


void init_data_timer();

char buffer[10];
volatile int object_detected = 0;


int main(){
    DDRB = 0xFF;
    PORTB = 0;
    USART1_Init(MYUBRR);
    USART1_send_string((unsigned char *)"USART1 (Bluetooth: HC-05) Initialized\r\n");
    enable_adc();
    //init_HMC5883L();
    init_motors();
    init_data_timer();
    //sei();
    

    unsigned char data[MAX_STRING_SIZE];
    int i;
    for (i = 0; i < MAX_STRING_SIZE; ++i){
        data[i] = '\0';
    }
    data[MAX_STRING_SIZE - 2] = '~';

    move(50,50,50);
    while (1){
        USART1_Receive_String(data);
        USART1_send_string((unsigned char *)"Data received: ");
        USART1_send_string(data);
        USART1_send_string((unsigned char *)"\r\n");

        if (strcmp((char *)data,"eee~") == 0){
            USART1_send_string((unsigned char *)"Initiating path 1\r\n");
            //disable RX0
            UCSR1B &= ~(1<<RXEN1);
            path2();
            //enable RX0
            UCSR1B |= (1<<RXEN1);
        }
        else if (strcmp((char *)data,"hhh~") == 0){
            USART1_send_string((unsigned char *)"Firing \r\n");
            fire();
        }
        else if (strcmp((char *)data,"222~") == 0){
            USART1_send_string((unsigned char *)"Stopping \r\n");
            move(50,50,0);
        }
        else{
            USART1_send_string((unsigned char *)"Moving: ");
            USART1_send_string(data);
            USART1_send_string((unsigned char*)"\r\n");
            move((float)data[0],(float)data[1],(float)data[2]);
        }

        //_delay_ms(10);
    }

    return 0;
}



/******************
*   Sends the drone info once ever .5 seconds
    Format:
        Depth: ddd.dddddd
        Object: (NO | YES)
        Heading: ddd.dddddd
        Water Level: (OK | WARNING | ERROR) : dd.dd
*******************/
ISR(TIMER3_COMPA_vect){ 
    USART1_send_string((unsigned char*)"Heading: ");
    USART1_send_string((unsigned char*)"Not yet implemented");
    USART1_send_string((unsigned char*)"\r\n");
}


void init_data_timer(){
    //CTC Mode
    TCCR3A |= (1 << COM3A1);
    TCCR3A &= ~(1 << COM3A0);

    //Prescalar 256
    TCCR3B |= (1 << CS32);

    unsigned char sreg;
    /* Save global interrupt flag */ 
    sreg = SREG;
    /* Disable interrupts */ 
    cli();
    /* Set TCNTn to 1 */
    TCNT3 = 1;
    sei();
    /* Restore global interrupt flag */ 
    SREG = sreg;
    ICR3 = 65535;
    OCR3A = 65535;

    //Enable timer3a interrupt
    ETIMSK = (1 << OCIE3A);

}

void path1(){
    //Forward for 2 seconds (about 6 feet)
    // clear_display();
    // string2lcd((unsigned char *)"Forward");
    USART1_send_string((unsigned char*)"Move Forward\r\n");
    move(50 + (MOVE_SPEED/2),50 + (MOVE_SPEED/2),0);
    _delay_ms(2000);
    //Down 3 seconds (aim for about 4 feet)
    // clear_display();
    // string2lcd((unsigned char *)"Down");
    USART1_send_string((unsigned char*)"Move Down\r\n");
    move (50,50,0);
    _delay_ms(3000);
    //spin left 90 degrees
    // clear_display();
    // string2lcd((unsigned char *)"Turn Left");
    USART1_send_string((unsigned char*)"Turn Left\r\n");
    move(50 - MOVE_SPEED/2, 50 + MOVE_SPEED/2, 0);
    _delay_ms(2000);
    //turn(-90);
    //Forward for 2 seconds (about 6 feet)
    // clear_display();
    // string2lcd((unsigned char *)"Forward");
    USART1_send_string((unsigned char*)"Move Forward\r\n");
    move(50 + MOVE_SPEED/2, 50 + MOVE_SPEED/2, 0);
    _delay_ms(2000);
    //Spin left 90 degrees
    // clear_display();
    // string2lcd((unsigned char *)"Turn Left");
    USART1_send_string((unsigned char*)"Turn Left\r\n");
    move(50 - MOVE_SPEED/2, 50 + MOVE_SPEED/2, 0);
    _delay_ms(2000);
    //turn(-90);
    //Forward for 2 seconds (about 6 feet)
    // clear_display();
    // string2lcd((unsigned char *)"Forward");
    USART1_send_string((unsigned char*)"Move Forward\r\n");
    move(50 + MOVE_SPEED/2, 50 + MOVE_SPEED/2, 0);
    _delay_ms(2000);
    //Up 3 seconds (resurface)
    // clear_display();
    // string2lcd((unsigned char *)"Up");
    USART1_send_string((unsigned char*)"Move Up\r\n");
    move(50,50,100);
    //Spin left 90 degrees
    //turn(-90);
    //Forward for 2 seconds (about 6 feet)
    // clear_display();
    // string2lcd((unsigned char *)"Forward");
    USART1_send_string((unsigned char*)"Move Forward\r\n");
    move(50 + MOVE_SPEED/2, 50 + MOVE_SPEED/2, 0);
    //Spin left 90 degrees
    // clear_display();
    // string2lcd((unsigned char *)"Turn Left");
    USART1_send_string((unsigned char*)"Turn Left\r\n");
    move(50 - MOVE_SPEED/2, 50 + MOVE_SPEED/2, 0);
    _delay_ms(2000);
    //turn(-90);
    //Complete (back in some position as start)
}
void path2(){
    USART1_send_string((unsigned char *)"Diving (10 seconds)\r\n");
    move(1,1,0);
    _delay_ms(10000);

    USART1_send_string((unsigned char *)"Turn right (1 second) r\n");
    move(100,1,0);
    _delay_ms(1000);

    USART1_send_string((unsigned char *)"Continue Diving (5 seconds)\r\n");
    move(1,1,0);
    _delay_ms(5000);

    USART1_send_string((unsigned char *)"Resurfacing...\r\n");
    move(100,100,0);
    _delay_ms(5000);

    USART1_send_string((unsigned char *)"Path Complete\r\n");
    move(50,50,50);

}
void path3(){
    //Implement if necessary
}

void turn(int degrees){
    int *x = 0;
    int *y = 0;
    int *z = 0;
    float headingDegrees = getHeading(x,y,z);
    //Get the new heading to aim for
    int new_heading = (int)(headingDegrees + degrees)%360;
    if (degrees < 0){
        //Spin left until new heading
        while ((int)headingDegrees != new_heading){
            move(-TURN_SPEED,TURN_SPEED,50);
            getHeading(x,y,z);
        }
    }
    else if (degrees > 0){
        //Spin right until new heading
        while ((int)headingDegrees != (int)new_heading){
            move(TURN_SPEED,-TURN_SPEED,50);
            getHeading(x,y,z);
        }
    }
}

/**************************
*	Sets the Fire pin high to release the solenoid	
***************************/
void fire(){
    USART1_send_string((unsigned char *)"Drone Fired\r\n");

	PORTB |= FIRE;
	_delay_ms(1000);
	PORTB &= ~FIRE;
}

