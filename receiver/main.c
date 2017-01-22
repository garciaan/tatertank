
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define BAUD 1200
#define UBRR 832
#define P1 0x01
#define P2 0x02
#define P3 0x04
#define P4 0x08
#define P5 0x10
#define P6 0x20
#define P7 0x40
#define P8 0x80

//NES Controller
#define LATCH	(1 << 7)	//PORTB
#define CLK		(1 << 6)	//PORTB
#define DATA	(1 << 0)	//PINF
#define HIGH 1
#define LOW 0
#define ON '1'
#define OFF '0'

//Motor control
#define DIRECTION11 (1 << 0)	//PORTE
#define DIRECTION12 (1 << 1)	//PORTE
#define DIRECTION21 (1 << 2)	//PORTE
#define DIRECTION22	(1 << 3)	//PORTE

//Other Outputs
#define FIRE		(1 << 5)	//PORTB


enum BUTTON {
	B,
	Y,
	Select,
	Start,
	Up,
	Down,
	Left,
	Right,
	A,
	X,
	L,
	R
};

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>



void USART_Init( unsigned int ubrr );
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
char* USART_get_string(void);
void home_line2(void);
void string2lcd(char *lcd_str);
void strobe_lcd(void);
void clear_display(void);
void home_line2(void);
void char2lcd(char a_char);
void string2lcd(char *lcd_str);
void spi_init(void);
void lcd_init(void);
void trigger(unsigned int pin);
double get_distance(unsigned int pin);
double print_distance(unsigned int pin);
void blink(int led, int speed);
void latch();
void clk(int state);
void read_data();
void print_nes();
void right();
void left();
void forward();
void reverse();
void left();
void right();
void stop();
void fire();

char nes_data[17];

uint8_t temp, read_byte;



int main(void){
	DDRB = 0xFF;
    PORTB = 0x00;
    DDRD =0x00;
    PORTD = 0x00;
    DDRE = 0xFF;    //OUTPUTS
	DDRF &= ~(1 << 0); // PORTF Pin 1 is input for data


	char buffer[16];
    spi_init();
    lcd_init();
	USART_Init(UBRR);
	int i;
    
    for (i = 0; i < 16; ++i){
        nes_data[i] = '-';
    }
	nes_data[16] = '\0';
    clear_display();
    string2lcd("Starting Program");

    _delay_ms(500);
    while(1){
        
		clear_display();
        switch(USART_Receive()){
            case Up:
                forward();
                break;
            case Down:
                reverse();
                break;
            case Right:
                right();
                break;
            case Left:
                left();
                break;
            case R:
                fire();
                break;

            default:
                stop();
                break;
        }
    
		
		_delay_ms(60);
		
        
	}

	return 0;
}
void fire(){
	string2lcd("Firing");
	PORTB |= FIRE;
	_delay_ms(200);
	PORTB &= ~FIRE;
}

void forward(){
	string2lcd("Forward");
	PORTE |= DIRECTION11;
	PORTE &= ~DIRECTION12;
	PORTE |= DIRECTION21;
	PORTE &= ~DIRECTION22;
	_delay_us(6);
}
void reverse(){
	string2lcd("Reverse");
	PORTE &= ~DIRECTION11;
	PORTE |= DIRECTION12;
	PORTE &= ~DIRECTION21;
	PORTE |= DIRECTION22;
	_delay_us(6);
}
void right(){
	string2lcd("Right");
	PORTE |= DIRECTION11;
	PORTE &= ~DIRECTION12;
	PORTE &= ~DIRECTION21;
	PORTE |= DIRECTION22;
	_delay_us(6);
}
void left(){
	string2lcd("Left");
	PORTE &= ~DIRECTION11;
	PORTE |= DIRECTION12;
	PORTE |= DIRECTION21;
	PORTE &= ~DIRECTION22;
	_delay_us(6);
}
void stop(){
	string2lcd("Stop");
	PORTE |= DIRECTION11;
	PORTE |= DIRECTION12;
	PORTE |= DIRECTION21;
	PORTE |= DIRECTION22;
	_delay_us(6);
}

