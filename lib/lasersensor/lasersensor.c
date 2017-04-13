#include "lasersensor.h"

double get_distance(unsigned int pin){
    return 0;
}

double print_distance(unsigned int pin){
    return 0;
}
void enable_laser_sensor(){
    //disable analog comparator interrupt, clear ACSR:ACIE
    ACSR &= ~(1 << ACIE);

    //enable analog comparator multiplexer, set SFIOR:ACME
    SFIOR |= (1 << ACME);

    //set AIN1 to laser sensor pin
    ADMUX &= 0b11111000; //clear the admux input pin bits
    ADMUX |= LASER_SENSOR;  //set the new bits
    
    //enable analog comparator, clear ACSR:ACD
    ACSR &= ~(1 << ACD);
    
    //set comparator interrupt on rising output edge, set ACSR:ACIS1, set ACSR:ACIS0
    ACSR |= (1 << ACIS1) | (1 << ACIS0);
    
    //enable analog comparator interrupt, set ACSR:ACIE
    ACSR |= (1 << ACIE);
    
    //Be sure to handle the interrupt until ACSR:ACO goes low
}
void disable_laser_sensor(){
    //disable analog comparator interrupt, clear ACSR:ACIE
    ACSR &= ~(1 << ACIE);

    //disable analog comparator, set ACSR:ACD
    ACSR |= (1 << ACD);

    //disable analog comparator multiplexor, clear SFIOR:ACME
    SFIOR &= ~(1 << ACME);
}

void laser_off(){
    PORTB |= (1 << LASER);
}
void laser_on(){
    PORTB &= ~(1 << LASER);
}



