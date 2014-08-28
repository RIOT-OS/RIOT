/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lpc2387
 * @{
 */

#include "cpu.h"
#include <stdint.h>
#include "VIC.h"

void lpc2387_pclk_scale(uint32_t source, uint32_t target, uint32_t *pclksel, uint32_t *prescale)
{
    uint32_t pclkdiv;
    *prescale = source / target;

    if ((*prescale % 16) == 0) {
        *pclksel = 3;
        pclkdiv = 8;
    }
    else if ((*prescale % 8) == 0) {
        *pclksel = 0;
        pclkdiv = 4;
    }
    else if ((*prescale % 4) == 0) {
        *pclksel = 2;
        pclkdiv = 2;
    }
    else {
        *pclksel = 1;
        pclkdiv = 1;
    }

    *prescale /= pclkdiv;

    if (*prescale % 2) {
        (*prescale)++;
    }
}

void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale)
{
    uint32_t pclksel;

    lpc2387_pclk_scale(source, target, &pclksel, prescale);

    PCLKSEL0 = (PCLKSEL0 & ~(BIT2 | BIT3)) | (pclksel << 2);        // timer 0
    PCLKSEL0 = (PCLKSEL0 & ~(BIT4 | BIT5)) | (pclksel << 4);        // timer 1
    PCLKSEL1 = (PCLKSEL1 & ~(BIT12 | BIT13)) | (pclksel << 12); // timer 2
}

/******************************************************************************
** Function name:       install_irq
**
** Descriptions:        Install interrupt handler
** parameters:          Interrupt number, interrupt handler address,
**                      interrupt priority
** Returned value:      true or false, return false if IntNum is out of range
**
******************************************************************************/
#define VIC_BASE_ADDR   0xFFFFF000

bool install_irq(int IntNumber, void (*HandlerAddr)(void), int Priority)
{
    int *vect_addr;
    int *vect_cntl;

    VICIntEnClr = 1 << IntNumber;   /* Disable Interrupt */

    if (IntNumber >= VIC_SIZE) {
        return (false);
    }
    else {
        /* find first un-assigned VIC address for the handler */
        vect_addr = (int *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + IntNumber * 4);
        vect_cntl = (int *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + IntNumber * 4);
        *vect_addr = (int)HandlerAddr;  /* set interrupt vector */
        *vect_cntl = Priority;
        VICIntEnable = 1 << IntNumber;  /* Enable Interrupt */
        return(true);
    }
}

/** @} */
