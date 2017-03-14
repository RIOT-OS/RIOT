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

#include "capacity.h"
#include "capacity_settings.h"
#include "ac.h"

uint16_t *my_saved_values;
uint8_t count;

void __comparator_hit(void* arg, uint8_t dev) {
	if(dev == MY_AC)
		gpio_toggle(capacity_conf.result_pin);
}


void capacity_init(void)
{
	/* Set Data direction */
	gpio_init(capacity_conf.result_pin, GPIO_OUT);
	gpio_init(capacity_conf.pxy_pin, GPIO_OUT);

	/*set up analog comparator */
	ac_isr_ctx_t my_isr = {&__comparator_hit, NULL};
	ac_init(MY_AC, AC_INPUT_CAPTURE, AC_IRQ_TOGGLE, my_isr);

	/*TODO: setup timer */
}

uint8_t start_measuring(uint8_t cycle, capacity_result_t *my_result)
{
	my_saved_values = (uint16_t*) malloc(size_of(uint16_t)*cycle);
	count=0;

	/*TODO:start timer and ac */

	gpio_set(capacity_conf.pxy_pin);
	gpio_set(capacity_conf.result_pin);
	while(count<cycle);
	uint32_t average=0;
	for(uint8_t i=0; i<cycle; i++){
		average += my_saved_values[i];
	}
	free(my_saved_values);
	average = average/cycle;
	my_result->timestamp = average;
	my_result->frequency = F_CPU/(float)average;
	my_result->capacity = 1000000000/(1.386*100000*my_result->frequency);
	return 0;
}
