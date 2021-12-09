/*
 * Copyright (C) 2021 Kaspar Schleiser <kaspar@schleiser.de>
 *               2021 Freie Universität Berlin
 *               2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer64
 * @{
 *
 * @file
 * @brief       ztimer64 core functionality
 *
 * This file contains ztimer64's main API implementation and functionality
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "ztimer64.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Checkpointing interval, don't change. See `ztimer64_now()` for an explanation
 * of the checkpointing.
 */
#define ZTIMER64_CHECKPOINT_INTERVAL (1LU << 31)

static int _add_entry_to_list(ztimer64_clock_t *clock, ztimer64_base_t *entry);
static int _del_entry_from_list(ztimer64_clock_t *clock,
                                ztimer64_base_t *entry);
static void _ztimer64_update(ztimer64_clock_t *clock);

/* debug aid */
void ztimer64_clock_print(const ztimer64_clock_t *clock);

static unsigned _is_set(const ztimer64_t *t)
{
    /* no timer can ever trigger at `target==0`, so we use that
     * as marker */
    return !!(t->base.target);
}

unsigned ztimer64_is_set(const ztimer64_t *timer)
{
    return _is_set(timer);
}

void ztimer64_remove(ztimer64_clock_t *clock, ztimer64_t *timer)
{
    unsigned state = irq_disable();

    if (_is_set(timer)) {
        if (_del_entry_from_list(clock, &timer->base) == 0) {
            _ztimer64_update(clock);
        }
    }

    irq_restore(state);
}

void ztimer64_set_at(ztimer64_clock_t *clock, ztimer64_t *timer, uint64_t val)
{
    unsigned state = irq_disable();

    if (_is_set(timer)) {
        _del_entry_from_list(clock, &timer->base);
    }

    /* optionally subtract a configurable adjustment value */
    /* there is no API to set this ATM */
    if (val > clock->adjust_set) {
        val -= clock->adjust_set;
    }
    else {
        val = 0;
    }

    timer->base.target = val;

    /* NOTE: if val is 0 at this point, this will be fixed to "now()" in _ztimer64_update() */

    if (_add_entry_to_list(clock, &timer->base)) {
        _ztimer64_update(clock);
    }

    irq_restore(state);
}

static int _add_entry_to_list(ztimer64_clock_t *clock, ztimer64_base_t *entry)
{
    ztimer64_base_t *pos = clock->first;

    if (pos) {
        if (pos->target > entry->target) {
            /* special case: new entry's target is earlier than old list head */
            entry->next = pos;
            clock->first = entry;
            return 1;
        }
        else {
            /* Jump past all entries which are set to an earlier target than the new entry */
            while (pos->next && pos->next->target <= entry->target) {
                pos = pos->next;
            }
            entry->next = pos->next;
            pos->next = entry;
            return 0;
        }
    }
    else {
        /* adding first entry */
        entry->next = 0;
        clock->first = entry;
        return 1;
    }
}

static int _del_entry_from_list(ztimer64_clock_t *clock, ztimer64_base_t *entry)
{
    DEBUG("_del_entry_from_list()\n");
    if (!clock->first) {
        return 0;
    }
    assert(_is_set((ztimer64_t *)entry));
    if (clock->first == entry) {
        /* special case: removing first entry */
        clock->first = entry->next;
        entry->next = 0;
        return 1;
    }
    else {
        ztimer64_base_t *pos = clock->first;

        while (pos->next) {
            if (pos->next == entry) {
                pos->next = entry->next;
                break;
            }
            pos = pos->next;
        }

        entry->next = 0;

        return 0;
    }
}

