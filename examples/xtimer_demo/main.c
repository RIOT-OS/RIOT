/*
 * xtimer_demo
 *
 * @author      Attilio Dona' <@attiliodona>
 *
 *
 */
#include <stdio.h>

#include "periph/gpio.h"
#include "xtimer.h"

#include "msg.h"
#include "thread.h"

#define GREEN 1
#define YELLOW 2

kernel_pid_t tid;


void green_pulse(void* arg) {
	msg_t m;
	m.type = GREEN;
	msg_send(&m, tid);
}

void yellow_pulse(void* arg) {
	msg_t m;
	m.type = YELLOW;
	msg_send(&m, tid);
}



int main(void) {
	msg_t m;
	xtimer_t green_tim, yellow_tim;

	tid = thread_getpid();

	green_tim.callback = green_pulse;
	yellow_tim.callback = yellow_pulse;

	gpio_init(DEBUG_PIN, GPIO_DIR_OUT, GPIO_NOPULL);
	//gpio_write(DEBUG_PIN, HIGH);

	xtimer_set(&green_tim, MSEC_TO_TICKS(100));
	xtimer_set(&yellow_tim, MSEC_TO_TICKS(1));

	while(1) {
		msg_receive(&m);
		switch (m.type) {
		case GREEN:
			LED_GREEN_TOGGLE;
			xtimer_set(&green_tim, MSEC_TO_TICKS(100));
			break;
		case YELLOW:
			gpio_toggle(DEBUG_PIN);
			LED_YELLOW_TOGGLE;
			xtimer_set(&yellow_tim, MSEC_TO_TICKS(200));
			break;
		}

	}

	return 0;
}
