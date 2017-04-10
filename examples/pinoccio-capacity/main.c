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
#include "periph/ac.h"
#include "capacity.h"
#include "periph/timer.h"
#include "capacity_settings.h"
//volatile uint8_t count=0;
//volatile uint16_t values[10];

void accb(void* arg, uint8_t dev) {
	PORTF ^= (1<<PF0);
	printf("hit");
}

int main(void)
{
	capacity_init(TIMER_DEV(2), AC_0);
	capacity_result_t my_result;
	start_measuring(10, &my_result);
	printf("Timestamp %u \n", my_result.timestamp);
	printf("Frequency %u \n", my_result.frequency);
	printf("Capacity %.6f \n", my_result.capacity*1000000000000);

}
	//PRR0 |=(1<<PRADC); //power on analog comperator
/*	ac_isr_ctx_t my_isr = {&accb, NULL};
	ac_init(AC_0, AC_INPUT_CAPTURE, AC_IRQ_TOGGLE, my_isr);
	ac_poweron(AC_0);
	printf("ACSR %u", ACSR);
	*/
/*	//first test everything based on reigsters
	//set output pins
	DDRE &= ~((1<<PE2)|(1<<PE3)); //PE2=AIN0 PE3=AIN1
	DDRF |= (1<<PF0)|(1<<PF1); //A0
	DDRD &= ~(1<<PD4);


	TCCR1A = 0x00;
	TCCR1B =0x00;
	TCCR1C = 0x00;
	TCCR1B |= (1<<ICES1);

	TIMSK1 |= (1<<ICIE1); //enable Input capture in timer

	PRR0 |=(1<<PRADC); //power on analog comperator
	ACSR &= ~((1<<ACIS1)|(1<<ACIS0)); //make sure AC is turned on


	PORTF |= (1<<PF1);
	puts("starting measurement");
	TCCR1B |= (1<<CS10); //set clock
	ACSR |=(1<<ACD)|(1<<ACIC)|(1<<ACIE);
	ACSR &= ~(1<<ACBG);
	printf("ACSR1 %u \n", ACSR);
	ACSR &= ~((1<<ACD)|(1<<ACBG));
	printf("ACSR2 %u \n", ACSR);
	printf("Timer Config %u \n", TCCR1B);
	PORTF |= (1<<PF0);
	sei();
	while(count<10)
	{
		//printf("%u",TCNT1);
		//_delay_ms(100);
	}
	uint8_t i;
	for(i=1; i<10; i++)
	{
		printf(" %u", values[i]-values[(i-1)]);
	}
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
	PORTF ^= (1<<PF0);
}

ISR(TIMER1_CAPT_vect)
{

	values[count] = ICR1L;
	values[count] |= ((uint16_t)ICR1H<<8);
	count++;
	if(count >=10) {
		//cli();
		ACSR |=(1<<ACD);
		//TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10)); //set clock
	}
	//}
}*/

