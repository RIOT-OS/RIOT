/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Peripheral PTP clock test application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>

#include "event.h"
#include "fmt.h"
#include "kernel_defines.h"
#include "periph/ptp.h"

static uint64_t ts_target;
static uint64_t ts_hit;
static const uint64_t max_diff = 10000; /* 10 µs */
static int failed = 0;
static unsigned n_timeouts = 0;
static event_queue_t q = EVENT_QUEUE_INIT_DETACHED;

static inline uint64_t abs_diff(uint64_t a, uint64_t b)
{
    return (a >= b) ? (a - b) : (b - a);
}

static void ev_handler(event_t *ev)
{
    (void)ev;
    uint64_t diff = abs_diff(ts_target, ts_hit);
    if (diff > max_diff) {
        failed = 1;
        print_str("Error: Timeout missed target by ");
        print_u64_dec(diff);
        print_str(" ns\n");
    }

    n_timeouts++;

    if (n_timeouts == 1) {
        /* check if setting timeout in the past does trigger IRQ */
        ts_target = ptp_clock_read_u64();
        ptp_timer_set_absolute_u64(0);
    }
    else if (n_timeouts < 100) {
        /* check if setting small relative timeouts work */
        uint64_t delay = (99 - n_timeouts) * 1000;
        ts_target = ptp_clock_read_u64() + delay;
        ptp_timer_set_u64(delay);
    }
    else {
        if (failed) {
            print_str("TEST FAILED!\n");
        }
        else {
            print_str("TEST SUCCEEDED!\n");
        }
    }
}

void ptp_timer_cb(void)
{
    ts_hit = ptp_clock_read_u64();
    static event_t ev = { .handler = ev_handler };
    event_post(&q, &ev);
}

int main(void)
{
    ts_target = ptp_clock_read_u64() + NS_PER_SEC;
    ptp_timer_set_absolute_u64(ts_target);
    event_queue_claim(&q);
    event_loop(&q);
    UNREACHABLE();
}
