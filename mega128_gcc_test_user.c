/*********************************************************************
Property of TekBots program, EECS, Oregon State University
Project : mega128.2 test code
Date    : 12/17/2004
Authors : Roger Traylor and the TekBots team
Comments: 
This code will test both the mega128.2 and its predecessor,
the mega128.1.  However, comments have been written for testing the
mega128.2.  
The full testing requires a DB9 loop back device, an LCD screen,and jumpers
on J5, J7, J9, J10, J11, and J12.

If errors occur during any of the following tests, The board will attempt to help 
the user remedy the board problems by giving helpfull hints and suggestions.  

1. The user will be required to perform a switch test that requires every switch 
to be pressed at least once. The LEDs L1-L8 will light as each switch is pressed.

2. Next a UART test is performed at the users request. If performed pins 2 and 3 of 
the DB9 connector must be shorted together.

3. An IR test is then performed. L3 will blink in unison with the 'IR transmitt led,'
L12 when functioning correctly.

4. The final test is the external ram test. The avr will perform a self test of 
memory region from 0x8000 - 0xFFFF.

A final summary displays the test results: "P" for pass and "N" for not passed.

THE 4 TESTS (assumes jumpers installed per above)
IR_GND: A problem that came with the mega128.2.  The metal shield must
	be grounded until another solution is found. Fails if PIND.2 is
	being pulled low by the IR receiver.                  
IR:	Tests the IR by transmitting a character then reading it in by
	a IR loopback.  Fails if character received does not match the
	character transmistted.
UART:	Similar to the IR test.  Uses UART0and  a loopback on the DB9.
	Fails if the character received does not match the character sent.
RAM:	Writes and reads 4 characters to each memory space of the external
	SRAM.  Fails if any of the 4 characters in any of the memory locations
	fails to return the value that was just written to it.
*********************************************************************/

#define F_CPU 16000000UL
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

void home_line2(void);
void string2lcd(char *lcd_str);

uint8_t temp, read_byte;

char uart_test[11]         = {"UART Test"};
char switch_test_start[17] = {"Test pushbuttons"}; 
char ir[4]                 = {"IR:"};
char ir_gnd[8]             = {"IR_GND:"};
char ram[5]                = {"RAM:"};
char uart[6]               = {"UART:"};
char uart_que[14]	   = {"Do UART Test?"};
char select[15]		   = {"S8-Yes | S1-No"};
char pass[5]		   = {"Pass"};
char fail[5]		   = {"Fail"};
char ram_jumpers[16]	   = {"Check/Insert J5"};
char retest[8]		   = {"Retest?"};
char complete[11]      	   = {"Tests Done"};
char ir_jumpers[16]	   = {"Check J10 & J11"};
char ir_que[12]		   = {"Do IR Test?"};
char no_gnd[16]		   = {"IR Shield Float"};
char skip[13]		   = {"Skip IR Test"};
char uart_jumpers[14]	   = {"Check J7 & J9"};

//twiddles bit 3, PORTF creating the enable signal for the LCD
void strobe_lcd(void){
	PORTF |= 0x08;
	PORTF &= ~0x08;
}

void clear_display(void){
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0x00;    //command, not data
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0x01;    //clear display command
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd();   //strobe the LCD enable pin
	_delay_ms(1.6);   //obligatory waiting for slow LCD
}

void home_line2(void){
	SPDR = 0x00;    //command, not data
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0xC0;   // cursor go home on line 2
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd(); 
	_delay_us(37);
}

//sends a char to the LCD
void char2lcd(char a_char){
 	//sends a char to the LCD
	//usage: char2lcd('H');  // send an H to the LCD
	SPDR = 0x01;   //set SR for data xfer with LSB=1
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = a_char; //send the char to the SPI port
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd();  //toggle the enable bit
	_delay_us(37);
}

