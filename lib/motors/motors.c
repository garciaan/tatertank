#include "motors.h"

void init_motors(){
    set_16bitPWM1();
}

void init_esc(){
    int i;
    for (i = 0; i < 3; ++i){
        PORTB |= (1 << 4);
        _delay_us(STOP);
        PORTB &= ~(1 << 4);
        _delay_ms(1000);
    }
}

void set_16bitPWM1(){
    //16-bit fast pwm non-inverting on PB5
    TCCR1A |= (1 << COM1A1); //inverting

    //16-bit fast pwm non-inverting on PB6
    TCCR1A |= (1 << COM1B1); //non-inverting

    //16-bit fast pwm non-inverting on PB7
    TCCR1A |= (1 << COM1C1); //non-inverting

    //Fast PWM w/ TOP ICR1
    TCCR1A |= (1 << WGM11); 
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    
    switch (PRESCALER){
        case 1:
            TCCR1B |= (1 << CS10); //244.140625 Hz
            break;
        case 8:
            TCCR1B |= (1 << CS11); //30.517578 Hz
            break;
        default:                    //prescaler 1
            TCCR1B |= (1 << CS10); //244.140625 Hz
            break;
    }
    

    TIM16_WriteTCNT1(1);
    ICR1 = (unsigned int) 65535;
    _delay_ms(100);
}

void TIM16_WriteTCNT1( unsigned int i ) {
    unsigned char sreg;
    /* Save global interrupt flag */ 
    sreg = SREG;
    /* Disable interrupts */ 
    cli();
    /* Set TCNTn to i */
    TCNT1 = i;
    sei();
    /* Restore global interrupt flag */ 
    SREG = sreg;
}

/*
    OCR1A = Left motor PB5
    OCR1B = Right motor PB6
    maybe OCR1C = Z motor? PB7

    NOTE: Does not activate AfroESC with Simonk firmware given less than 6
*/
void move(float left, float right, float z){
    
    if (left < (MIN_INPUT + SATURATE_DIFFERENCE)){
        left = MIN_INPUT + SATURATE_DIFFERENCE;
    }
    if (left > (MAX_INPUT - SATURATE_DIFFERENCE)){
        left = MAX_INPUT - SATURATE_DIFFERENCE;
    }
    if (right < (MIN_INPUT + SATURATE_DIFFERENCE)){
        right = MIN_INPUT + SATURATE_DIFFERENCE;
    }
    if (right > (MAX_INPUT - SATURATE_DIFFERENCE)){
        right = MAX_INPUT - SATURATE_DIFFERENCE;
    }
    if (z < (MIN_INPUT + SATURATE_DIFFERENCE)){
        z = MIN_INPUT;
    }
    if (z > (MAX_INPUT - SATURATE_DIFFERENCE)){
        z = MAX_INPUT;
    }
    unsigned int left_speed, right_speed, z_speed;
    if (left < 50){
        //Set left motor to reverse
        PORTE &= ~DIRECTION11;
        PORTE |= DIRECTION12;
        PORTB |= (1 << 0);
        PORTB &= ~(1 << 0);
        //left_speed = 0;
        left_speed = ((50-left)/50.0) * 65535.0;
    }
    else if (left > 50){
        //Set left motor to forward
        PORTE |= DIRECTION11;
        PORTE &= ~DIRECTION12;
        PORTB |= (1 << 1);
        PORTB &= ~(1 << 0);
        left_speed = (left-50)/50.0 * 65535.0;
    }
    else {
        left_speed = 0;
        PORTB |= (1 << 0);
        PORTB |= (1 << 1);
    }
    if (right < 50){
        //Set right motor to reverse
        PORTE &= ~DIRECTION21;
        PORTE |= DIRECTION22;
        right_speed = (50-right)/50.0 * 65535.0;
        //right_speed = 0;
    }
    else if (right > 50){
        //Set right motor to forward
        PORTE |= DIRECTION21;
        PORTE &= ~DIRECTION22;
        right_speed = (right-50)/50.0 * 65535.0;
    }
    else {
        right_speed = 0;
    }

    
    
    z_speed = 0;
    OCR1A = left_speed;
    OCR1B = right_speed;
    OCR1C = z_speed;

}




