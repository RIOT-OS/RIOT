/*
 * leds and button example
 *
 * @author      Attilio Dona' <@attiliodona>
 *
 *
 */
#include <stdio.h>

#include "periph/gpio.h"
#include "hwtimer.h"


void green_pulse(void* arg) {

	LED_GREEN_TOGGLE;

	// rescheduling needed because timer API does not have PERIODIC vs ONE_SHOT timers
	hwtimer_set(MSEC_TO_TICKS(500), green_pulse, NULL);
}

void yellow_pulse(void* arg) {
	//printf("yellow timer\n");

	LED_YELLOW_TOGGLE;

	// rescheduling needed because timer API does not have PERIODIC vs ONE_SHOT timers
	hwtimer_set(SEC_TO_TICKS(1), yellow_pulse, NULL);
}

void bounce_filter(void* arg) {
	int val;

	//printf("debounce timer\n");

	val = gpio_read(PUSH2);

	if (val == HIGH) {
		LED_RED_ON;
	} else {
		LED_RED_OFF;
	}

}

void sw2_event(void* arg) {
	//printf("button push\n");
	hwtimer_set(MSEC_TO_TICKS(10), bounce_filter, NULL);
}

int main(void) {
	
	gpio_init_int(PUSH2, GPIO_NOPULL, GPIO_BOTH, sw2_event, 0);

	hwtimer_set(MSEC_TO_TICKS(500), green_pulse, NULL);
	hwtimer_set(SEC_TO_TICKS(1), yellow_pulse, NULL);

	return 0;
}