//sends a string in FLASH to LCD
void string2lcd(char *lcd_str){
	int count;
	for (count=0; count<=(strlen(lcd_str)-1); count++){
		while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
		SPDR = 0x01; //set SR for data
		while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
		SPDR = lcd_str[count]; 
		while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
		strobe_lcd();
		_delay_us(37);	// Max delay for this function is 48us
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

//External RAM will reside between 8000h - FFFFh.
//There will be 2 wait states for both read and write.
void init_ext_ram(void){
	DDRA=0x00;
	DDRB=0xFF;
	DDRC=0xFF;
	DDRG=0xFF;
	PORTA=0x00;
	PORTB=0x00;
	PORTC=0x00;
	PORTG=0x00;
	MCUCR=0x80;
	XMCRA=0x42;
	XMCRB=0x80;
}

//initialize the LCD to receive data
void lcd_init(void){
	int i;
	//initalize the LCD to receive data
	_delay_ms(15);   
	for(i=0; i<=2; i++){ //do funky initalize sequence 3 times
		SPDR = 0x00;
		while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
		SPDR = 0x30;
		while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
		strobe_lcd();
		_delay_us(37);
	}

	SPDR = 0x00;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0x38;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd();
	_delay_us(37);

	SPDR = 0x00;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0x08;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd();
	_delay_us(37);

	SPDR = 0x00;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0x01;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd();
	_delay_ms(1.6);

	SPDR = 0x00;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0x06;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd();
	_delay_us(37);

	SPDR = 0x00;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	SPDR = 0x0E;
	while (!(SPSR & 0x80)) {}	// Wait for SPI transfer to complete
	strobe_lcd();
	_delay_us(37);
}

// Takes a character and sends it to USART0
void uart_putc(char data) {
    while (!(UCSR0A&(1<<UDRE)));    // Wait for previous transmissions
    UDR0 = data;    // Send data byte
    while (!(UCSR0A&(1<<UDRE)));    // Wait for previous transmissions
}

// Modified to not block indefinantly in the case of a lost byte
char uart_getc(void) {
    uint16_t timer = 0;
    while (!(UCSR0A & (1<<RXC0))) {
	timer++;
        if(timer >= 16000)
            return 0;
    }  // Wait for byte to arrive
    return UDR0;
}

// Takes a string and sends each charater to be sent to USART0
void uart_puts(unsigned char *str) {
    int i = 0;
    while(str[i] != '\0') { // Loop through string, sending each character
	uart_putc(str[i]);
	i++;
    }
}

int main (void){

	uint16_t i;
	unsigned char ir_flag = 0, ir_grounded_flag = 0, uart_flag = 0, ram_flag = 0;
	uint8_t kill = 1, ir_bit;
	volatile uint8_t *p;


	DDRA=0xFF;
	PORTA=0x00;
	DDRB=0xFF;
	PORTB=0x00;
	DDRC=0xFF;
	PORTC=0x00;
	DDRD=0x00;
	PORTD=0x00;
	DDRE=0xFF;
	PORTE=0x00;
	DDRF=0xFF;
	PORTF=0x00;
	DDRG=0xFF;
	PORTG=0x00;
	/*
	   After this point in the tests the LCD is initialized so
	   that it can be used to report success or failure of tests.
	 */
	//initialize the SPI port and then the LCD
	spi_init();
	lcd_init();
	clear_display();

	// IR grounded test
	if (!(PIND & (1<<PD2))){
		ir_grounded_flag = 1;  // possible IR receiver failure
	}   

	/********** pushbutton connectivity test **********/
	/*
	   As each pushbutton is pushed, the corresponding PORTB
	   LED should light.  Multiple button pushes work also.
	   Jumper J12 must be removed so that portD bit 2 can
	   go high.  If J12 is left on, the IR detector probably
	   will see IR energy and pull portD bit 3 low. To terminate
	   this test, push S2 and S9 (bit 0 and 7) simultaneously.
	 */

	string2lcd(switch_test_start);  //indicate start of test

	//turn off other unused ports for safety sake
	SPCR=0x00;//kill spi so that portB is free
	DDRA=0x00;
	DDRC=0x00;
	DDRE=0x00;
	DDRF=0x08;
	DDRG=0x00;

	PORTD = 0x00;
	DDRD = 0x00;  //PORTD all inputs
	DDRB = 0xFF;  //PORTB all outputs 


	while (PORTB != 0xFF){
		PORTB = PORTB | (PIND ^ 0xFF); //invert all bits of button inputs- active low
	}             

	PORTB = 0x00;  //clear port B LEDs

	//reinitialize the SPI port
	spi_init();
	lcd_init();
	clear_display();
	/********** end of pushbutton connectivity test **********/

	/********** UART0 Test **********/
	/*Echo characters back from DB9 connectors using PC as terminal.
	  Communication Parameters: 8 Data, 1 Stop, No Parity
	  Rcvr On, Xmtr On, Asynch Mode, Baud rate: 9600
	  Jumpers J15 and J19 must be in place for the MAX232 chip
	  to get data.  However nothing is usually connected to its
	  serial outputs for this UART port.  A better test will be
	  to send data to itself using the IR channel.             */

	//initialize UART0

	PORTE=0x00;
	DDRE=0xFF;
	UCSR0A=0x00;
	UCSR0B=0x18;
	UCSR0C=0x06;
	UBRR0H=0x00;
	UBRR0L=0x67;

	clear_display();
	string2lcd(uart_que);
	home_line2();
	string2lcd(select);
	
	DDRD=0x00;
	PORTD=0x00;
	
	for (i=0 ; i < 50 ; i++) { // 0.5 second delay
		_delay_ms(10);
	}

	while (kill)
	{
		if (!(PIND & (1<<PD0)))
		{
			kill=0;
		}
		else if (!(PIND & (1<<PD7)))
		{
uart_test:
			kill=0;
			clear_display();
			string2lcd(uart);
			string2lcd(fail);
			home_line2();
			string2lcd(uart_jumpers);
			for (i=0; i<=strlen(uart_test)-1; i++) 
			{
				uart_putc(uart_test[i]);  //send char by char to uart
				if(uart_test[i] != uart_getc())      //uart error
					uart_flag = 1;
			}
			if (!uart_flag)
			{
				clear_display();
				string2lcd(uart);
				string2lcd(pass);
			}
			else
			{
				clear_display();
				string2lcd(uart);
				string2lcd(fail);
				for (i=0 ; i < 500 ; i++) { // 5 second delay
					_delay_ms(10);
				}

				clear_display();
				string2lcd(retest);
				home_line2();
				string2lcd(select);
				while (1)
				{
					if (!(PIND & (1 << PD0)))
					{
						kill=0;
						break;
					}
					else if (!(PIND & (1 << PD7)))
					{
						uart_flag = 0;
						goto uart_test;
					}
				}
			}
			for (i=0 ; i < 500 ; i++) { // 5 second delay
				_delay_ms(10);
			}
		}
	}

	/********** end of UART0 Test **********/



	/********** IR LED and IR receiver test **********/
	/*
	   This test turns on the IR led and then senses through
	   the IR receiver if it is on.  If so, it turns on the
	   PORTB LED bit 0.  Jumpers J12 and  and J13 must be
	   installed.
	 */

	clear_display();
	string2lcd(ir_que);
	home_line2();
	string2lcd(select);

	for (i=0 ; i < 50 ; i++) { // 0.5 second delay
		_delay_ms(10);
	}

	DDRD=0x08;
	PORTD=0xff;

	kill = 1;
	while (kill)
	{
		temp = 0x55;	// bit pattern "01010101"
		if (!(PIND & (1<<PD0)))
		{
			kill=0;
		}
		else if (!(PIND & (1<<PD7)))
		{
			if(ir_grounded_flag) {
				clear_display();
				string2lcd(ir_gnd);
				string2lcd(fail);
				home_line2();
				string2lcd(no_gnd);
				for (i=0 ; i < 500 ; i++) { // 5 second delay
					_delay_ms(10);
				}
				goto ir_end;
			}

			for (i=0; i<=8; i++){
				if (temp & 0x80) {
					ir_bit = 1<<PD2;
					PORTD &= 0xf7;     //turn on IR Transmitter LED - active low assertion
				}
				else {
					ir_bit = 0;
					PORTD |= 0x08;
				}
				_delay_ms(10);      //delay for receiver to sense IR
				if ((PIND & (1<<PD2)) == ir_bit)
					ir_flag = 1;

				temp = temp << 1;
			}
			if (ir_flag)
			{
				clear_display();
				string2lcd(ir);
				string2lcd(fail);
				for (i=0 ; i < 300 ; i++) { // 3 second delay
					_delay_ms(10);
				}
				clear_display();
				string2lcd(ir_jumpers);
				for (i=0 ; i < 300 ; i++) { // 3 second delay
					_delay_ms(10);
				}
				clear_display();
				string2lcd(retest);
				home_line2();
				string2lcd(select);
				while (1)
				{
					if (!(PIND & (1 << PD0)))
					{
						kill=0;
						break;
					}
					if (!(PIND & (1 << PD7)))
					{
						ir_flag = 0;
						break;
					}
				}
			}
			else
			{
				clear_display();
				string2lcd(ir);
				string2lcd(pass);
				kill=0;
				for (i=0 ; i < 500 ; i++) { // 5 second delay
					_delay_ms(10);
				}
			}
		}


	}
ir_end:
	/********** end of IR LED and IR receiver test **********/


	/********** RAM Test **********/
	/*
	   This test writes a 0x00,0xA5,0x5A,0x00 pattern to each external
	   RAM cell and does a readback to determine if the write and read
	   was successful. The top two bits of PORTB LEDS will be strobing
	   very quickly while the test runs.  If the test fails, PORTB LEDS
	   will flash 1 sec on, 1 sec off.  If the test concludes successfully,
	   at least the top four PORTB LEDS will go out.
	 */

	kill = 1;
	while (kill)
	{

		init_ext_ram();  //initialize the external RAM for use
		for (i=0; i<=32767; i++){   //check all RAM cells

			p = (volatile uint8_t *)(0x8000 + i);
			*p = 0x00;  //initalize location to 0x00    
			if (0x00 != *p) ram_flag = 'N'; //make sure values are identical

			*p = 0xA5;  //write the location with 0xA5 
			if (0xA5 != *p) ram_flag = 'N';

			*p = 0x5A;  //initalize location to 0x5A
			if (0x5A != *p) ram_flag = 'N'; 

			*p = 0x00;  //initalize location to 0x00
			if (0x00 != *p) ram_flag = 'N';

			PORTB++;  //twiddle the PORTB LEDS
		} 
		/********** end of RAM Test **********/

		spi_init();
		lcd_init();

		if (ram_flag)
		{
			clear_display();
			string2lcd(ram);
			string2lcd(fail);
			for (i=0 ; i < 500 ; i++) { // 5 second delay
				_delay_ms(10);
			}
			clear_display();
			string2lcd(ram_jumpers);
			for (i=0 ; i < 500 ; i++) { // 5 second delay
				_delay_ms(10);
			}
			clear_display();
			string2lcd(retest);
			home_line2();
			string2lcd(select);
			while (1)
			{
				if (!(PIND & (1 << PD0)))
				{
					kill=0;
					break;
				}
				if (!(PIND & (1 << PD7)))
				{
					ram_flag = 0;
					break;
				}
			}
		}
		else
		{
			clear_display();
			string2lcd(ram);
			string2lcd(pass);
			kill=0;
			for (i=0 ; i < 300 ; i++) { // 3 second delay
				_delay_ms(10);
			}
		}

	}




	// Test the LEDs L1-L8
	clear_display();
	string2lcd(complete);
	SPCR=0x00;	//kill spi so that portB is free	
	DDRB = 0xff;
	PORTB=0xff;	//turn all PORTB LEDs on
	while(1){}


}//main
