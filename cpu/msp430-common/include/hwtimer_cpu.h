/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
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

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#ifdef CC430
	#include <cc430f6137.h>
#else
	#include <msp430x16x.h>
#endif
#include <stdint.h>

#ifdef __MSP430_HAS_TA2__
#define ARCH_MAXTIMERS 2
#endif
#ifdef __MSP430_HAS_TA3__
#define ARCH_MAXTIMERS 3
#endif
#ifdef __MSP430_HAS_T0A5__
#define ARCH_MAXTIMERS 5
#endif


#ifndef ARCH_MAXTIMERS
#warning "ARCH_MAXTIMERS UNSET!"
#define ARCH_MAXTIMERS 0
#endif

#define HWTIMER_SPEED 32768
#define HWTIMER_MAXTICKS (0xFFFFFFFF)

#endif // __HWTIMER_CPU_H
