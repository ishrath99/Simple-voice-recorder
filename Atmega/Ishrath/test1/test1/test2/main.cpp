/*
 * test2.cpp
 *
 * Created: 6/7/2021 6:57:03 PM
 * Author : ishra
 */ 
#define	F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int analogRead(){
	ADMUX |= (1<<REFS0)|(1<<ADLAR);   //VCC as reference & left justified ADC(8 bits) & select ADC0 as input
	
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);    //enable ADC & set prescaler to 128 (16000000/128=125kHz)
	
	DDRC &= ~(1<<DDC0);   //select ADC0 as input
	
	ADCSRA = ADCSRA | (1 << ADSC);  //// Start an ADC conversion
	
	// Wait until the ADSC bit has been cleared
	while(ADCSRA & (1 << ADSC));
	
	return ADCH;	
}

void analogWriteConfig(){
	TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);
	
	TCCR0B |= (1<<CS00);
	
	DDRD |= (1<<DDD6);
}

int main(void)
{
    analogWriteConfig();
    while (1) 
    {
		OCR0A= analogRead();
		_delay_us(100);
    }
}

