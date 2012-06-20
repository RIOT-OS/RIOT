/*
 * testThread.h
 *
 *  Created on: 01.06.2012
 *      Author: julsch
 */

#ifndef TESTTHREAD_H_
#define TESTTHREAD_H_


char second_thread_stack[8192];
char third_thread_stack[8192];


/**
 * Testing thread functions
 * Thread exmaple only works, if the content of the body is included in the
 * main. In testThreadFunction the example does not work.
 */
void testThreadFunctions(void);

/**
 * howto-exmaple to create a thread in ukleos. The new thread runs the first time after the caller function returns or the procedure ends.
 * The caller function has to declared the stack.
 */
int createThread(void (*threadFunction)(void), char *stack, int stacksize);

/**
 * Thread execution function. The function print the whole ABC on serial and redo this on an infinite loop
 */
void runThread(void);

/**
 * Second thread execution function. The function prints numbers between 1 and 90 in an infinite loop.
 */
void runThread2(void);




#endif /* TESTTHREAD_H_ */
