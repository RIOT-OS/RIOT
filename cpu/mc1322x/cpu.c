/*
 * cpu.c - MC1322X architecture common support functions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
