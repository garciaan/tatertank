#include "./adc.h"


void enable_adc(){
	DDRF = 0x00;
	PORTF = 0x00;
	//internally wire REFSn bits in the ADMUX register to connect VREF 
		//to internal 2.56v reference --> 0.0025 v/step
	//ADMUX = (1 << REFS0) | (1 << REFS1); //2.56v
	ADMUX = (1 << REFS0); //AVCC
	//Set analog input channel by writing to the MUX in ADMUX
	//Single ended input, gain is not allowed
	
	//Default Right Adjusted

	//Set ADEN in ADSCRA to enable ADC
	ADCSRA = (1 << ADEN);
	//Set the ADC Clock frequency prescaler to 128 --> 125kHz
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	//Set free running mode
	//ADCSRA |= (1 << ADFR);
	//Enable the ADC Interrupt Flag
	//ADCSRA |= (1 << ADIE);
	//Start first conversion by setting ADSC in ADCSRA
	ADCSRA |= (1 << ADSC);

}
int read_adc(int pin){
	int data;
	if (pin > 7){
		return -1;
	}
	int i;
	//clear all the mux bits
	for (i = 0; i < 5; ++i){
		ADMUX &= ~(1 << i);
	}
	//set the new mux bit
	ADMUX |= pin;
	ADCSRA |= (1 << ADSC);
	//Wait until ADC registers complete the conversion
	while (!(ADCSRA & (1 << ADIF)));
	data = ADCL;
	data |= (ADCH << 8);
	return data;
}
double get_voltage(int adc){
	if (adc < 0 || adc > 7){
		return 0;
	}
	return (double)VREF/(1024.0) * (double)read_adc(adc);
}


