/*
 * trace.c
 *
 *  Created on: 19.06.2012
 *      Author: julsch
 */
//#include "led.h"

void __attribute__((no_instrument_function)) __cyg_profile_func_enter (void *func,  void *caller) {
 	 //if(fp_trace != NULL) {
 	printf("e %p %p\n", func, caller);
 	 //}
	//toggleLedNTimes(2,62500LU);
}

void __attribute__((no_instrument_function)) __cyg_profile_func_exit (void *func, void *caller) {
	//if(fp_trace != NULL) {
	printf("x %p %p\n", func, caller);
 	//}
	//toggleLedNTimes(2,62500LU);
}


