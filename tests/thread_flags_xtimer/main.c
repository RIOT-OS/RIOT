/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test waking up a thread using thread flags from xtimer callback
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "thread_flags.h"

#define TIMEOUT         (100UL * US_PER_MS)
#define REPEAT          (5)
#define RUNTIME         (TIMEOUT * REPEAT)

static void time_evt(void *arg)
{
    thread_flags_set((thread_t *)arg, 0x1);
}

int main(void)
{

    xtimer_t timer;
    timer.callback = time_evt;
    timer.arg = (void *)sched_active_thread;
    uint32_t last = xtimer_now_usec();

    puts("\nTest setting thread flags from (x)timer callback");
    printf("You should see the message '+++ timeout XX +++' printed to the \n"
           "screen %i times, once every %lu milliseconds\n\n",
            REPEAT, (TIMEOUT / US_PER_MS));

    for (int i = 1; i <= REPEAT; i++) {
        xtimer_set(&timer, TIMEOUT);
        thread_flags_wait_any(0x1);
        printf("+++ timeout %2i +++\n", i);
    }

    /* we consider the test successful, if the runtime was above 500ms */
    uint32_t runtime = xtimer_now_usec() - last;
    if (runtime > RUNTIME) {
        puts("Test finished: SUCCESS\n");
    }
    else {
        puts("Test finished: FAILED\n");
    }

    return 0;
}
