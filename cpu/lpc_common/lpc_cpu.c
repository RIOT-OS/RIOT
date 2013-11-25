/*
 * lpc_cpu.c - LPC architecture common support functions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 *
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
