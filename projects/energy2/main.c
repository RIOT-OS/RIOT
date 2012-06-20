#include <stdio.h>
#include <hwtimer.h>
#include <swtimer.h>
#include <board.h>


//#include "testThread.h"

// init MSB
void initMSBModules(void);

// led functions
//void toggleLedNTimes(int times, long int toggleSpeed);

// thread functions
//void testThreadFunctions(void);
//int createThread(void (*threadFunction)(char *), char *stack, int stacksize);
//void runThread(void);
//void runThread2(void);


// global variables
int thread1 = -1;
int thread2 = -1;
int pid = -1;

//char stack[2048];

/**
 * This is the hello-world main file. Here are the first steps for testing and learning c programming.
 * @return the program status
 */
int main(void) {
    //BEGIN: 	ORGINAL
	//printf("Hello world!\n");
	//END: 		ORGINAL

	//printf("main loop\n");
	initMSBModules();

	// --- testing ---
	//testThreadFunctions();
	//testSHT11Functions();
	//startShell();
	testLTCFunctions();

	printf("main loop\n");
	while(1) {
		toggleLedNTimes(2,62500LU);
		//printf("main loop\n");
	}

    return 0;
}

/******************************************************************************
 * Initialze MSB modules
 *****************************************************************************/

/**
 * Initiliaze modules of the MSB for the hello-world program.
 */
void initMSBModules(void) {
	printf("init MSB modules\n");
	// do nothing because auto_init initiate all init method without parameters

	//hwtimer_init();
	//swtimer_init();
	//sht11_init();
}
