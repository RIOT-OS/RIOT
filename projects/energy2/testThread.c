/*
 * testThread.c
 * testing thread functions in ukleos
 *  Created on: 01.06.2012
 *      Author: julsch
 */

#include "testThread.h"
#ifndef THREAD_H_
#include <thread.h>
#endif


/**
 * Testing thread functions
 * Thread exmaple only works, if the content of the body is included in the
 * main. In testThreadFunction the example does not work.
 */
void testThreadFunctions(void) {
	printf("init thread\n");
	toggleLedNTimes(10,125000LU);

	int thread1 = -1;
	int thread2 = -1;

	thread1 = createThread(runThread, second_thread_stack, sizeof(second_thread_stack));
	thread2 = createThread(runThread2, third_thread_stack, sizeof(third_thread_stack));
}

/**
 * howto-exmaple to create a thread in ukleos. The new thread runs the first time after the caller function returns or the procedure ends.
 * The caller function has to declared the stack.
 */
int createThread(void (*threadFunction)(void), char *stack, int stacksize) {
	int threadID;

	printf("create thread\n");
	threadID = thread_create(stack, stacksize, 0, CREATE_WOUT_YIELD | CREATE_STACKTEST, threadFunction, "nr2");
	return threadID;
}


/**
 * Thread execution function. The function print the whole ABC on serial and redo this on an infinite loop
 */
void runThread(void) {
	char *value = 'A';
	while(1) {
		swtimer_usleep(500000LU);
		printf("Message %c\n", value);
		if (value == 90) {
			value = 64;
			toggleLedNTimes(10,50000LU);
		}
		value++;
	}
}

/**
 * Second thread execution function. The function prints numbers between 1 and 90 in an infinite loop.
 */
void runThread2(void) {
	char *value = 1;
	while(1) {
		swtimer_usleep(500000LU);
		printf("Message %d\n", value);
		if (value == 90) {
			value = 1;
			toggleLedNTimes(10,50000LU);
		}
		value++;
	}
}