void print_nes(){
	/*
	//clear_display();
	if (nes_data[A] == ON)
		string2lcd("A");
	if (nes_data[B] == ON)
		string2lcd("B");
	if (nes_data[Y] == ON)
		string2lcd("Y");
	if (nes_data[X] == ON)
		string2lcd("X");
	if (nes_data[Start] == ON)
		string2lcd("S");
	if (nes_data[Select] == ON)
		string2lcd("E");
	if (nes_data[Left] == ON)
		string2lcd("L");
	if (nes_data[Up] == ON)
		string2lcd("U");
	if (nes_data[Down] == ON)
		string2lcd("D");
	if (nes_data[Right] == ON)
		string2lcd("R");
	if (nes_data[R] == ON)
		string2lcd("F");
	if (nes_data[L] == ON)
		string2lcd("C");
	*/
	char buttons[17];
	int i;
	for (i = 0; i < 16; ++i){
		buttons[i] = '-';
	}
	if (nes_data[A] == ON){
		buttons[A] = 'A';
	}
	if (nes_data[B] == ON){
		buttons[B] = 'B';
	}
	if (nes_data[X] == ON){
		buttons[X] = 'X';
	}
	if (nes_data[Y] == ON){
		buttons[Y] = 'Y';
	}
	if (nes_data[Start] == ON){
		buttons[Start] = 'S';
	}
	if (nes_data[Select] == ON){
		buttons[Select] = 'L';
	}
	if (nes_data[Up] == ON){
		buttons[Up] = 'U';
	}
	if (nes_data[Down] == ON){
		buttons[Down] = 'D';
	}
	if (nes_data[Left] == ON){
		buttons[Left] = 'L';
	}
	if (nes_data[Right] == ON){
		buttons[Right] = 'R';
	}
	if (nes_data[L] == ON){
		buttons[L] = 'C';
	}
	if (nes_data[R] == ON){
		buttons[R] = 'F';
	}
	buttons[16] = '\0';
	string2lcd(buttons);
}

void latch(){
	PORTB |= LATCH;
	_delay_us(12);
	PORTB &= ~LATCH;
	_delay_us(6);
}

void clk(int state){
	if (state){
		PORTB |= CLK;
	}
	else {
		PORTB &= ~CLK;
	}
	_delay_us(6);
}

void read_data(){
	int i;
	latch();
	for (i = 0; i < 16; ++i){
		clk(LOW);
		if (i <= 11){
			if (~PINF & DATA){
				nes_data[i] = ON;
			}
			else {
				nes_data[i] = OFF;
			}
		}
		clk(HIGH);
	}

}
void blink(int led, int speed){
    if (speed < 0){
        speed = 0;
    }
    if (speed > 100){
        speed = 100;
    }
    int i;
    PORTB |= (1 << led);
    for (i = 0; i < (121-speed); ++i){
        _delay_ms(1);
    }
    PORTB &= ~(1 << led);
    /*
    int wait = 0;
    int i;
    if (speed <= 25){
        wait = 100;
    }
    else if (speed > 25 && speed <= 50){
        wait = 60;
    }
    else if (speed > 50 && speed <= 75){
        wait = 30;
    }
    else {
        wait = 10;
    }
    PORTB |= (1 << led);
    for (i = 0; i < wait; ++i){
        _delay_ms(1);
    }
    PORTB &= ~(1 << led);
    */
}
double print_distance(unsigned int pin){
    char str[16];
    double distance = 0;
    distance = get_distance(pin);
    dtostrf(distance,1,6,str);
    string2lcd(str);
    return distance;
}

void trigger(unsigned int pin){
    PORTE |= pin;
    _delay_us(15);
    PORTE &= ~pin;
}

