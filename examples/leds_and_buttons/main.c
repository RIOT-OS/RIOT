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

#include "driverlib/utils.h"


#if 0
void __attribute__((naked)) hwtimer_spin(unsigned long ticks) {
    __asm("    subs    r0, #1\n"
          "    bne     hwtimer_spin\n"
          "    bx      lr");
}
#endif

void green_pulse(void* arg) {

	LED_GREEN_TOGGLE;

	// rescheduling needed because timer API does not have PERIODIC vs ONE_SHOT timers
	hwtimer_set(MSEC_TO_TICKS(100), green_pulse, NULL);
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
	//int i=4;
	int start, end;

	//gpio_init_int(PUSH2, GPIO_NOPULL, GPIO_BOTH, sw2_event, 0);
	gpio_init(DEBUG_PIN, GPIO_DIR_OUT, GPIO_NOPULL);
	//gpio_write(DEBUG_PIN, HIGH);

	start = HWREG(0x40033048);
//	while(i--) {
//		HWREG(GPIOA2_BASE + 0b100000000) = 0b0000000;
		HWREG(GPIOA2_BASE + 0b100000000) = 0b1000000;
		hwtimer_wait(2);
		//hwtimer_wait(USEC_TO_TICKS(10));
		HWREG(GPIOA2_BASE + 0b100000000) = 0b0000000;
		//gpio_write(DEBUG_PIN, LOW);
//	}
	end = HWREG(0x40033048);
	printf("delta: %d\n", end-start);
	hwtimer_set(MSEC_TO_TICKS(100), green_pulse, NULL);
	hwtimer_set(SEC_TO_TICKS(1), yellow_pulse, NULL);

	return 0;
}
