/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Periodic timer test application
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "board.h"
#include "test_utils/expect.h"

#include "mutex.h"
#include "periph/timer.h"
#include "xtimer.h"

/* We use the timer used for xtimer with the frequency used by xtimer here
 * to make sure we have a known valid timer configuration.
 *
 * DO NOT USE any low-level timer functions demonstrated here when xtimer
 * is used with that timer!
 * Configure a separate timer, XTIMER_DEV is usually 'owned' by xtimer, but
 * as xtimer is not used in this test, we can use it and the fact that every board
 * provides a configuration for it.
 */
#define TIMER_CYCL  XTIMER_DEV
#define CYCLE_MS    100UL
#define CYCLES_MAX   10

static unsigned count[TIMER_CHANNELS];

static void cb(void *arg, int chan)
{
    unsigned c = count[chan]++;

    printf("[%d] tick\n", chan);

    if (c > CYCLES_MAX) {
        timer_stop(TIMER_CYCL);
        mutex_unlock(arg);
    }
}

static const char* _print_ok(int chan, bool *succeeded)
{
    if (chan == 0 && count[chan] > 0) {
        return "OK";
    }

    if (chan > 0 && count[chan] == 0) {
        return "OK";
    }

    *succeeded = false;
    return "ERROR";
}

int main(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    const unsigned long timer_hz = XTIMER_HZ;
    const unsigned steps = (CYCLE_MS * timer_hz) / 1000;

    printf("\nRunning Timer %d at %lu Hz.\n", TIMER_CYCL, timer_hz);
    printf("One counter cycle is %u ticks or %lu ms\n", steps, CYCLE_MS);
    puts("Will print 'tick' every cycle.\n");

    expect(timer_init(TIMER_CYCL, timer_hz, cb, &lock) == 0);

    puts("TEST START");

    /* Only the first channel should trigger and reset the counter */
    /* If subsequent channels trigger this is an error. */
    timer_set_periodic(TIMER_CYCL, 1, 2 * steps, TIM_FLAG_RESET_ON_SET);
    timer_set_periodic(TIMER_CYCL, 0, steps, TIM_FLAG_RESET_ON_MATCH);

    mutex_lock(&lock);

    puts("\nCycles:");

    bool succeeded = true;
    for (unsigned i = 0; i < TIMER_CHANNELS; ++i) {
        printf("channel %u = %02u\t[%s]\n", i, count[i], _print_ok(i, &succeeded));
    }

    if (succeeded) {
        puts("TEST SUCCEEDED");
    } else {
        puts("TEST FAILED");
    }

    return 0;
}
