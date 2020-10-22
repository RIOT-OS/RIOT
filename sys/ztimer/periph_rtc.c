/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup     sys_ztimer_periph_rtc
 * @{
 *
 * @file
 * @brief       ztimer periph/rtc backend implementation
 *
 * This implementation simply converts an integer time to split RTC values and
 * back, which is rather inefficient. If available, use ztimer_periph_rtt.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include "periph/rtc.h"
#include "ztimer/periph_rtc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* this algorithm and the one from _timestamp_to_gmt_civil() have been adapted from
 * http://ptspts.blogspot.com/2009/11/how-to-convert-unix-timestamp-to-civil.html.
 *
 * "The algorithmic solution above is part of the programming folklore."
 */
static uint32_t _gmt_civil_to_timestamp(unsigned y, unsigned m, unsigned d,
                                        unsigned h, unsigned mi, unsigned s)
{
    if (m <= 2) {
        y -= 1;
        m += 12;
    }
    return (365 * y + y / 4 - y / 100 + y / 400 + 3 * (m + 1) / 5 + 30 * m + d -
            719561) * 86400 + 3600 * h + 60 * mi + s;
}

void _timestamp_to_gmt_civil(struct tm *_tm, uint32_t epoch)
{
    uint32_t s = epoch % 86400;

    epoch /= 86400;
    uint32_t h = s / 3600;
    uint32_t m = s / 60 % 60;
    s = s % 60;
    uint32_t x = (epoch * 4 + 102032) / 146097 + 15;
    uint32_t b = epoch + 2442113 + x - (x / 4);
    uint32_t c = (b * 20 - 2442) / 7305;
    uint32_t d = b - 365 * c - c / 4;
    uint32_t e = d * 1000 / 30601;
    uint32_t f = d - e * 30 - e * 601 / 1000;

    if (e < 14) {
        struct tm tmp =
        { .tm_year = c - 4716 - 1900, .tm_mon = e - 1, .tm_mday = f,
          .tm_hour = h, .tm_min = m, .tm_sec = s };
        *_tm = tmp;
    }
    else {
        struct tm tmp =
        { .tm_year = c - 4715 - 1900, .tm_mon = e - 13, .tm_mday = f,
          .tm_hour = h, .tm_min = m, .tm_sec = s };
        *_tm = tmp;
    }
}

static void _ztimer_periph_rtc_callback(void *arg)
{
    ztimer_handler((ztimer_clock_t *)arg);
}

static uint32_t _ztimer_periph_rtc_now(ztimer_clock_t *clock)
{
    (void)clock;

    struct tm time;
    rtc_get_time(&time);

    return _gmt_civil_to_timestamp(time.tm_year + 1900, time.tm_mon,
                                   time.tm_mday, time.tm_hour, time.tm_min,
                                   time.tm_sec);
}

static void _ztimer_periph_rtc_set(ztimer_clock_t *clock, uint32_t val)
{
    unsigned state = irq_disable();

    uint32_t now = _ztimer_periph_rtc_now(NULL);
    uint32_t target;

    do {
        /* make sure there's no pending ISR */
        rtc_clear_alarm();

        target = now + val;

        struct tm _tm;
        _timestamp_to_gmt_civil(&_tm, target);

        /* TODO: ensure this doesn't underflow */
        rtc_set_alarm(&_tm, _ztimer_periph_rtc_callback, clock);

        if (val > 1) {
            /* If val <= 1, it is possible that the RTC second flips somewhere
             * between getting the current value and adding 1, resulting in
             * setting the current time as target, which in turn would make the
             * RTC never trigger. In that case, check the target that as been
             * set is still in the future at the end of the loop body.
             *
             * Skip that if val was more than a second away.
             */
            break;
        }
    } while (target <= (now = _ztimer_periph_rtc_now(NULL)));

    irq_restore(state);
}

static void _ztimer_periph_rtc_cancel(ztimer_clock_t *clock)
{
    (void)clock;
    rtc_clear_alarm();
}

static const ztimer_ops_t _ztimer_periph_rtc_ops = {
    .set = _ztimer_periph_rtc_set,
    .now = _ztimer_periph_rtc_now,
    .cancel = _ztimer_periph_rtc_cancel,
};

void ztimer_periph_rtc_init(ztimer_periph_rtc_t *clock)
{
    clock->ops = &_ztimer_periph_rtc_ops;
    clock->max_value = UINT32_MAX;
    rtc_init();
    rtc_poweron();
}
