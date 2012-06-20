/*
 * testLTC4150.c
 *
 *  Created on: 05.06.2012
 *      Author: julsch
 */

#include "testLTC4150.h"

void testLTCFunctions(void) {
	startCurrentMeasurement();

	int i = 0;
	while(i < 10) {
		printCurrent();
		//printAverageCurrent();
		swtimer_usleep(1000000LU);
		//toggleLedNTimes(2,62500LU);
		//printf("now %lu",hwtimer_arch_now());
		i++;
	}
}

void startCurrentMeasurement(void) {
	printf("starting ltc4150 measuring\n");
	ltc4150_start();
}


void stopCurrentMeasurement(void) {
	printf("stopping ltc4150 measuring\n");
	ltc4150_stop();
}


void resetCurrentMeasurement(void) {
	printf("restarting ltc4150 measuring\n");
	ltc4150_start();
}

double getAverageCurrent(void) {
	return ltc4150_get_avg_mA();
}

double getCurrent(void) {
	return ltc4150_get_current_mA();
}

void printAverageCurrent(void) {
	double avg = ltc4150_get_avg_mA();
	printf("avg %.4f mA\n", avg);
}

void printCurrent(void) {
	double avg = ltc4150_get_current_mA();
	printf("current %.4f mA\n", avg);
}

void printCurrentWithLED(void) {
	LED_RED_ON;
	swtimer_usleep(2000000LU);
	double avg = ltc4150_get_current_mA();
	printf("current %.4f mA\n", avg);
	LED_RED_OFF;
}

void printCurrentWithoutLED(void) {
	LED_RED_OFF;
	swtimer_usleep(2000000LU);
	double avg = ltc4150_get_current_mA();
	printf("current %.4f mA\n", avg);
}



