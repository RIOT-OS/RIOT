/*
 * cpu.c - MC1322X architecture common support functions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *                    Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 *
 */

#include "mc1322x.h"
#include "cpu.h"
#include "lpm.h"
#include "arm_cpu.h"

__attribute__((naked,noreturn)) void arm_reset(void)
{
    dINT();
    CRM->SW_RST = SW_RST_VAL;
    while(1);
}

enum lpm_mode lpm_set(enum lpm_mode target) {
    (void) target;
    return LPM_ON;
}

/******************************************************************************
** Function name:       install_irq
**
** Descriptions:        Install interrupt handler.
**                      A wrapper to register_isr to be consistant to lpc2387
**                      implementation.
** parameters:          Interrupt number, interrupt handler address,
**                      interrupt priority
** Returned value:      true or false, return false if IntNum is out of range
**
******************************************************************************/

bool install_irq(int int_number, void *handler_addr, int priority)
{
    (void) priority;
    register_isr(int_number, handler_addr);
    return (true);
}
