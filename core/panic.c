/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Crash handling functions
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#include <string.h>
#include <stdio.h>

#include "cpu.h"
#include "irq.h"
#include "lpm.h"
#include "panic.h"
#include "arch/panic_arch.h"

#define PANIC_STR_SIZE 80

/* "public" variables holding the crash data */
char panic_str[PANIC_STR_SIZE];
int panic_code;

/* flag preventing "recursive crash printing loop" */
static int crashed = 0;

/* WARNING: this function NEVER returns! */
NORETURN void core_panic(int crash_code, const char *message)
{
    /* copy panic datas to "public" global variables */
    panic_code = crash_code;
    strncpy(panic_str, message, sizeof(panic_str));
    /* strncpy does not add any null-termination. */
    panic_str[sizeof(panic_str)-1] = '\0';
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
    disableIRQ();
    panic_arch();
#ifndef DEVELHELP
    /* DEVELHELP not set => reboot system */
    (void) reboot(RB_AUTOBOOT);
#endif

    /* tell the compiler that we won't return from this function
       (even if we actually won't even get here...) */
    UNREACHABLE();
}
