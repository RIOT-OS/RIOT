/**
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    xtimer
 *
 * @{
 *
 * @file
 * @brief      xtimer hardware abstraction layer
 *
 * @author     Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

/* WARNING! enabling this will have side effects and can lead to timer underflows. */
#define ENABLE_DEBUG 0
#include "debug.h"

static void _periph_timer_callback(void *arg, int chan)
{
    (void)arg;
    (void)chan;
    _timer_callback();
}

static void _shoot(xtimer_t *timer)
{
    timer->callback(timer->arg);
}

static inline void _lltimer_set(uint32_t target)
{
    if (_in_handler) {
        return;
    }
    DEBUG("_lltimer_set(): setting %" PRIu32 "\n", _xtimer_lltimer_mask(target));
    timer_set_absolute(XTIMER_DEV, XTIMER_CHAN, _xtimer_lltimer_mask(target));
}

/**
 * @brief handle low-level timer overflow, advance to next short timer period
 */
static void _next_period(void)
{
#if XTIMER_MASK
    /* advance <32bit mask register */
    _xtimer_high_cnt += ~XTIMER_MASK + 1;
    if (_xtimer_high_cnt == 0) {
        /* high_cnt overflowed, so advance >32bit counter */
        _long_cnt++;
    }
#else
    /* advance >32bit counter */
    _long_cnt++;
#endif

    /* swap overflow list to current timer list */
    timer_list_head = overflow_list_head;
    overflow_list_head = NULL;

    _select_long_timers();
}

/**
 * @brief main xtimer callback function
 */
static void _timer_callback(void)
{
    uint32_t next_target;
    uint32_t reference;

    _in_handler = 1;

    DEBUG("_timer_callback() now=%" PRIu32 " (%" PRIu32 ")pleft=%" PRIu32 "\n",
          xtimer_now().ticks32, _xtimer_lltimer_mask(xtimer_now().ticks32),
          _xtimer_lltimer_mask(0xffffffff - xtimer_now().ticks32));

    if (!timer_list_head) {
        DEBUG("_timer_callback(): tick\n");
        /* there's no timer for this timer period,
         * so this was a timer overflow callback.
         *
         * In this case, we advance to the next timer period.
         */
        _next_period();

        reference = 0;

        /* make sure the timer counter also arrived
         * in the next timer period */
        while (_xtimer_lltimer_now() == _xtimer_lltimer_mask(0xFFFFFFFF)) {}
    }
    else {
        /* we ended up in _timer_callback and there is
         * a timer waiting.
         */
        /* set our period reference to the current time. */
        reference = _xtimer_lltimer_now();
    }

overflow:
    /* check if next timers are close to expiring */
    while (timer_list_head && (_time_left(_xtimer_lltimer_mask(timer_list_head->target), reference) < XTIMER_ISR_BACKOFF)) {
        /* make sure we don't fire too early */
        while (_time_left(_xtimer_lltimer_mask(timer_list_head->target), reference)) {}

        /* pick first timer in list */
        xtimer_t *timer = timer_list_head;

        /* advance list */
        timer_list_head = timer->next;

        /* make sure timer is recognized as being already fired */
        timer->target = 0;
        timer->long_target = 0;

        /* fire timer */
        _shoot(timer);
    }

    /* possibly executing all callbacks took enough
     * time to overflow.  In that case we advance to
     * next timer period and check again for expired
     * timers.*/
    if (reference > _xtimer_lltimer_now()) {
        DEBUG("_timer_callback: overflowed while executing callbacks. %i\n",
              timer_list_head != NULL);
        _next_period();
        reference = 0;
        goto overflow;
    }

    if (timer_list_head) {
        /* schedule callback on next timer target time */
        next_target = timer_list_head->target - XTIMER_OVERHEAD;

        /* make sure we're not setting a time in the past */
        if (next_target < (_xtimer_lltimer_now() + XTIMER_ISR_BACKOFF)) {
            goto overflow;
        }
    }
    else {
        /* there's no timer planned for this timer period */
        /* schedule callback on next overflow */
        next_target = _xtimer_lltimer_mask(0xFFFFFFFF);
        uint32_t now = _xtimer_lltimer_now();

        /* check for overflow again */
        if (now < reference) {
            _next_period();
            reference = 0;
            goto overflow;
        }
        else {
            /* check if the end of this period is very soon */
            if (_xtimer_lltimer_mask(now + XTIMER_ISR_BACKOFF) < now) {
                /* spin until next period, then advance */
                while (_xtimer_lltimer_now() >= now) {}
                _next_period();
                reference = 0;
                goto overflow;
            }
        }
    }

    _in_handler = 0;

    /* set low level timer */
    _lltimer_set(next_target);
}
