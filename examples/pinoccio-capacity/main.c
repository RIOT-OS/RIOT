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
#include <periph/ac.h>
volatile uint8_t count=0;
volatile uint16_t values[10];

void accb(void* arg) {
	PORTF ^= (1<<PF0);
}

int main(void)
{
	ac_isr_ctx_t my_isr = {&accb, NULL};
	ac_init(AC_0, AC_INPUT_CAPTURE, AC_IRQ_TOGGLE, my_isr);
	ac_poweron(AC_0);

	while(1){
		printf("%u \n",ac_read(AC_0));
		_delay_us(100);
	}
	//first test everything based on reigsters
	//set output pins
	/*DDRE &= ~((1<<PE2)|(1<<PE3)); //PE2=AIN0 PE3=AIN1
	DDRF |= (1<<PF0)|(1<<PF1); //A0
	DDRD &= ~(1<<PD4);


	TCCR1A = 0x00;
	TCCR1B =0x00;
	TCCR1C = 0x00;
	TCCR1B |= (1<<ICES1);

	TIMSK1 |= (1<<ICIE1)|(1<<TOIE0); //enable Input capture in timer

	PRR0 |=(1<<PRADC); //power on analog comperator
	ACSR &= ~((1<<ACIS1)|(1<<ACIS0)); //make sure AC is turned on

	puts("starting measurement");
	TCCR1B |= (1<<CS10); //set clock
	ACSR &= ~(1<<ACD);
	ACSR |=(1<<ACIC)|(1<<ACIE);
	PORTF |= (1<<PF1);
	PORTF |= (1<<PF0);
	sei();
	while(count<10)
	{
		//printf("%u",TCNT1);
		//_delay_ms(100);
	}
	uint8_t i;
	for(i=0; i<10; i++)
	{
		printf(" %u", values[i]);
	}
	while(1) {

	}*/
	return 0;


}
/*ISR (TIMER1_OVF_vect)
{
	puts("OVERFLOW\n");
}
ISR(ANALOG_COMP_vect)
{
	PORTF ^= (1<<PF0);
}

ISR(TIMER1_CAPT_vect)
{
	//TCCR1B ^= (1<<ICES1);
	//count++;
	//printf("%u",count);


	values[count] = ICR1L;
	values[count] |= (ICR1H<<8);
	//printf("count %u",count);
	//printf("Timer %u \n", timestamp);
	//TCNT1H = 0;
	//TCNT1L = 0;
	count++;
	if(count >=10)
		cli();
		TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10)); //set clock
	//}
} */
