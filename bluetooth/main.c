
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define BAUD 1200
#define UBRR 832
#define BLUETOOTH_BAUD 9600
#define BLUETOOTH_UBRR F_CPU/16/BLUETOOTH_BAUD-1

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
#define DIRECTION31 (1 << 5)	//PORTE
#define DIRECTION32 (1 << 4)	//PORTE

#define STEP_TIME 100

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
#include <stdlib.h>
#include <avr/interrupt.h>


void USART_Init( unsigned int ubrr );
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void USART_send_string(char *data);
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
void lookUp();
void lookDown();
void stepUp();
void stepDown();
char nes_data[17];

uint8_t temp, read_byte;



int main(void){
	DDRB = 0xFF;		//set PORTB to OUTPUTS
    PORTB = 0x00;		//set all of portb to low
    //DDRD =0x00;			//Set portD to INPUTS
    //PORTD = 0x00;		//Set all of PORTD to low
    DDRE = 0xFF;		//Set portE to OUTPUTS
	DDRF &= ~(1 << 0);	// PORTF Pin 1 is input for data


    spi_init();
    lcd_init();
	USART_Init(BLUETOOTH_UBRR);
	int i;
	
	//Initialize the buffer to dashes for printing
    for (i = 0; i < 16; ++i){
        nes_data[i] = '-';
    }
	//Set the end of the string
	nes_data[16] = '\0';

    clear_display();
    string2lcd("Starting Program");
	char a = 'a';
    _delay_ms(500);
	while(1){
        
		clear_display();
		USART_Transmit('a');
		a = USART_Receive();
		nes_data[1] = a;
		string2lcd(nes_data);
		_delay_ms(100);
        
	}

	return 0;
}


/**************************
*	Move the motor up for STEP_TIME milliseconds
*	then pause for a second
***************************/
void stepUp(){
	string2lcd("Step Up");
	PORTE |= DIRECTION31;
	PORTE &= ~DIRECTION32;
	_delay_ms(STEP_TIME);
	PORTE |= DIRECTION32;
	_delay_ms(1000);
}

/**************************
*	Move the motor down for STEP_TIME milliseconds
*	then pause for a second
***************************/
void stepDown(){
	string2lcd("Step Down");
	PORTE |= DIRECTION32;
	PORTE &= ~DIRECTION31;
	_delay_ms(STEP_TIME);
	PORTE |= DIRECTION31;
	_delay_ms(1000);
}



/**************************
*	Set the motor to move up 
***************************/
void lookUp(){
	string2lcd("Look Up");
	PORTE |= DIRECTION31;
	PORTE &= ~DIRECTION32;
	_delay_us(6);
}


/**************************
*	Set the motor to move down
***************************/
void lookDown(){
	string2lcd("Look Down");
	PORTE |= DIRECTION32;
	PORTE &= DIRECTION31;
	_delay_us(6);
}


/**************************
*	Sets the Fire pin high to release the solenoid	
***************************/
void fire(){
	string2lcd("Firing");
	
	PORTB |= FIRE;
	_delay_ms(600);
	PORTB &= ~FIRE;
}



/**************************
*	Sets both motors to foward
***************************/
void forward(){
	string2lcd("Forward");
	PORTE |= DIRECTION11;
	PORTE &= ~DIRECTION12;
	PORTE |= DIRECTION21;
	PORTE &= ~DIRECTION22;
	_delay_us(6);
}


/**************************
*	Sets both motors to reverse
***************************/
void reverse(){
	string2lcd("Reverse");
	PORTE &= ~DIRECTION11;
	PORTE |= DIRECTION12;
	PORTE &= ~DIRECTION21;
	PORTE |= DIRECTION22;
	_delay_us(6);
}


/**************************
*	Sets left motor to forward 
*	right motor to reverse
***************************/
void left(){
	string2lcd("Left");
	PORTE |= DIRECTION11;
	PORTE &= ~DIRECTION12;
	PORTE &= ~DIRECTION21;
	PORTE |= DIRECTION22;
	_delay_us(6);
}

/**************************
*	Sets right motor to forward
*	left motor to reverse
***************************/
void right(){
	string2lcd("right");
	PORTE &= ~DIRECTION11;
	PORTE |= DIRECTION12;
	PORTE |= DIRECTION21;
	PORTE &= ~DIRECTION22;
	_delay_us(6);
}


/**************************
*	Sets both motors off
***************************/
void stop(){
	string2lcd("Stop");
	PORTE |= DIRECTION11;
	PORTE |= DIRECTION12;
	PORTE |= DIRECTION21;
	PORTE |= DIRECTION22;
	PORTE |= DIRECTION31;
	PORTE |= DIRECTION32;
	_delay_us(6);
}


/**************************
*	Print the controller data, button by button, to the LCD
***************************/
void print_nes(){
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


/**************************
*	Raises and lowers the latch pin 
*	for the nes controller, which 
*	grabs the newest data from the 
*	nes controller and stores it in the
*	shift register
***************************/
void latch(){
	PORTB |= LATCH;
	_delay_us(12);
	PORTB &= ~LATCH;
	_delay_us(6);
}


/**************************
*	Sets the clock pin to state
*	for the nes controller. This 
*	tells the nes controller to 
*	send the next bit (button state)
***************************/
void clk(int state){
	if (state){
		PORTB |= CLK;
	}
	else {
		PORTB &= ~CLK;
	}
	_delay_us(6);
}


/**************************
*	Get all data from the controller
*	and store it in nes_data
***************************/
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

/**************************
*	Initializes uart1 with the given ubrr
***************************/
void USART_Init( unsigned int ubrr ) {
    /* Set baud rate */
    UBRR1H = (unsigned char)(ubrr>>8);
    UBRR1L = (unsigned char)ubrr;
    /* Enable receiver and transmitter */ 
    UCSR1B = (1<<RXEN1)|(1<<TXEN1);
    /* Set frame format: 9data, 1stop bit */ 
    UCSR1C = (1 << UCSZ12) | (1 << UCSZ11) | (1 << UCSZ10);
}


/**************************
*	Transmits the given byte over uart1
***************************/
void USART_Transmit( unsigned char data ) {
    /* Wait for empty transmit buffer */ 
    while ( !( UCSR1A & (1<<UDRE1)) );
    /* Put data into buffer, sends the data */ 
    UDR1 = data;
}


/**************************
*	Wait for data to be received, then
*	return the data from UDR1
***************************/
unsigned char USART_Receive( void ) {
	/* Wait for data to be received */ //
	while ( !(UCSR1A & (1<<RXC1)) ){
	}
	/* Get and return received data from buffer */ 
	
	return UDR1;
}

void USART_send_string(char *data){
	int i = 0;
	while (data[i] != '\0'){
		USART_Transmit(data[i]);
		++i;
	}
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
    _delay_ms(2.6);   //obligatory waiting for slow LCD
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
