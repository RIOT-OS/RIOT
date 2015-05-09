/*
 * Copyright (C) 2014 Freie Universitaet Berlin (FUB) and INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  core_util
 * @{
 *
 * @file        panic.c
 * @brief       Crash handling functions implementation for 'native' port
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "panic.h"
#include "native_internal.h"

/* "public" variables holding the crash data (look for them in your debugger) */
char panic_str[80];
int panic_code;

/* flag preventing "recursive crash printing loop" */
static int crashed = 0;

NORETURN void core_panic(int crash_code, const char *message)
{
    if (crashed == 0) {
        crashed = 1;
        /* copy the crash data to "long-lived" global variables */
        panic_code = crash_code;
        strncpy(panic_str, message, 80);
        /* try to print panic message to console (if possible) */
        puts("******** SYSTEM FAILURE ********\n");
        puts(message);
#if DEVELHELP
        puts("******** RIOT HALTS HERE ********\n");
#else
        puts("******** RIOT WILL REBOOT ********\n");
#endif
        puts("\n\n");
    }

    dINT();
#if DEVELHELP
    /* since we're atop an Unix-like platform,
       just use the (developer-)friendly core-dump feature */
    kill(_native_pid, SIGTRAP);
#else
    (void) reboot(RB_AUTOBOOT);
#endif

    /* tell the compiler that we won't return from this function
       (even if we actually won't even get here...) */
    UNREACHABLE();
}
