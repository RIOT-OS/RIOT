/*
 * Copyright (C) 2018 FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_newlib_syscalls_riot
 * @file
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @brief clock() system call (using xtimer)
 *
 * This module uses xtimer to implement the clock() system call, that is,
 * relative timings. If the functionality is not required, but only that the
 * symbol is present to avoid a linking error, use the stub in the
 * newlib_stub_clock module.
 *
 */

#include <time.h>

#include "xtimer.h"

/**
 * @brief Get the processor time used by the program.
 *
 * This implements the C standard function clock(). The return value must be
 * divided by CLOCKS_PER_SEC to give a value in seconds.
 *
 * To use this function you should include <time.h>
 *
 * @note Since RIOT is a single process (with multiple threads), this function is
 * equivalent to the wall time elapsed since boot. Idle time is also considered
 * as time used by the program.
 *
 * @return  Time used by RIOT (in arbitrary units of "CLOCKS")
 */
clock_t clock(void)
{
    timex_t now;

    /* By using a timex_t we have two 32 bit values and that makes it easier to
     * convert to clock_t without overflow */
    xtimer_now_timex(&now);

    return  ((clock_t)now.seconds) * CLOCKS_PER_SEC
          + ((clock_t)now.microseconds) * CLOCKS_PER_SEC / US_PER_SEC;
}
