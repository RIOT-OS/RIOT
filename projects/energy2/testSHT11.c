/*
 * testSHT11.c
 * Testing sht11 funstions
 *
 *  Created on: 01.06.2012
 *      Author: julsch
 */

#include "testSHT11.h"

#ifndef THREAD_H_
#include <thread.h>
#endif

#ifndef LED_H_
#include "led.h"
#endif

// interprocess communication, stack and pid for thread
static sht11_mode_t printingMode;
char stack[8196];
int pid;

/**
 * Testing SHT11 functions.
 */
void testSHT11Functions(void) {
	//printf("resut of 'or': %d\n",HUMIDITY | TEMPERATURE);

	// simple measurement test
	/*while(1) {
		//printMeasurement(HUMIDITY | TEMPERATURE);
		printMeasurement(TEMPERATURE);
		swtimer_usleep(1000000LU);
	}*/

	// measurement test in thread

	printingMode = HUMIDITY;
	printf("create SHT11 thread\n");
	pid = thread_create(stack, sizeof(stack), 0, STATUS_SLEEPING, getMeasurementInThread, "nr2");

	int i=0;
	while (1) {
		i++;

		if (i==2) {
			printingMode = HUMIDITY;
		} else if (i == 5) {
			printingMode = HUMIDITY | TEMPERATURE;
		} else {
			printingMode = TEMPERATURE;
		}
		swtimer_usleep(2000000LU);
		thread_wakeup(pid);

		i = i % 5;

	}
}

void getMeasurementInThread(void) {
	//thread_print_stack();
	while(1) {
		toggleLedNTimes(10,62500LU);
		printf("Reading measurements with type %d from SHT11\n", printingMode);
		printMeasurement(printingMode);
		thread_sleep();
	}
}

/**
 * Depending on parameter sht11_mode_t the procedure prints rel. humidity,
 * temperature and/or temperature compensated relative humidity.
 * @param sht11_mode_t is value HUMIDITY or/and TEMPERATURE
 */
void printMeasurement(sht11_mode_t mode) {
	sht11_val_t sht11_val;
	uint8_t success = 0;
	success = sht11_read_sensor(&sht11_val, mode);
	if (!success) {
		printf("Error while reading humidity");
	} else {
		toggleLedNTimes(2,50000LU);
		switch (mode) {
			case HUMIDITY:
				printf("%5.2f%% relative humidity\n", sht11_val.relhum);
				break;
			case TEMPERATURE:
				printf("%5.2f%°C temperature\n", sht11_val.temperature);
				break;
			case HUMIDITY | TEMPERATURE:
				printf("%5.2f%°C temperature,\n", sht11_val.temperature);
				printf("%5.2f%% relative humidity,\n", sht11_val.relhum);
				printf("%5.2f%% temperature compensated relative humidity.\n", sht11_val.relhum_temp);
				break;
			default:
				break;
		}
	}
}
