/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @ingroup lpc2387
 * @{
 */

/**
 * @file
 * @brief		LPC2387 Low-Power management
 * @ingroup		lpc2387
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author 		Heiko Will
 * @version     $Revision$
 *
 * @note    	$Id$
 */

#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "lpc23xx.h"
#include "lpc2387.h"
#include "lpm.h"

/* lpm is accessed before memory init and initialized separately through code */
__attribute__((section(".noinit")))
static enum lpm_mode lpm;

extern void init_clks1(void);
extern void init_clks2(void);

#define ENABLE_DEBUG 0
#include <debug.h>

void lpm_init(void) {
	lpm = LPM_ON;
}

#define LPM_DEBUG 1

void lpm_begin_awake(void) {
	if (lpm >= LPM_SLEEP ) {									// wake up from deep sleep
		init_clks1();
	}
}

void lpm_end_awake(void) {
	if( lpm >= LPM_SLEEP ) {									// wake up from deep sleep
		init_clks2();
	}
	lpm = LPM_ON;
}

void lpm_awake(void) {
#if LPM_DEBUG
	unsigned long usec = RTC_CTC;
#endif
	if( lpm >= LPM_SLEEP ) {									// wake up from deep sleep
		//benchmark
		init_clks1();
		init_clks2();
	    // Debug tests
#if LPM_DEBUG
		usec = RTC_CTC-usec;
	    DEBUG("Wakeup in %lu usecs\n",usec * 31);
#endif
	}
	lpm = LPM_ON;
}

enum lpm_mode lpm_set(enum lpm_mode target) {
	unsigned target_flags;
	enum lpm_mode last_lpm = lpm;

	/* calculate target mcu power mode */
	if( target == LPM_IDLE )
		target_flags = PM_IDLE;
	else if( target == LPM_SLEEP )
		target_flags = PM_SLEEP;
	else if( target == LPM_POWERDOWN )
		target_flags = PM_POWERDOWN;
	else
		target_flags = 0;

	lpm = target;

	#if iENABLE_DEBUG
		DEBUG("# LPM power down %u -> %u", lpm, target);
	#endif

	PCON |= target_flags;										// set target power mode
	return last_lpm;
}
/*---------------------------------------------------------------------------*/
enum lpm_mode
lpm_get(void) {
	return lpm;
}
/*---------------------------------------------------------------------------*/
/** @} */
