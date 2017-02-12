/*
 * Copyright (C) 2016 RWTH Aachen, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Test Measurement of a capacity
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#define F_CPU 16000000
#include <stdio.h>

#include "board.h"
#include "rgbled.h"
#include "shell.h"
#include <avr/io.h>
#include "debug.h"
#include <util/delay.h>
#include <avr/interrupt.h>
volatile uint8_t count=0;

int main(void)
{

	//first test everything based on reigsters
	//set output pins
	DDRE &= ~((1<<PE2)|(1<<PE3)); //PE2=AIN0 PE3=AIN1
	DDRF |= (1<<PF0)|(1<<PF1); //A0
	DDRD &= ~(1<<PD4);


	TCCR1A = 0x00;
	TCCR1B =0x00;
	TCCR1C = 0x00;
	TCCR1B |= (1<<ICNC1)|(1<<ICES1);

	TIMSK1 |= (1<<ICIE1)|(1<<TOIE0); //enable Input capture in timer

	PRR0 |=(1<<PRADC); //power on analog comperator
	ACSR &= ~((1<<ACIS1)|(1<<ACIS0)); //make sure AC is turned on


	sei();
	puts("starting measurement");
	TCCR1B |= (1<<CS12)|(1<<CS10); //set clock
	ACSR &= ~(1<<ACD);
	ACSR |=(1<<ACIC)|(1<<ACIE);
	PORTF |= (1<<PF1);
	PORTF |= (1<<PF0);
	/*while(1) {
		_delay_ms(100);
		uint8_t measure = ACSR & (1<<ACO);
		printf("%u",measure);
		if(measure != 0)
			PORTF &= ~(1<<PF0);
		else
			PORTF |= (1<<PF0);
	} */
	while(1) {

	}
	return 0;


}
ISR (TIMER1_OVF_vect)
{
	puts("OVERFLOW\n");
}
ISR(ANALOG_COMP_vect)
{
	//puts("comp hit");
	PORTF ^= (1<<PF0);
}

ISR(TIMER1_CAPT_vect)
{
	count++;
	printf("%u",count);
	if(count>=10) {
		PORTF &= ~(1<<PF1);
		TCCR1B &=~((1<<CS10)|(1<<CS11)|(1<<CS12));
		cli();
	}else{
	uint16_t timestamp = ICR1L;
	timestamp = timestamp & (ICR1H<<8);
	//printf("count %u",count);
	printf("Timer %u \n", timestamp);
	TCNT1H = 0;
	TCNT1L = 0;
	}
}
