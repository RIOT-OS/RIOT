/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_spin_random
 *
 * @{
 * @file
 * @brief       spin_random implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

#include "spin_random.h"
#include "random.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Default is whatever, just some small delay if the user forgets to initialize */
static uint32_t spin_max = 64;

/**
 * @brief   Busy wait (spin) for the given number of loop iterations
 */
static void spin(uint32_t limit)
{
    /* Platform independent busy wait loop, should never be optimized out
     * because of the volatile asm statement */
    while (limit--) {
        __asm__ volatile ("");
    }
}

void spin_random_delay(void)
{
    uint32_t limit = random_uint32_range(0, spin_max);
    spin(limit);
}

uint32_t spin_random_calibrate(tim_t timer_dev, uint32_t spin_max_target)
{
    spin_max = 16;
    uint32_t t1;
    uint32_t t2;
    DEBUG("spin_random_calibrate: Begin calibration with timer %u against "
          "target %" PRIu32 "\n", (unsigned) timer_dev, spin_max_target);
    do {
        spin_max = (spin_max * 5) / 4;
        t1 = timer_read(timer_dev);
        spin(spin_max);
        t2 = timer_read(timer_dev);
    } while ((t2 - t1) < spin_max_target && (spin_max < (UINT32_MAX / 5)));
    DEBUG("spin_max = %" PRIu32 "\n", spin_max);
    return spin_max;
}
