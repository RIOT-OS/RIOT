/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Shared peripheral timer code
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph/timer.h"
#include "irq.h"

#ifndef PERIPH_TIMER_PROVIDES_SET
int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    unsigned int state = irq_disable();
    int res = timer_set_absolute(dev, channel, timer_read(dev) + timeout);
    irq_restore(state);
    return res;
}
#endif

#ifdef MODULE_PERIPH_TIMER_QUERY_FREQS
__attribute__((weak))
uword_t timer_query_channel_numof(tim_t dev)
{
    (void)dev;
    return TIMER_CHANNEL_NUMOF;
}

uint32_t timer_get_closest_freq(tim_t dev, uint32_t target)
{
    uint32_t freq;
    uword_t idx_max = timer_query_freqs_numof(dev) - 1;

    /* use binary search to find one of the three possibilities:
     * 1. If an exact match is possible: The exact match is found
     * 2. Otherwise the closest frequency is found, which might be either
     *      a) the highest frequency below the target, or
     *      b) the lowest frequency above the target
     */
    uword_t idx_lower = 0;
    uword_t idx_upper = idx_max;
    while (idx_lower != idx_upper) {
        uword_t idx_mid = (idx_lower + idx_upper) >> 1;
        freq = timer_query_freqs(dev, idx_mid);
        if (freq > target) {
            idx_lower = idx_mid + 1;
        }
        else {
            idx_upper = idx_mid;
        }
    }

    freq = timer_query_freqs(dev, idx_lower);
    if ((freq < target) && (idx_lower > 0)) {
        /* binary search yielded the closest frequency below the target. But
         * maybe the closest frequency above the target is actually better. */
        uint32_t diff = target - freq;
        uint32_t alternative = timer_query_freqs(dev, idx_lower - 1);
        if (target + diff > alternative) {
            /* the alternative is better */
            return alternative;
        }
    }
    else if ((freq > target) && (idx_lower < idx_max)) {
        /* Got a frequency above the target, maybe the one below would be
         * a closer match */
        uint32_t diff = freq - target;
        uint32_t alternative = timer_query_freqs(dev, idx_lower + 1);
        if (target < alternative + diff) {
            return alternative;
        }
    }

    /* either we got an exact match, or the other candidate was no closer */
    return freq;
}
#endif
