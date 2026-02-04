/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "thread.h"
#include "thread_flags.h"

#define TIMEOUT         (100UL * US_PER_MS)
#define REPEAT          (5U)
#define RUNTIME         (TIMEOUT * REPEAT)

static void time_evt(void *arg)
{
    thread_flags_set(arg, 0x1);
}

int main(void)
{
    puts("START");
    xtimer_t timer;
    timer.callback = time_evt;
    timer.arg = thread_get_active();
    uint32_t last = xtimer_now_usec();

    puts("Test setting thread flags from (x)timer callback");
    printf("You should see the message '+++ timeout XX +++' printed to the \n"
           "screen %u times, once every %lu milliseconds\n\n",
            REPEAT, (TIMEOUT / US_PER_MS));

    for (unsigned i = 1; i <= REPEAT; i++) {
        xtimer_set(&timer, TIMEOUT);
        thread_flags_wait_any(0x1);
        printf("+++ timeout %2u +++\n", i);
    }

    /* we consider the test successful, if the runtime was above 500ms */
    uint32_t runtime = xtimer_now_usec() - last;
    if (runtime > RUNTIME) {
        puts("SUCCESS");
    }
    else {
        puts("FAILURE");
    }

    return 0;
}
