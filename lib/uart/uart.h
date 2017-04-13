#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

//Maximum size of string expected from buoy
#define MAX_STRING_SIZE 5


#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "ascii_special.h"



/************************
*   BOTH UART FUNCTIONS
*   USART1: Bluetooth Module
*   USART0: RS232 Connection
*************************/
void USART1_Init( unsigned int ubrr );
void USART1_Transmit(unsigned char data );
void USART1_send_string(unsigned char *data);
unsigned char USART1_Receive(void);
void USART1_Receive_String(unsigned char *str);
void USART1_flush();

void USART0_Init( unsigned int ubrr );
void USART0_Transmit(unsigned char data );
void USART0_send_string(unsigned char *data);
unsigned char USART0_Receive(void);
void USART0_Receive_String(unsigned char *str);
void USART0_flush();
void USART0_send_drone(unsigned char *data);
