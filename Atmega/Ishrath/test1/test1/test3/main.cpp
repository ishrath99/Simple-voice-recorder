/*
 * test3.cpp
 *
 * Created: 6/10/2021 6:57:12 PM
 * Author : ishra
 */ 

#define	F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define sbi(port, bit) port|= 1<<bit

void config()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();
	
	// on the ATmega168, timer 0 is also used for fast hardware pwm
	// (using phase-correct PWM would mean that timer 0 overflowed half as often
	// resulting in different millis() behavior on the ATmega8 and ATmega168)
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);
	
	// set timer 0 prescale factor to 64
	// this combination is for the standard 168/328/1280/2560
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
	
	// enable timer 0 overflow interrupt
	sbi(TIMSK0, TOIE0);
	
	// timers 1 and 2 are used for phase-correct hardware pwm
	// this is better for motors as it ensures an even waveform
	// note, however, that fast pwm mode can achieve a frequency of up
	// 8 MHz (with a 16 MHz clock) at 50% duty cycle
	TCCR1B = 0;
	
	// set timer 1 to no prescaling 
	//sbi(TCCR1B, CS11);
	sbi(TCCR1B, CS10);
	
	// put timer 1 in FAST pwm mode, non-inverting mode
	sbi(TCCR1A, WGM10);
	sbi(TCCR1B, WGM12);
	sbi(TCCR1A, COM1A1);
	
	// set timer 2 prescale factor to 64
	sbi(TCCR2B, CS22);
	
	// configure timer 2 for phase correct pwm (8-bit)
	sbi(TCCR2A, WGM20);
	
	// set a2d prescaler to 128
	sbi(ADCSRA, ADPS2);
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);
	
	// enable a2d conversions
	sbi(ADCSRA, ADEN);
	
	//VCC as reference & left justified ADC(8 bits) & select ADC0 as input
	sbi(ADMUX, REFS0);
	sbi(ADMUX, ADLAR);
	
	// the bootloader connects pins 0 and 1 to the USART; disconnect them
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
	
	UCSR0B = 0;
}

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

