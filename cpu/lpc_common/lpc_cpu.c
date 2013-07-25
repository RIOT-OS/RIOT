/*
 * lpc_cpu.c - LPC architecture common support functions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 *
 */

__attribute__((naked,noreturn)) void arm_reset(void)
{
    dINT();
    WDTC = 0x00FFF;
    WDMOD = 0x03;
    WDFEED= 0xAA;
    WDFEED= 0x55;
    while(1);
}
