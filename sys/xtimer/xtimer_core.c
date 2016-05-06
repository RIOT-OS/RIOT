/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2016 Eistec AB
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
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
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
#define ENABLE_DEBUG (0)
#include "debug.h"

static int _in_handler = 0;

static volatile uint32_t _long_cnt = 0;

static uint32_t _safe_long_cnt(uint32_t now)
{
    uint32_t long_value = _long_cnt;

    /* _long_cnt counts half 32bit intervals. so to compensate for a possibly
     * late overflow interrupt, compare if _long_cnt's LSB and the timer's MSB
     * match. If (_long_cnt & 1) is set but the timer is already in the next
     * period ((now >> 32) == 0), the overflow interrupt has been missed.
     */

    return (long_value >> 1) + ((long_value & 1) && (~now >> 31));
}

#if XTIMER_MASK
volatile uint32_t _xtimer_high_cnt = 0;
#define XTIMER_HALFTICK     (_xtimer_high_cnt & XTIMER_MSBMASK)
#define XTIMER_LL_HALFTICK  (_xtimer_lltimer_now() & XTIMER_MSBMASK)
#else
#define XTIMER_HALFTICK     (_long_cnt & 1)
#define XTIMER_LL_HALFTICK  (_xtimer_lltimer_now() >> 31)
#endif

#define _NEXT_OVERFLOW_TICK (_xtimer_lltimer_mask(0xFFFFFFFF) >> (!XTIMER_HALFTICK))

static xtimer_t *timer_list_head = NULL;

static void _add_timer_to_list(xtimer_t **list_head, xtimer_t *timer);
static void _shoot(xtimer_t *timer);
static void _remove(xtimer_t *timer);
static uint32_t _time_left(xtimer_t *timer);
static inline void _lltimer_set(uint32_t target);
static void _update_lltimer(void);

static void _timer_callback(void);
static void _periph_timer_callback(void *arg, int chan);

static inline int _is_set(xtimer_t *timer)
{
    return (timer->target || timer->long_target);
}

static inline void xtimer_spin_until(uint32_t target) {
#if XTIMER_MASK
    target = _xtimer_lltimer_mask(target);
#endif
    while (_xtimer_lltimer_now() > target);
    while (_xtimer_lltimer_now() < target);
}

void xtimer_init(void)
{
    /* initialize low-level timer */
    timer_init(XTIMER_DEV, XTIMER_USEC_TO_TICKS(1000000ul), _periph_timer_callback, NULL);

    /* register initial overflow tick */
    _update_lltimer();
}

static void _xtimer_now64(uint32_t *short_term, uint32_t *long_term)
{
    uint32_t short_value = xtimer_now();
    *short_term = short_value;
    *long_term = _safe_long_cnt(short_value);
}

uint64_t xtimer_now64(void)
{
    uint32_t short_term, long_term;
    _xtimer_now64(&short_term, &long_term);

    return ((uint64_t)long_term<<32) + short_term;
}

void xtimer_set(xtimer_t *timer, uint32_t offset)
{
    _xtimer_set64(timer, offset, 0);
}

void _xtimer_set64(xtimer_t *timer, uint32_t offset, uint32_t long_offset)
{
    DEBUG(" _xtimer_set64() offset=%" PRIu32 " long_offset=%" PRIu32 "\n", offset, long_offset);
    _xtimer_now64(&timer->target, &timer->long_target);
    timer->target += offset;
    timer->long_target += long_offset;
    if (timer->target < offset) {
        timer->long_target++;
    }

    _xtimer_set_absolute(timer);
}

void _xtimer_set_absolute(xtimer_t *timer)
{
    DEBUG("timer_set_absolute(): now=%" PRIu32 " target=%" PRIu32 "\n",xtimer_now(), timer->target);

    timer->next = NULL;
    if (_time_left(timer) < XTIMER_BACKOFF) {
        while(_time_left(timer));
        _shoot(timer);
        return;
    }

    unsigned state = irq_disable();
    if (_is_set(timer)) {
        _remove(timer);
    }

    _add_timer_to_list(&timer_list_head, timer);

    if (timer_list_head == timer) {
        DEBUG("timer_set_absolute(): timer is new list head. updating lltimer.\n");
        _update_lltimer();
    }

    irq_restore(state);
}


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