uint64_t ztimer64_now(ztimer64_clock_t *clock)
{
    uint64_t now;
    unsigned state = irq_disable();
    uint32_t base_now = ztimer_now(clock->base_clock);

    /* ztimer64 checkpointing works by storing
     * the upper 33 bits in clock->checkpoint.
     * The final time is the lower 32bit time ORed
     * with that checkpoint.
     * The highest bit of the lower 32bit is used
     * as control bit. If the checkpoint and 32bit
     * time both have that bit set (or not), no correction is necessary.
     * If checkpoint and 32bit time disagree on that bit,
     * the checkpoint lags behind, and needs to be updated.
     * This works as long as the checkpoint is at most (2**32-1) behind,
     * which is ensured by setting the base clock timer at most
     * ZTIMER64_CHECKPOINT_INTERVAL into the future.
     */
    if ((clock->checkpoint & ZTIMER64_CHECKPOINT_INTERVAL)
        ^ (base_now & ZTIMER64_CHECKPOINT_INTERVAL)) {
        clock->checkpoint += ZTIMER64_CHECKPOINT_INTERVAL;
    }

    now = clock->checkpoint | base_now;

    irq_restore(state);
    return now;
}

static void _ztimer64_update(ztimer64_clock_t *clock)
{
    uint64_t now = ztimer64_now(clock);
    uint64_t next_checkpoint = clock->checkpoint + ZTIMER64_CHECKPOINT_INTERVAL;
    uint64_t target;

    if (next_checkpoint < now) {
        next_checkpoint = now;
    }

    if (!clock->first) {
        target = next_checkpoint;
    }
    else {
        uint64_t next = clock->first->target;
        if (next > now) {
            if (next < next_checkpoint) {
                /* timer within range */
                target = next;
            }
            else {
                /* timer out of range */
                target = next_checkpoint;
            }
        }
        else {
            /* timer has passed already, trigger ASAP */
            target = now;

            /* if we arrive here due to a timer with "target==0" being set,
             * fix this here to consider it "set".
             */
            if (next == 0) {
                clock->first->target = now;
            }
        }
    }

    ztimer_set(clock->base_clock, &clock->base_timer, target - now);
}

void ztimer64_handler(void *arg)
{
    ztimer64_clock_t *clock = arg;

    while (clock->first) {
        uint64_t now = ztimer64_now(clock);
        if (clock->first->target <= now) {
            ztimer64_t *timer = (ztimer64_t *)clock->first;
            /* remove timer from list */
            clock->first = clock->first->next;
            /* clear timer struct so it can be re-set */
            timer->base.next = NULL;
            timer->base.target = 0;

            /* shoot callback */
            timer->callback(timer->arg);
        }
        else {
            /* timer target is in the future */
            break;
        }
    }

    _ztimer64_update(clock);
}

void ztimer64_clock_init(ztimer64_clock_t *clock, ztimer_clock_t *base_clock)
{
    *clock =
        (ztimer64_clock_t){ .base_clock = base_clock,
                            .base_timer =
                            { .callback = ztimer64_handler, .arg = clock } };

    /* initialize checkpointing */
    _ztimer64_update(clock);
}

#if MODULE_ZTIMER64_USEC
static ztimer64_clock_t _ztimer64_usec;
ztimer64_clock_t *const ZTIMER64_USEC = &_ztimer64_usec;
#endif
#if MODULE_ZTIMER64_MSEC
static ztimer64_clock_t _ztimer64_msec;
ztimer64_clock_t *const ZTIMER64_MSEC = &_ztimer64_msec;
#endif
#if MODULE_ZTIMER64_SEC
static ztimer64_clock_t _ztimer64_sec;
ztimer64_clock_t *const ZTIMER64_SEC = &_ztimer64_sec;
#endif

void ztimer64_init(void)
{
#if MODULE_ZTIMER64_USEC
    ztimer64_clock_init(ZTIMER64_USEC, ZTIMER_USEC);
#endif
#if MODULE_ZTIMER64_MSEC
    ztimer64_clock_init(ZTIMER64_MSEC, ZTIMER_MSEC);
#endif
#if MODULE_ZTIMER64_SEC
    ztimer64_clock_init(ZTIMER64_SEC, ZTIMER_SEC);
#endif
}

void ztimer64_clock_print(const ztimer64_clock_t *clock)
{
    const ztimer64_base_t *entry = clock->first;

    while (entry) {
        printf("0x%08x:%" PRIu64 "\n", (unsigned)entry,
               entry->target);

        entry = entry->next;
    }
    puts("");
}
