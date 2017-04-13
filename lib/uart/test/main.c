#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "../../lib/uart/ascii_special.h"
#include "../../lib/adc/adc.h"
#include "../../lib/motors/motors.h"
#include "../../lib/uart/uart.h"
#include "../../lib/lcd/lcd.h"
#include "../../lib/magnometer/magnometer.h"
#include "../../lib/lasersensor/lasersensor.h" //NOT YET IMPLEMENTED


//Some easy to read defines
#define PATH1 1
#define PATH2 2
#define PATH3 3
#define MOVE_SPEED 50
#define TURN_SPEED 15
#define STABLE_Z 50



/************************
*   High level movement functions
*************************/
void path1();
void path2();
void path3();
void turn(int degrees);
void forward();
void reverse();



int main(){
    DDRB = 0xFF;
    PORTB = 0x00;
    USART0_Init(MYUBRR);
    enable_adc();
    spi_init();
    lcd_init();
    int on;
    int off;
    double voltage;
    //init_HMC5883L();
    //init_motors();
    unsigned char data[100];
    /*****************
    *   THIS IS THE TEST FILE
    *******************/
    while (1){
        laser_on();
        //_delay_ms(100);
        on = read_adc(LASER_SENSOR);
        USART0_send_string((unsigned char*)"ADC Value ON: ");

        USART0_send_string((unsigned char*)itoa(on,(char *)data,10));
        clear_display();
        string2lcd((unsigned char*)itoa(on,(char *)data,10));
        //USART0_send_string("   ");
        //_delay_ms(100);
        //laser_on();
        //_delay_ms(100);
        //off = read_adc(LASER_SENSOR);
        //USART0_send_string((unsigned char*)"ADC Value OFF: ");
        //USART0_send_string((unsigned char*)itoa(off,(char *)data,10));
        //USART0_Transmit(10);
        _delay_ms(100);
        //USART0_send_string((unsigned char*)"Difference: ");
        //USART0_send_string((unsigned char*)itoa((on-off),(char *)data,10));
        //USART0_Transmit(10);
        USART0_Transmit(10);
    }

    return 0;
}

void path1(){
    //Forward for 2 seconds (about 6 feet)
    move(50 + (MOVE_SPEED/2),50 + (MOVE_SPEED/2),STABLE_Z);
    _delay_ms(2000);
    //Down 3 seconds (aim for about 4 feet)
    move (50,50,0);
    _delay_ms(3000);
    //spin left 90 degrees
    turn(-90);
    //Forward for 2 seconds (about 6 feet)
    move(50 + MOVE_SPEED/2, 50 + MOVE_SPEED/2, STABLE_Z);
    _delay_ms(2000);
    //Spin left 90 degrees
    turn(-90);
    //Forward for 2 seconds (about 6 feet)
    move(50 + MOVE_SPEED/2, 50 + MOVE_SPEED/2, STABLE_Z);
    _delay_ms(2000);
    //Up 3 seconds (resurface)
    move(50,50,100);
    //Spin left 90 degrees
    turn(-90);
    //Forward for 2 seconds (about 6 feet)
    move(50 + MOVE_SPEED/2, 50 + MOVE_SPEED/2, STABLE_Z);
    //Spin left 90 degrees
    turn(-90);
    //Complete (back in some position as start)
}
void path2(){
    //Implement if necessary
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



