/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  core_util
 * @{
 *
 * @file        crash.c
 * @brief       Crash handling functions implementation for LPC1768 MCU
 *              (actually, a copy from 'arm_common'...)
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#include "cpu.h"
#include "lpm.h"
#include "crash.h"

#include <string.h>
#include <stdio.h>

/* "public" variables holding the crash data */
char panic_str[80];
int panic_code;

/* flag preventing "recursive crash printing loop" */
static int crashed = 0;

/* WARNING: this function NEVER returns! */
NORETURN void core_panic(int crash_code, const char *message)
{
    /* copy panic datas to "public" global variables */
    panic_code = crash_code;
    strncpy(panic_str, message, 80);
    /* print panic message to console (if possible) */
    if (crashed == 0) {
        crashed = 1;
        puts("******** SYSTEM FAILURE ********\n");
        puts(message);
#if DEVELHELP
        puts("******** RIOT HALTS HERE ********\n");
#else
        puts("******** RIOT WILL REBOOT ********\n");
#endif
        puts("\n\n");
    }
    /* disable watchdog and all possible sources of interrupts */
    //TODO
    dINT();
#if DEVELHELP
    /* enter infinite loop, into deepest possible sleep mode */
    while (1) {
        lpm_set(LPM_OFF);
    }
#else
    /* DEVELHELP not set => reboot system */
    (void) reboot(RB_AUTOBOOT);
#endif

    /* tell the compiler that we won't return from this function
       (even if we actually won't even get here...) */
    UNREACHABLE();
}