static void _add_timer_to_list(xtimer_t **list_head, xtimer_t *timer)
{
    while (*list_head
        && (((*list_head)->long_target < timer->long_target)
        || (((*list_head)->long_target == timer->long_target) && ((*list_head)->target <= timer->target)))) {
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

static void _remove(xtimer_t *timer)
{
    if (timer_list_head == timer) {
        timer_list_head = timer->next;
        _update_lltimer();
    }
    else {
        _remove_timer_from_list(&timer_list_head, timer);
    }
}

void xtimer_remove(xtimer_t *timer)
{
    int state = irq_disable();
    if (_is_set(timer)) {
        _remove(timer);
    }
    irq_restore(state);
}

static uint32_t _time_left(xtimer_t *timer)
{
    uint32_t short_now, long_now, result;
    _xtimer_now64(&short_now, &long_now);

    if (timer->long_target < long_now) {
        return 0;
    }

    if (timer->long_target > long_now) {
        return 0xffffffff;
    }

    result = timer->target - short_now;
    if (result > timer->target) {
        return 0;
    }
    else {
        return result;
    }
}

static inline void _lltimer_set(uint32_t target)
{
    if (_in_handler) {
        return;
    }
#ifdef XTIMER_SHIFT
    target = XTIMER_USEC_TO_TICKS(target);
    if (!target) {
        target++;
    }
#endif
    if (_xtimer_lltimer_mask(target - _xtimer_lltimer_now()) > (_xtimer_lltimer_mask(0xFFFFFFFF)>>1) ) {
        target = _xtimer_lltimer_now() + XTIMER_ISR_BACKOFF;
    }
    timer_set_absolute(XTIMER, XTIMER_CHAN, _xtimer_lltimer_mask(target));
}

static void _update_lltimer(void)
{
    uint32_t next_target = _NEXT_OVERFLOW_TICK;

    if (timer_list_head) {
        uint32_t left = _time_left(timer_list_head);
        if (left < (_NEXT_OVERFLOW_TICK - _xtimer_lltimer_now())) {
            /* schedule callback on next timer target time */
            next_target = _xtimer_lltimer_mask(_xtimer_lltimer_now() + left - XTIMER_OVERHEAD);
        }
    }

    /* set low level timer */
    _lltimer_set(next_target);
}

/**
 * @brief handle low-level timer half-period, advance to next half period
 */
static void _next_half_period(void)
{
#if XTIMER_MASK
    /*
     * advance <32bit high count variable.
     * (XTIMER_PERIOD_LENGTH >> 1) evaluates to 0x8000 for 16bit timers
     * with XTIMER_MASK==0xFFFF.
     */
    _xtimer_high_cnt += (XTIMER_PERIOD_LENGTH >> 1);

    if ((_xtimer_high_cnt == 0) || (_xtimer_high_cnt == (1LU << 31))) {
        /* high_cnt overflowed or reached half 32bit period,
         * so advance 32bit half-period counter */
        _long_cnt++;
    }
#else
    /* advance 32bit half-period counter */
    _long_cnt++;
#endif
}

/**
 * @brief main xtimer callback function
 */
static void _timer_callback(void)
{
    DEBUG("T: now=%" PRIu32 " (ll=%" PRIu32 ")\n", xtimer_now(), _xtimer_lltimer_now());
    _in_handler = 1;

    do {
        /* check if next timers are close to expiring */
        while (timer_list_head && (_time_left(timer_list_head) < XTIMER_ISR_BACKOFF)) {
            /* make sure we don't fire too early */
            while (_time_left(timer_list_head));

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

        /* if the saved half-period tick doesn't match the timer value MSB, we
         * have to advance into the next half-period */
        if (XTIMER_HALFTICK != XTIMER_LL_HALFTICK) {
            _next_half_period();
            continue;
        }
    } while(0);

    _in_handler = 0;
    _update_lltimer();
}
