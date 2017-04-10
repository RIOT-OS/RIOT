/*
 * Copyright 2017, RWTH-Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    drivers_capacity  capacity
 * @ingroup     drivers_sensors
 * @brief       Driver to read a capacity value
 *
 * 				This is using a analog comparator and the input capture feature
 * 				in order to calculate the capacity of a Capacitor.
 * 				For further details on the circuit used check: http://www.ti.com/lit/an/slaa363a/slaa363a.pdf
 * 				The Output of the comparator is feed back using an interrupt and a gpio pin
 * @{
 *
 * @file
 * @brief       Driver to read a capacity value
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#define ENABLE_DEBUG	(1)

#include "capacity.h"
#include "capacity_settings.h"
#include "periph_conf.h"
#include "periph/ac.h"
#include "periph/timer.h"
#include "debug.h"
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile uint16_t my_saved_values[10];
volatile uint8_t count;
volatile uint8_t global_cycle;

typedef struct{
	tim_t timer_dev;
	uint8_t ac_dev;
}capacity_config;

capacity_config config;


void __comparator_hit(void* arg, uint8_t dev) {
	if(dev == MY_AC)
		//gpio_toggle(capacity_conf.result_pin);
		PORTF ^= (1<<PF0);
}

int __input_capture_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
	timer_init(tim, freq, cb, arg);
	/* now also add the input capture function */
	/*TODO: remove hardware dependency in module */
	mega_timer_t *my_timer = NULL;
	switch(tim) {
	case TIMER_DEV(0):
			#ifdef TIMER_0
					my_timer = TIMER_0;
			#endif
			break;
	case TIMER_DEV(1):
			#ifdef TIMER_1
					my_timer = TIMER_1;
					*TIMER_1_MASK |= (1<<ICIE1);
			#endif
			break;
	case TIMER_DEV(2):
			#ifdef TIMER_2
					my_timer = TIMER_2;
					*TIMER_2_MASK |= (1<<ICIE1);
			#endif
			break;
	case TIMER_DEV(3):
			#ifdef TIMER_3
					my_timer = TIMER_3;
					*TIMER_3_MASK |= (1<<ICIE1);
			#endif
			break;
	}
	my_timer->CRB |= (1<<ICES1);
	DEBUG("Timer Config %u \n", TCCR1B);

	return 0;
}


void capacity_init(uint8_t timer_dev, uint8_t ac_dev)
{
	config.timer_dev = timer_dev;
	config.ac_dev = ac_dev;
	/* Set Data direction */
	gpio_init(capacity_conf.result_pin, GPIO_OUT);
	gpio_init(capacity_conf.pxy_pin, GPIO_OUT);

	/*set up analog comparator */
	ac_isr_ctx_t my_isr = {&__comparator_hit, NULL};
	ac_init(ac_dev, AC_INPUT_CAPTURE, AC_IRQ_TOGGLE, my_isr);

	__input_capture_init(timer_dev, CLOCK_CORECLOCK, NULL, NULL);
}

uint8_t start_measuring(uint8_t cycle, capacity_result_t *my_result)
{
	//my_saved_values = (uint16_t*) malloc(sizeof(uint16_t)*cycle);
	count=0;
	global_cycle = cycle + 1;

	/*TODO:start timer and ac */
	gpio_set(capacity_conf.pxy_pin);

	DEBUG("starting measurement \n");
	timer_start(config.timer_dev);
	ac_poweron(config.ac_dev);
	gpio_set(capacity_conf.result_pin);
	while(count<global_cycle);
	DEBUG("measurement done\n");
	ac_poweroff(config.ac_dev);
	timer_stop(config.timer_dev);
	uint32_t average=0;
	for(uint8_t i=1; i<global_cycle; i++){
		average += my_saved_values[i]-my_saved_values[i-1];
		DEBUG("%u ", my_saved_values[i]-my_saved_values[i-1]);
	}
	//free(my_saved_values);
	average = average/(global_cycle-1);
	my_result->timestamp = average;
	my_result->frequency = CLOCK_CORECLOCK/(float)average;
	my_result->capacity = 1/(1.386*100000*my_result->frequency);
	return 0;
}

ISR(TIMER1_CAPT_vect)
{
	if(count >=global_cycle) {
			ACSR |=(1<<ACD);
			//ac_poweroff(config.ac_dev);
			//timer_stop(config.timer_dev);
	}
	my_saved_values[count] = ICR1L;
	my_saved_values[count] |= ((uint16_t)ICR1H<<8);
	count++;
}
