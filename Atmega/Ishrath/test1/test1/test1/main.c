/*
 * test1.c
 *
 * Created: 5/31/2021 1:30:17 PM
 * Author : ishra
 */ 

#define	F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>




int main(void)
{
    ADMUX |= (1<<REFS0)|(1<<ADLAR);   //VCC as reference & left justified ADC(8 bits) & select ADC0 as input
	
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);    //enable ADC & set prescaler to 128 (8000000/128=62.5kHz)
	
	DDRC &= ~(1<<DDC0);   //select ADC0 as input
	
	//TCCR0A |= (1<<WGM00)|(1<<COM0A1)|(1<<WGM01)|(1<<CS00);    // initialize timer0 in PWM mode
	
	DDRB = 0b11111111;     //set portB as output
	
    while (1) 
    {
		ADCSRA = ADCSRA |= (1 << ADSC);  //// Start an ADC conversion 
		
		// Wait until the ADSC bit has been cleared
		while(ADCSRA & (1 << ADSC));
		
		PORTB = ADCH;
		 
		_delay_us(62.5);
    }
}

