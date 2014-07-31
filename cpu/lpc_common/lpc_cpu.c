/*
 * lpc_cpu.c - LPC architecture common support functions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "cpu.h"
#include "irq.h"

__attribute__((naked,noreturn)) void arm_reset(void)
{
    disableIRQ();
    WDTC = 0x0FFFF;
    WDMOD = 0x03;
    WDFEED= 0xAA;
    WDFEED= 0x55;
    while(1);
}
