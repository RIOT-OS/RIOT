/*
 * Copyright (C) 2020 Inria
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
 * @brief       Test to figure out RTT_MIN_OFFSET
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"

#include "xtimer.h"

#define US_PER_TICK     (US_PER_SEC / RTT_FREQUENCY)
/* min. amount of time to wait between set_alarm() */
#define MIN_WAIT_US     (3 * US_PER_TICK)

#ifndef SAMPLES
#define SAMPLES 1024LU
#endif

void cb(void *arg)
{
    mutex_unlock(arg);
}

int main(void)
{
    uint32_t value = 0;
    uint32_t samples = 0;
    /* mutex starts out locked, and each time an rtt callback is successfully
       called it will be locked again for the next iteration */
    mutex_t lock = MUTEX_INIT_LOCKED;

    rtt_init();

    printf("Evaluate RTT_MIN_OFFSET over %" PRIu32 " samples\n",
           (uint32_t)SAMPLES);

    for (unsigned i = 0; i < SAMPLES; i++) {
        uint32_t offset = 0;
        int ret = -1;
        while (ret != 0) {
            offset++;
            rtt_clear_alarm();
            uint32_t now = rtt_get_counter();
            rtt_set_alarm((now + offset) % RTT_MAX_VALUE, cb, &lock);
            ret = xtimer_mutex_lock_timeout(
                &lock, offset * US_PER_TICK + MIN_WAIT_US);
        }
        if (offset > value) {
            value = offset;
        }
        printf(".");
        fflush(stdout);
        samples++;
    }
    printf("\n");

    printf("RTT_MIN_OFFSET for %s over %" PRIu32 " samples: %" PRIu32 "\n",
           RIOT_BOARD, samples, value);

    return 0;
}
