/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup xtimer
 * @{
 * @file
 * @brief xtimer core functionality
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>
#include <string.h>
#include "board.h"
#include "periph/timer.h"
#include "periph_conf.h"

#include "xtimer.h"
#include "irq.h"

/* WARNING! enabling this will have side effects and can lead to timer underflows. */
#define ENABLE_DEBUG 0
#include "debug.h"

static volatile uint32_t _long_cnt = 0;
#if XTIMER_MASK
volatile uint32_t _high_cnt = 0;
#endif

static xtimer_t *timer_list_head = NULL;
static xtimer_t *overflow_list_head = NULL;
static xtimer_t *long_list_head = NULL;

static void _add_timer_to_list(xtimer_t **list_head, xtimer_t *timer);
static void _add_timer_to_long_list(xtimer_t **list_head, xtimer_t *timer);
static void _shoot(xtimer_t *timer);
static inline void _lltimer_set(uint32_t target);
static uint32_t _time_left(uint32_t target, uint32_t reference);

static void _timer_callback(void);
static void _periph_timer_callback(int chan);

static inline int _this_high_period(uint32_t target);

static inline int _is_set(xtimer_t *timer)
{
    return (timer->target || timer->long_target);
}

void xtimer_init(void)
{
    /* initialize low-level timer */
    timer_init(XTIMER, 1 /* us_per_tick */, _periph_timer_callback);

    /* register initial overflow tick */
    _lltimer_set(0xFFFFFFFF);
}

static void _xtimer_now64(uint32_t *short_term, uint32_t *long_term)
{
    uint32_t before, after, long_value;

    /* loop to cope with possible overflow of xtimer_now() */
    do {
        before = xtimer_now();
        long_value = _long_cnt;
        after = xtimer_now();

    } while(before > after);

    *short_term = after;
    *long_term = long_value;
}

uint64_t xtimer_now64(void)
{
    uint32_t short_term, long_term;
    _xtimer_now64(&short_term, &long_term);

    return ((uint64_t)long_term<<32) + short_term;
}

void _xtimer_set64(xtimer_t *timer, uint32_t offset, uint32_t long_offset)
{
    DEBUG(" _xtimer_set64() offset=%" PRIu32 " long_offset=%" PRIu32 "\n", offset, long_offset);
    if (!long_offset) {
        /* timer fits into the short timer */
        xtimer_set(timer, (uint32_t) offset);
    }
    else {
        xtimer_remove(timer);

        _xtimer_now64(&timer->target, &timer->long_target);
        timer->target += offset;
        timer->long_target += long_offset;
        if (timer->target < offset) {
            timer->long_target++;
        }

        int state = disableIRQ();
        _add_timer_to_long_list(&long_list_head, timer);
        restoreIRQ(state);
        DEBUG("xtimer_set64(): added longterm timer (long_target=%" PRIu32 " target=%" PRIu32 ")\n",
                timer->long_target, timer->target);
    }
}

void xtimer_set(xtimer_t *timer, uint32_t offset)
{
    DEBUG("timer_set(): offset=%" PRIu32 " now=%" PRIu32 " (%" PRIu32 ")\n", offset, xtimer_now(), _xtimer_now());
    if (!timer->callback) {
        DEBUG("timer_set(): timer has no callback.\n");
        return;
    }

    xtimer_remove(timer);
    uint32_t target = xtimer_now() + offset;

    if (offset < XTIMER_BACKOFF) {
        if (offset > 1) {
            /* spin until timer should be run */
            xtimer_spin_until(target);
        }

        _shoot(timer);
    }
    else {
        _xtimer_set_absolute(timer, target);
    }
}

static void _periph_timer_callback(int chan)
{
    (void)chan;
    _timer_callback();
}

static void _shoot(xtimer_t *timer)
{
    timer->callback(timer->arg);
}

static inline void _lltimer_set(uint32_t target)
{
    DEBUG("__lltimer_set(): setting %" PRIu32 "\n", _mask(target));
#ifdef XTIMER_SHIFT
    target >>= XTIMER_SHIFT;
    if (!target) {
        target++;
    }
#endif
    timer_set_absolute(XTIMER, XTIMER_CHAN, _mask(target));
}

int _xtimer_set_absolute(xtimer_t *timer, uint32_t target)
{
    uint32_t now = xtimer_now();
    int res = 0;

    DEBUG("timer_set_absolute(): now=%" PRIu32 " target=%" PRIu32 "\n", now, target);

    timer->next = NULL;
    if ((target >= now) && ((target - XTIMER_BACKOFF) < now)) {
        /* backoff */
        xtimer_spin_until(target);
        _shoot(timer);
        return 0;
    }

    timer->target = target;
    timer->long_target = _long_cnt;
    if (target < now) {
        timer->long_target++;
    }

    unsigned state = disableIRQ();
    if ( !_this_high_period(target) ) {
        DEBUG("xtimer_set_absolute(): the timer doesn't fit into the low-level timer's mask.\n");
        _add_timer_to_long_list(&long_list_head, timer);
    }
    else {
        if (_mask(now) >= target) {
            DEBUG("xtimer_set_absolute(): the timer will expire in the next timer period\n");
            _add_timer_to_list(&overflow_list_head, timer);
        }
        else {
            DEBUG("timer_set_absolute(): timer will expire in this timer period.\n");
            _add_timer_to_list(&timer_list_head, timer);

            if (timer_list_head == timer) {
                DEBUG("timer_set_absolute(): timer is new list head. updating lltimer.\n");
                _lltimer_set(target - XTIMER_OVERHEAD);
            }
        }
    }

    restoreIRQ(state);

    return res;
}

static void _add_timer_to_list(xtimer_t **list_head, xtimer_t *timer)
{
    while (*list_head && (*list_head)->target <= timer->target) {
        list_head = &((*list_head)->next);
    }

    timer->next = *list_head;
    *list_head = timer;
}

static void _add_timer_to_long_list(xtimer_t **list_head, xtimer_t *timer)
{
    while (*list_head
            && (*list_head)->long_target <= timer->long_target
            && (*list_head)->target <= timer->target) {
        list_head = &((*list_head)->next);
    }

    timer->next = *list_head;
    *list_head = timer;
}

static int _remove_timer_from_list(xtimer_t **list_head, xtimer_t *timer)
{
    while (*list_head) {
        if (*list_head == timer) {
            *list_head = timer->next;
            return 1;
        }
        list_head = &((*list_head)->next);
    }

    return 0;
}

int xtimer_remove(xtimer_t *timer)
{
    if (!_is_set(timer)) {
        return 0;
    }

    unsigned state = disableIRQ();
    int res = 0;
    if (timer_list_head == timer) {
        uint32_t next;
        timer_list_head = timer->next;
        if (timer_list_head) {
            /* schedule callback on next timer target time */
            next = timer_list_head->target - XTIMER_OVERHEAD;
        }
        else {
            next = _mask(0xFFFFFFFF);
        }
        _lltimer_set(next);
    }
    else {
        res = _remove_timer_from_list(&timer_list_head, timer) ||
            _remove_timer_from_list(&overflow_list_head, timer) ||
            _remove_timer_from_list(&long_list_head, timer);
    }

    timer->target = 0;
    timer->long_target = 0;

    restoreIRQ(state);

    return res;
}

static uint32_t _time_left(uint32_t target, uint32_t reference)
{
    uint32_t now = _xtimer_now();

    if (now < reference) {
        return 0;
    }

    if (target > now) {
        return target - now;
    }
    else {
        return 0;
    }
}

static inline int _this_high_period(uint32_t target) {
#if XTIMER_MASK
    return (target & XTIMER_MASK) == _high_cnt;
#else
    (void)target;
    return 1;
#endif
}

/**
 * @brief compare two timers' target values, return the one with lower value.
 *
 * if either is NULL, return the other.
 * if both are NULL, return NULL.
 */
static inline xtimer_t *_compare(xtimer_t *a, xtimer_t *b)
{
    if (a && b) {
        return a->target <= b->target ? a : b;
    }
    else {
        return a ? a : b;
    }
}

/**
 * @brief merge two timer lists, return head of new list
 */
static xtimer_t *_merge_lists(xtimer_t *head_a, xtimer_t *head_b)
{
    xtimer_t *result_head = _compare(head_a, head_b);
    xtimer_t *pos = result_head;

    while(1) {
        head_a = head_a->next;
        head_b = head_b->next;
        if (!head_a) {
            pos->next = head_b;
            break;
        }
        if (!head_b) {
            pos->next = head_a;
            break;
        }

        pos->next = _compare(head_a, head_b);
        pos = pos->next;
    }

    return result_head;
}

/**
 * @brief parse long timers list and copy those that will expire in the current
 *        short timer period
 */
static void _select_long_timers(void)
{
    xtimer_t *select_list_start = long_list_head;
    xtimer_t *select_list_last = NULL;

    /* advance long_list head so it points to the first timer of the next (not
     * just started) "long timer period" */
    while (long_list_head) {
        if ((long_list_head->long_target <= _long_cnt) && _this_high_period(long_list_head->target)) {
            select_list_last = long_list_head;
            long_list_head = long_list_head->next;
        }
        else {
            /* remaining long_list timers belong to later long periods */
            break;
        }
    }

    /* cut the "selected long timer list" at the end */
    if (select_list_last) {
        select_list_last->next = NULL;
    }

    /* merge "current timer list" and "selected long timer list" */
    if (timer_list_head) {
        if (select_list_last) {
            /* both lists are non-empty. merge. */
            timer_list_head = _merge_lists(timer_list_head, select_list_start);
        }
        else {
            /* "selected long timer list" is empty, nothing to do */
        }
    }
    else { /* current timer list is empty */
        if (select_list_last) {
            /* there's no current timer list, but a non-empty "selected long
             * timer list".  So just use that list as the new current timer
             * list.*/
            timer_list_head = select_list_start;
        }
    }
}

/**
 * @brief handle low-level timer overflow, advance to next short timer period
 */
static void _next_period(void)
{
#if XTIMER_MASK
    /* advance <32bit mask register */
    _high_cnt += ~XTIMER_MASK + 1;
    if (! _high_cnt) {
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

    DEBUG("_timer_callback() now=%" PRIu32 " (%" PRIu32 ")pleft=%" PRIu32 "\n", xtimer_now(),
            _mask(xtimer_now()), _mask(0xffffffff-xtimer_now()));

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
        while (_xtimer_now() == _mask(0xFFFFFFFF));
    }
    else {
        /* we ended up in _timer_callback and there is
         * a timer waiting.
         */
        /* set our period reference to that timer's target time. */
        reference = _mask(timer_list_head->target);
    }

overflow:
    /* check if next timers are close to expiring */
    while (timer_list_head && (_time_left(_mask(timer_list_head->target), reference) < XTIMER_ISR_BACKOFF)) {
        /* make sure we don't fire too early */
        while (_time_left(_mask(timer_list_head->target), 0));

        xtimer_t *next = timer_list_head->next;

        _shoot(timer_list_head);

        /* advance to next timer in list */
        timer_list_head = next;
    }

    /* possibly executing all callbacks took enough
     * time to overflow.  In that case we advance to
     * next timer period and check again for expired
     * timers.*/
    if (reference > _xtimer_now()) {
        DEBUG("_timer_callback: overflowed while executing callbacks. %i\n", timer_list_head != 0);
        _next_period();
        reference = 0;
        goto overflow;
    }

    if (timer_list_head) {
        /* schedule callback on next timer target time */
        next_target = timer_list_head->target - XTIMER_OVERHEAD;
    }
    else {
        /* there's no timer planned for this timer period */
        /* schedule callback on next overflow */
        next_target = _mask(0xFFFFFFFF);
        uint32_t now = _xtimer_now();

        /* check for overflow again */
        if (now < reference) {
            _next_period();
            reference = 0;
            goto overflow;
        }
        else {
            /* check if the end of this period is very soon */
            if (_mask(now + XTIMER_ISR_BACKOFF) < now) {
                /* spin until next period, then advance */
                while (_xtimer_now() > now);
                _next_period();
                reference = 0;
                goto overflow;
            }
        }
    }

    /* set low level timer */
    _lltimer_set(next_target);
}