double get_distance(unsigned int pin){
    double distance = 0;
    int count = 0;
    trigger(pin);
    while ((PINE & (pin << 1)) == 0); //while pinc port 1 is low, aka wait for echo to raise
    while (1){
        if ((PINE & (pin << 1)) != (pin << 1)){ //wait for pinc port 1 to go back to low
            break;
        }
        if (count == 255){
            break;
        }
        _delay_us(50);
        ++count;
    }

    distance = (double)count * 40;
    distance /= 58;


    return distance;
}


void USART_Init( unsigned int ubrr ) {
    /* Set baud rate */
    UBRR1H = (unsigned char)(ubrr>>8);
    UBRR1L = (unsigned char)ubrr;
    /* Enable receiver and transmitter */ 
    UCSR1B = (1<<RXEN1)|(1<<TXEN1);
    /* Set frame format: 8data, 2stop bit */ 
    UCSR1C = (1<<USBS1)|(3<<UCSZ01);
}
void USART_Transmit( unsigned char data ) {
    /* Wait for empty transmit buffer */ 
    while ( !( UCSR1A & (1<<UDRE1)) );
    /* Put data into buffer, sends the data */ 
    UDR1 = data;
}

unsigned char USART_Receive( void ) {
/* Wait for data to be received */ //while ( !(UCSR1A & (1<<RXC)) );
/* Get and return received data from buffer */ return UDR1;
}

//twiddles bit 3, PORTF creating the enable signal for the LCD
void strobe_lcd(void){
    PORTF |= 0x08;
    PORTF &= ~0x08;
}

void clear_display(void){
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0x00;    //command, not data
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0x01;    //clear display command
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd();   //strobe the LCD enable pin
    _delay_ms(1.6);   //obligatory waiting for slow LCD
}

void home_line2(void){
    SPDR = 0x00;    //command, not data
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0xC0;   // cursor go home on line 2
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd(); 
    _delay_us(37);
}

//sends a char to the LCD
void char2lcd(char a_char){
    //sends a char to the LCD
    //usage: char2lcd('H');  // send an H to the LCD
    SPDR = 0x01;   //set SR for data xfer with LSB=1
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = a_char; //send the char to the SPI port
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd();  //toggle the enable bit
    _delay_us(37);
}

//sends a string in FLASH to LCD
void string2lcd(char *lcd_str){
    int count;
    for (count=0; count<=(strlen(lcd_str)-1); count++){
        while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
        SPDR = 0x01; //set SR for data
        while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
        SPDR = lcd_str[count]; 
        while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
        strobe_lcd();
        _delay_us(37);  // Max delay for this function is 48us
    }
}   

/* Run this code before attempting to write to the LCD.*/
void spi_init(void){
    DDRF |= 0x08;  //port F bit 3 is enable for LCD
    PORTB |= 0x00; //port B initalization for SPI
    DDRB |= 0x07;  //Turn on SS, MOSI, SCLK 
    //Master mode, Clock=clk/2, Cycle half phase, Low polarity, MSB first  
    SPCR = 0x50;
    SPSR = 0x01;
}

//initialize the LCD to receive data
void lcd_init(void){
    int i;
    //initalize the LCD to receive data
    _delay_ms(15);   
    for(i=0; i<=2; i++){ //do funky initalize sequence 3 times
        SPDR = 0x00;
        while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
        SPDR = 0x30;
        while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
        strobe_lcd();
        _delay_us(37);
    }

    SPDR = 0x00;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0x38;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd();
    _delay_us(37);

    SPDR = 0x00;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0x08;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd();
    _delay_us(37);

    SPDR = 0x00;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0x01;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd();
    _delay_ms(1.6);

    SPDR = 0x00;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0x06;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd();
    _delay_us(37);

    SPDR = 0x00;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    SPDR = 0x0E;
    while (!(SPSR & 0x80)) {}   // Wait for SPI transfer to complete
    strobe_lcd();
    _delay_us(37);
}
