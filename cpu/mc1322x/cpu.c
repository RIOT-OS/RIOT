/*
 * cpu.c - MC1322X architecture common support functions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 *
 */

#include "mc1322x.h"
#include "lpm.h"

__attribute__((naked,noreturn)) void arm_reset(void)
{
    dINT();
    CRM->SW_RST = SW_RST_VAL;
    while(1);
}

enum lpm_mode lpm_set(enum lpm_mode target) {
    return LPM_ON;
}

/******************************************************************************
** Function name:		cpu_install_irq
**
** Descriptions:		Install interrupt handler
** parameters:			Interrupt number, interrupt handler address,
**						interrupt priority
** Returned value:		true or false, return false if IntNum is out of range
**
******************************************************************************/
#define VIC_BASE_ADDR   0xFFFFF000

bool cpu_install_irq(int IntNumber, void *HandlerAddr, int Priority)
{
    int *vect_addr;
    int *vect_cntl;

    VICIntEnClr = 1 << IntNumber;	/* Disable Interrupt */

    if (IntNumber >= VIC_SIZE) {
        return (false);
    }
    else {
        /* find first un-assigned VIC address for the handler */
        vect_addr = (int *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + IntNumber * 4);
        vect_cntl = (int *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + IntNumber * 4);
        *vect_addr = (int)HandlerAddr;	/* set interrupt vector */
        *vect_cntl = Priority;
        VICIntEnable = 1 << IntNumber;	/* Enable Interrupt */
        return(true);
    }
}
