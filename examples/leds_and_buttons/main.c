/*
 * leds and button example
 *
 * @author      Attilio Dona' <@attiliodona>
 *
 *
 */
#include <stdio.h>

#include "periph/gpio.h"
#include "periph/timer.h"


void green_pulse(int channel) {
	//printf("green timer fired\n");

	gpio_toggle(GREEN_LED);

	// rescheduling needed because timer API does not have PERIODIC vs ONE_SHOT timers
	timer_set(TIMER_1, 0, SEC_TO_TICKS(2));
}

void yellow_pulse(int channel) {
	printf("yellow timer\n");

	gpio_toggle(YELLOW_LED);

	// rescheduling needed because timer API does not have PERIODIC vs ONE_SHOT timers
	timer_set(TIMER_3, 0, SEC_TO_TICKS(4));
}

void bounce_filter(int channel) {
	int val;

	printf("debounce timer\n");

	val = gpio_read(PUSH2);

	if (val == HIGH) {
		gpio_set(RED_LED);
	} else {
		gpio_clear(RED_LED);
	}

}

void sw2_event(void* arg) {
	printf("button push\n");
	timer_set(TIMER_2, 0, MSEC_TO_TICKS(10));
}

int main(void) {
	
//	gpio_init(RED_LED, GPIO_DIR_OUT, GPIO_NOPULL);
//	gpio_init(GREEN_LED, GPIO_DIR_OUT, GPIO_NOPULL);
//	gpio_init(YELLOW_LED, GPIO_DIR_OUT, GPIO_NOPULL);

	gpio_init_int(PUSH2, GPIO_NOPULL, GPIO_BOTH, sw2_event, 0);

	timer_init(TIMER_1, 0, green_pulse);
	timer_init(TIMER_3, 0, yellow_pulse);

	timer_init(TIMER_2, 0, bounce_filter);

	timer_set(TIMER_1, 0, SEC_TO_TICKS(2));
	timer_set(TIMER_3, 0, SEC_TO_TICKS(4));

	return 0;
}
