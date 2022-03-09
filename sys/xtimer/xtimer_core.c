/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
 *               2018 Josua Arndt
 *               2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_xtimer
 *
 * @{
 * @file
 * @brief xtimer core functionality
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#ifndef MODULE_XTIMER_ON_ZTIMER
#include "board.h"
#include "periph/timer.h"
#include "periph_conf.h"
#endif

#include "assert.h"
#include "xtimer.h"
#include "irq.h"

/* WARNING! enabling this will have side effects and can lead to timer underflows. */
#define ENABLE_DEBUG 0
#include "debug.h"

static volatile int _in_handler = 0;

volatile uint64_t _xtimer_current_time = 0;

static xtimer_t *timer_list_head = NULL;
static xtimer_t *long_list_head = NULL;
static bool _lltimer_ongoing = false;

static void _add_timer_to_list(xtimer_t **list_head, xtimer_t *timer, uint32_t now);
static void _shoot(xtimer_t *timer);
static inline void _update_short_timers(uint64_t *now);
static inline void _update_long_timers(uint64_t *now);
static inline void _schedule_earliest_lltimer(uint32_t now);

static void _timer_callback(void);

#ifndef MODULE_XTIMER_ON_ZTIMER
static void _periph_timer_callback(void *arg, int chan);
#else
static void _ztimer_callback(void *arg);
static ztimer_t _ztimer = { .callback=_ztimer_callback };
#endif

void xtimer_init(void)
{
#ifndef MODULE_XTIMER_ON_ZTIMER
    /* initialize low-level timer */
    int ret = timer_init(XTIMER_DEV, XTIMER_HZ, _periph_timer_callback, NULL);
    (void)ret;
    assert(ret == 0);
#endif

    /* register initial overflow tick */
    _schedule_earliest_lltimer(_xtimer_now());
}

uint32_t _xtimer_now(void)
{
    return (uint32_t) _xtimer_now64();
}

void _xtimer_set64(xtimer_t *timer, uint32_t offset, uint32_t long_offset)
{
    DEBUG(" _xtimer_set64() offset=%" PRIu32 " long_offset=%" PRIu32 "\n", offset, long_offset);

    if (!timer->callback) {
        DEBUG("_xtimer_set64(): timer has no callback.\n");
        return;
    }

    xtimer_remove(timer);

    if (!long_offset && offset < XTIMER_BACKOFF) {
        /* timer fits into the short timer */
        _xtimer_spin(offset);
        _shoot(timer);
        return;
    }

    /* time sensitive */
    unsigned int state = irq_disable();
    uint64_t now = _xtimer_now64();
    timer->offset = offset;
    timer->long_offset = long_offset;
    timer->start_time = (uint32_t)now;
    timer->long_start_time = (uint32_t)(now >> 32);

    if (!long_offset) {
        _add_timer_to_list(&timer_list_head, timer, (uint32_t)now);

        if (timer_list_head == timer) {
            DEBUG("_xtimer_set64(): timer is new list head. updating lltimer.\n");
            _schedule_earliest_lltimer((uint32_t)now);
        }
    }
    else {
        _add_timer_to_list(&long_list_head, timer, (uint32_t)now);
        DEBUG("_xtimer_set64(): added longterm timer.\n");
    }
    irq_restore(state);
}

#ifndef MODULE_XTIMER_ON_ZTIMER
static void _periph_timer_callback(void *arg, int chan)
{
    (void)arg;
    (void)chan;
    _timer_callback();
}
#else
static void _ztimer_callback(void *arg)
{
    (void)arg;
    _timer_callback();
}
#endif

static void _shoot(xtimer_t *timer)
{
    timer->callback(timer->arg);
}

static inline void _schedule_earliest_lltimer(uint32_t now)
{
    uint32_t target;

    if (_in_handler) {
        return;
    }

    if (timer_list_head && timer_list_head->offset <= (_xtimer_lltimer_mask(0xFFFFFFFF)>>1)) {
        /* schedule lltimer on next timer target time */
        target = timer_list_head->start_time + timer_list_head->offset;
    }
    else if (!_lltimer_ongoing) {
        /* schedule lltimer after max_low_level_time/2 to detect a cycle */
        target = now + (_xtimer_lltimer_mask(0xFFFFFFFF)>>1);
    }
    else {
        /* lltimer is already running */
        return;
    }

    DEBUG("_schedule_earliest_lltimer(): setting %" PRIu32 "\n", _xtimer_lltimer_mask(target));
#ifndef MODULE_XTIMER_ON_ZTIMER
    timer_set_absolute(XTIMER_DEV, XTIMER_CHAN, _xtimer_lltimer_mask(target));
#else
    ztimer_set(ZTIMER_USEC, &_ztimer, target - ztimer_now(ZTIMER_USEC));
#endif
    _lltimer_ongoing = true;
}

/**
 * @brief   compare two timers. return true if timerA expires earlier than or
 *          equal to timerB and false otherwise.
 */
static bool _timer_comparison(xtimer_t* timerA, xtimer_t* timerB, uint32_t now)
{
    if (timerA->long_offset < timerB->long_offset) {
        return true;
    }
    if (timerA->long_offset == timerB->long_offset) {
        uint32_t elapsedA = now - timerA->start_time;
        uint32_t elapsedB = now - timerB->start_time;
        /* it is necessary to compare two offsets, instead of two absolute times
         * two conditions: (1) timerA was already expired
         *                 (2) timerA will expire earlier than or equal to timerB
         */
        if (timerA->offset < elapsedA || timerA->offset - elapsedA <= timerB->offset - elapsedB) {
            return true;
        }
    }
    return false;
}

/**
 * @brief add a timer to an ordered list of timers
 */
static void _add_timer_to_list(xtimer_t **list_head, xtimer_t *timer, uint32_t now)
{
    while (*list_head && _timer_comparison((*list_head), timer, now)) {
        list_head = &((*list_head)->next);
    }

    timer->next = *list_head;
    *list_head = timer;
}

/**
 * @brief remove a timer from an ordered list of timers
 */
static void _remove_timer_from_list(xtimer_t **list_head, xtimer_t *timer)
{
    while (*list_head) {
        if (*list_head == timer) {
            *list_head = timer->next;
            timer->next = NULL;
            return;
        }
        list_head = &((*list_head)->next);
    }
}

void xtimer_remove(xtimer_t *timer)
{
    /* time sensitive since the target timer can be fired */
    unsigned int state = irq_disable();
    timer->offset = 0;
    timer->long_offset = 0;
    timer->start_time = 0;
    timer->long_start_time = 0;

    _remove_timer_from_list(&timer_list_head, timer);
    _remove_timer_from_list(&long_list_head, timer);
    irq_restore(state);
}

/**
 * @brief update long timers' offsets and switch those that will expire in
 *        one short timer period to the short timer list
 */
static inline void _update_long_timers(uint64_t *now)
{
    xtimer_t *timer = long_list_head;

    while (timer) {
        uint32_t elapsed = (uint32_t)*now - timer->start_time;

        if (timer->offset < elapsed) {
            timer->long_offset--;
        }
        timer->offset -= elapsed;
        timer->start_time = (uint32_t)*now;
        timer->long_start_time = (uint32_t)(*now >> 32);

        if (!timer->long_offset) {
            assert(timer == long_list_head);

            _remove_timer_from_list(&long_list_head, timer);
            _add_timer_to_list(&timer_list_head, timer, (uint32_t)*now);
            timer = long_list_head;
        }
        else {
            timer = timer->next;
        }
    }
}

/**
 * @brief update short timers' offsets and fire those that are close to expiry
 */
static inline void _update_short_timers(uint64_t *now)
{
    xtimer_t *timer = timer_list_head;

    while (timer) {
        assert(!timer->long_offset);
        uint32_t elapsed = (uint32_t)*now - timer->start_time;
        if (timer->offset < elapsed || timer->offset - elapsed < XTIMER_ISR_BACKOFF) {
            assert(timer == timer_list_head);

            /* make sure we don't fire too early */
            if (timer->offset > elapsed) {
                while (_xtimer_now() - timer->start_time < timer->offset) {}
            }
            /* advance list */
            timer_list_head = timer->next;
            /* make sure timer is recognized as being already fired */
            timer->offset = 0;
            timer->start_time = 0;
            timer->long_start_time = 0;
            timer->next = NULL;
            /* fire timer */
            _shoot(timer);
            /* assign new head */
            timer = timer_list_head;
            /* update current_time */
            *now = _xtimer_now64();
        }
        else {
            timer->offset -= elapsed;
            timer->start_time = (uint32_t)*now;
            timer->long_start_time = (uint32_t)(*now >> 32);
            timer = timer->next;
        }
    }
}

/**
 * @brief main xtimer callback function (called in an interrupt context)
 */
static void _timer_callback(void)
{
    uint64_t now;
    _in_handler = 1;
    _lltimer_ongoing = false;
    now = _xtimer_now64();

update:
    /* update short timer offset and fire */
    _update_short_timers(&now);
    /* update long timer offset */
    _update_long_timers(&now);
    /* update current time */
    now = _xtimer_now64();

    if (timer_list_head) {
        /* make sure we're not setting a time in the past */
        uint32_t elapsed = (uint32_t)now - timer_list_head->start_time;
        if (timer_list_head->offset < elapsed ||
            timer_list_head->offset - elapsed < XTIMER_ISR_BACKOFF) {
            goto update;
        }
        else {
            timer_list_head->offset -= elapsed;
            timer_list_head->start_time = (uint32_t)now;
            timer_list_head->long_start_time = (uint32_t)(now >> 32);
        }
    }
    _in_handler = 0;

    /* set low level timer */
    _schedule_earliest_lltimer((uint32_t)now);
}
