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

unsigned int disableIRQ(void)
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if (state) {
        dINT();
    }

    return state;
}

unsigned int enableIRQ(void)
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if (!state) {
        eINT();
    }

    return state;
}

void restoreIRQ(unsigned int state)
{
    if (state) {
        eINT();
    }
}
