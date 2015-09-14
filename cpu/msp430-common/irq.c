/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
* @{
 *
 * @file
 * @brief       ISR related functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "irq.h"
#include "cpu.h"

volatile int __inISR = 0;

char __isr_stack[MSP430_ISR_STACK_SIZE];

unsigned int disableIRQ(void)
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if (state) {
        __disable_irq();
    }

    return state;
}

unsigned int enableIRQ(void)
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if (!state) {
        __enable_irq();
    }

    return state;
}

void restoreIRQ(unsigned int state)
{
    if (state) {
        __enable_irq();
    }
}

int inISR(void)
{
    return __inISR;
}
