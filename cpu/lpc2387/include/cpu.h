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

#ifndef __CPU_H
#define __CPU_H

/**
 * @defgroup	lpc2387		NXP LPC2387
 * @ingroup		cpu
 * @{
 */

#include <stdbool.h>
#include "lpc2387.h"
#include "arm_cpu.h"

extern uintptr_t __stack_start;		///< end of user stack memory space

void lpc2387_pclk_scale(uint32_t source, uint32_t target, uint32_t* pclksel, uint32_t* prescale);
bool install_irq( int IntNumber, void *HandlerAddr, int Priority );

/** @} */
#endif /* __CPU_H */
