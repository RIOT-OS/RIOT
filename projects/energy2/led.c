/*
 * led.c
 *
 *  Created on: 30.05.2012
 *      Author: julsch
 */

#include "led.h"

#ifndef SWTIMER_H_
#include <swtimer.h>
#endif

#ifndef BOARD_H_
#include <board.h>
#endif


char led_stack[MINIMUM_STACK_SIZE+1024];
int led_pid;
static int timesInThread;
static long int toggleSpeedInThread;


/******************************************************************************
 * LED FUNCTIONS
 *****************************************************************************/

/**
 * Toggle the LED n times.
 */
void toggleLedNTimes(int times, long int toggleSpeed) {
	LED_RED_ON;

	for (int var = 1; var < times; var++) {
		//hwtimer_wait(toggleSpeed);
		swtimer_usleep(toggleSpeed);
		LED_RED_TOGGLE;
	}
}

/*void toggleLedInThread(void) {
	toggleLedNTimes(timesInThread, toggleSpeedInThread);
}

void toggleLedNTimesInThread(int times, long int toggleSpeed) {
	timesInThread = times;
	toggleSpeedInThread = toggleSpeed;
	led_pid = thread_create(led_stack,sizeof(led_stack),PRIORITY_MIN,CREATE_STACKTEST,toggleLedInThread,"toggle led");
}*/


