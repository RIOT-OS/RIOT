/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Hwtimer test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "hwtimer.h"
#include "thread.h"

#define BASE_DELAY (1000UL * 1000UL)
#define DELTA_DELAY (10UL * 1000UL)
#define MSGLEN 12 // == strlen("callback %2i")
char msg[MSGLEN * HWTIMER_MAXTIMERS]; // == [callback  1\0callback  2\0...]

void callback(void *ptr)
{
    puts((char *) ptr);
}

int main(void)
{
    puts("hwtimer test application...");

    puts("");
    puts("  Timers should print \"callback x\" once when they run out.");
    printf("  The order for x is 1, n-1, n-2, ..., 2 where n is the number of available hardware timers (%u on this platform).\n", HWTIMER_MAXTIMERS);
    puts("  In 1 second, one timer should fire every second/100 until all timers have run out.");
    puts("  Additionally the message \"hwtimer set.\" should be printed once 1 second from now.");
    puts("");
    puts("Setting timers:");
    puts("");

    unsigned long delay = BASE_DELAY + ((HWTIMER_MAXTIMERS - 1) * DELTA_DELAY);

    /* make the first timer first to fire so timers do not run out linearly */
    char *msgn = msg;
    snprintf(msgn, MSGLEN, "callback %2x", 1);
    hwtimer_set(HWTIMER_TICKS(BASE_DELAY), callback, (void *) msgn);
    printf("set %s\n", msgn);

    /* set up to HWTIMER_MAXTIMERS-1 because hwtimer_wait below also
     * needs a timer */
    for (int i = 1; i < (HWTIMER_MAXTIMERS - 1); i++) {
        msgn = msg + (i * MSGLEN);
        delay -= DELTA_DELAY;
        snprintf(msgn, MSGLEN, "callback %2x", i + 1);
        hwtimer_set(HWTIMER_TICKS(delay), callback, (void *) msgn);
        printf("set %s\n", msgn);
    }

    puts("");
    puts("All timers set.");
    puts("");

    hwtimer_wait(HWTIMER_TICKS(BASE_DELAY));

    puts("hwtimer set.");
    thread_sleep();
    return 0;
}
