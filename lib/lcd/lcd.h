#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

/************************
*   LCD Functions
*************************/
void strobe_lcd(void);
void clear_display(void);
void home_line2(void);
void char2lcd(unsigned char a_char);
void string2lcd(unsigned char *lcd_str);
void home_line2(void);
void spi_init(void);
void lcd_init(void);