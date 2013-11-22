/*
 * stm32f415rg-lpm.c
 *
 *  Created on: 09.08.2012
 *      Author: pfeiffer
 */
#include <stdio.h>
#include <stdint.h>
#include "lpm.h"

/* lpm is accessed before memory init and initialized separately through code */
__attribute__((section(".noinit")))
static enum lpm_mode lpm;

// FIXME IMPL SLEEP MODES
enum lpm_mode lpm_set(enum lpm_mode target) {
//	unsigned target_flags;
	enum lpm_mode last_lpm = lpm;

	if( target == LPM_IDLE )
	{

	}
	/* calculate target mcu power mode */

//	if( target == LPM_IDLE )
//		target_flags = PM_IDLE;
//	else if( target == LPM_SLEEP )
//		target_flags = PM_SLEEP;
//	else if( target == LPM_POWERDOWN )
//		target_flags = PM_POWERDOWN;
//	else
//		target_flags = 0;
//
//
//	if(target > LPM_IDLE){
//		// Disconnect PLL
//
//		PLLCON &= ~0x0002;
//		PLLFEED = 0xAA;
//		PLLFEED = 0x55;
//		while (PLLSTAT & BIT25);
//		// Disabling PLL not mandatory, is done by hardware
//	}

	lpm = target;

//	PCON |= target_flags;										// set target power mode
	return last_lpm;
}

void lpm_awake(void) {
#if LPM_DEBUG
	unsigned long usec = RTC_CTC;
#endif
//	if( (lpm >= LPM_SLEEP) | ( ~PLLSTAT & (BIT24 | BIT25 | BIT25) )) {	// wake up from deep sleep & detect loss of PLL connection
//		//benchmark
//		init_clks1();
//		init_clks2();
//	    // Debug tests
//#if LPM_DEBUG
//		usec = RTC_CTC-usec;
//	    printf("Wakeup in %lu usecs\n",usec * 31);
//#endif
//	}
	lpm = LPM_ON;
}
