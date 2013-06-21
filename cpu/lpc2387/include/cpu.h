/******************************************************************************
Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file subject to the terms and conditions of the GNU Lesser General Public
License. See the file LICENSE in the top level directory for more details.
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

void lpc2387_pclk_scale(uint32_t source, uint32_t target, uint32_t *pclksel, uint32_t *prescale);
bool install_irq(int IntNumber, void *HandlerAddr, int Priority);

/** @} */
#endif /* __CPU_H */
