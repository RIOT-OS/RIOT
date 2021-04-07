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
 * @ingroup     sys_ztimer
 * @{
 *
 * @file
 * @brief       ztimer core functionality
 *
 * This file contains ztimer's main API implementation and functionality
 * present in all ztimer clocks (most notably multiplexing ant extension).
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "kernel_defines.h"
#include "irq.h"
#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _add_entry_to_list(ztimer_clock_t *clock, ztimer_base_t *entry);
static void _del_entry_from_list(ztimer_clock_t *clock, ztimer_base_t *entry);
static void _ztimer_update(ztimer_clock_t *clock);
static void _ztimer_print(const ztimer_clock_t *clock);

#ifdef MODULE_ZTIMER_EXTEND
static inline uint32_t _min_u32(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}
#endif

static unsigned _is_set(const ztimer_clock_t *clock, const ztimer_t *t)
{
    if (!clock->list.next) {
        return 0;
    }
    else {
        return (t->base.next || &t->base == clock->last);
    }
}

unsigned ztimer_is_set(const ztimer_clock_t *clock, const ztimer_t *timer)
{
    unsigned state = irq_disable();
    unsigned res = _is_set(clock, timer);

    irq_restore(state);
    return res;
}

void ztimer_remove(ztimer_clock_t *clock, ztimer_t *timer)
{
    unsigned state = irq_disable();

    if (_is_set(clock, timer)) {
        ztimer_update_head_offset(clock);
        _del_entry_from_list(clock, &timer->base);

        _ztimer_update(clock);
    }

    irq_restore(state);
}

void ztimer_set(ztimer_clock_t *clock, ztimer_t *timer, uint32_t val)
{
    DEBUG("ztimer_set(): %p: set %p at %" PRIu32 " offset %" PRIu32 "\n",
          (void *)clock, (void *)timer, clock->ops->now(clock), val);

    unsigned state = irq_disable();

    ztimer_update_head_offset(clock);
    if (_is_set(clock, timer)) {
        _del_entry_from_list(clock, &timer->base);
    }

    /* optionally subtract a configurable adjustment value */
    if (val > clock->adjust_set) {
        val -= clock->adjust_set;
    }
    else {
        val = 0;
    }

    timer->base.offset = val;
    _add_entry_to_list(clock, &timer->base);
    if (clock->list.next == &timer->base) {
#ifdef MODULE_ZTIMER_EXTEND
        if (clock->max_value < UINT32_MAX) {
            val = _min_u32(val, clock->max_value >> 1);
        }
        DEBUG("ztimer_set(): %p setting %" PRIu32 "\n", (void *)clock, val);
#endif
        clock->ops->set(clock, val);
    }

    irq_restore(state);
}

static void _add_entry_to_list(ztimer_clock_t *clock, ztimer_base_t *entry)
{
    uint32_t delta_sum = 0;

    ztimer_base_t *list = &clock->list;

#ifdef MODULE_PM_LAYERED
    /* First timer on the clock's linked list */
    if (list->next == NULL &&
        clock->block_pm_mode != ZTIMER_CLOCK_NO_REQUIRED_PM_MODE) {
        pm_block(clock->block_pm_mode);
    }
#endif

    /* Jump past all entries which are set to an earlier target than the new entry */
    while (list->next) {
        ztimer_base_t *list_entry = list->next;
        if ((list_entry->offset + delta_sum) > entry->offset) {
            break;
        }
        delta_sum += list_entry->offset;
        list = list->next;
    }

    /* Insert into list */
    entry->next = list->next;
    entry->offset -= delta_sum;
    if (entry->next) {
        entry->next->offset -= entry->offset;
    }
    else {
        clock->last = entry;
    }
    list->next = entry;
    DEBUG("_add_entry_to_list() %p offset %" PRIu32 "\n", (void *)entry,
          entry->offset);

}

static uint32_t _add_modulo(uint32_t a, uint32_t b, uint32_t mod)
{
    if (a < b) {
        a += mod + 1;
    }
    return a - b;
}

#ifdef MODULE_ZTIMER_EXTEND
ztimer_now_t _ztimer_now_extend(ztimer_clock_t *clock)
{
    assert(clock->max_value);
    unsigned state = irq_disable();
    uint32_t lower_now = clock->ops->now(clock);

    DEBUG(
        "ztimer_now() checkpoint=%" PRIu32 " lower_last=%" PRIu32
        " lower_now=%" PRIu32 " diff=%" PRIu32 "\n",
        (uint32_t)clock->checkpoint, clock->lower_last, lower_now,
        _add_modulo(lower_now, clock->lower_last, clock->max_value));
    clock->checkpoint += _add_modulo(lower_now, clock->lower_last,
                                     clock->max_value);
    clock->lower_last = lower_now;
    DEBUG("ztimer_now() returning %" PRIu32 "\n", (uint32_t)clock->checkpoint);
    ztimer_now_t now = clock->checkpoint;

    irq_restore(state);
    return now;
}
#endif /* MODULE_ZTIMER_EXTEND */

void ztimer_update_head_offset(ztimer_clock_t *clock)
{
    uint32_t old_base = clock->list.offset;
    uint32_t now = ztimer_now(clock);
    uint32_t diff = now - old_base;

    ztimer_base_t *entry = clock->list.next;

    DEBUG(
        "clock %p: ztimer_update_head_offset(): diff=%" PRIu32 " old head %p\n",
        (void *)clock, diff, (void *)entry);
    if (entry) {
        do {
            if (diff <= entry->offset) {
                entry->offset -= diff;
                break;
            }
            else {
                diff -= entry->offset;
                entry->offset = 0;
                if (diff) {
                    /* skip timers with offset==0 */
                    do {
                        entry = entry->next;
                    } while (entry && (entry->offset == 0));
                }
            }
        } while (diff && entry);
        DEBUG(
            "ztimer %p: ztimer_update_head_offset(): now=%" PRIu32 " new head %p",
            (void *)clock, now, (void *)entry);
        if (entry) {
            DEBUG(" offset %" PRIu32 "\n", entry->offset);
        }
        else {
            DEBUG("\n");
        }
    }

    clock->list.offset = now;
}

static void _del_entry_from_list(ztimer_clock_t *clock, ztimer_base_t *entry)
{
    DEBUG("_del_entry_from_list()\n");
    ztimer_base_t *list = &clock->list;

    assert(_is_set(clock, (ztimer_t *)entry));

    while (list->next) {
        ztimer_base_t *list_entry = list->next;
        if (list_entry == entry) {
            if (entry == clock->last) {
                /* if entry was the last timer, set the clocks last to the
                 * previous entry, or NULL if that was the list ptr */
                clock->last = (list == &clock->list) ? NULL : list;
            }

            list->next = entry->next;
            if (list->next) {
                list_entry = list->next;
                list_entry->offset += entry->offset;
            }

            /* reset the entry's next pointer so _is_set() considers it unset */
            entry->next = NULL;
            break;
        }
        list = list->next;
    }

#ifdef MODULE_PM_LAYERED
    /* The last timer just got removed from the clock's linked list */
    if (clock->list.next == NULL &&
        clock->block_pm_mode != ZTIMER_CLOCK_NO_REQUIRED_PM_MODE) {
        pm_unblock(clock->block_pm_mode);
    }
#endif
}

static ztimer_t *_now_next(ztimer_clock_t *clock)
{
    ztimer_base_t *entry = clock->list.next;

    if (entry && (entry->offset == 0)) {
        clock->list.next = entry->next;
        if (!entry->next) {
            /* The last timer just got removed from the clock's linked list */
            clock->last = NULL;
#ifdef MODULE_PM_LAYERED
            if (clock->block_pm_mode != ZTIMER_CLOCK_NO_REQUIRED_PM_MODE) {
                pm_unblock(clock->block_pm_mode);
            }
#endif
        }
        return (ztimer_t *)entry;
    }
    else {
        return NULL;
    }
}

static void _ztimer_update(ztimer_clock_t *clock)
{
#ifdef MODULE_ZTIMER_EXTEND
    if (clock->max_value < UINT32_MAX) {
        if (clock->list.next) {
            clock->ops->set(clock,
                            _min_u32(clock->list.next->offset,
                                     clock->max_value >> 1));
        }
        else {
            clock->ops->set(clock, clock->max_value >> 1);
        }
#else
    if (0) {
#endif
    }
    else {
        if (clock->list.next) {
            clock->ops->set(clock, clock->list.next->offset);
        }
        else {
            if (IS_USED(MODULE_ZTIMER_NOW64)) {
                /* ensure there's at least one ISR per half period */
                clock->ops->set(clock, clock->max_value >> 1);
            }
            else {
                clock->ops->cancel(clock);
            }
        }
    }
}

void ztimer_handler(ztimer_clock_t *clock)
{
    DEBUG("ztimer_handler(): %p now=%" PRIu32 "\n", (void *)clock, clock->ops->now(
              clock));
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _ztimer_print(clock);
    }

#if MODULE_ZTIMER_EXTEND || MODULE_ZTIMER_NOW64
    if (IS_USED(MODULE_ZTIMER_NOW64) || clock->max_value < UINT32_MAX) {
        /* calling now triggers checkpointing */
        uint32_t now = ztimer_now(clock);

        if (clock->list.next) {
            uint32_t target = clock->list.offset + clock->list.next->offset;
            int32_t diff = (int32_t)(target - now);
            if (diff > 0) {
                DEBUG("ztimer_handler(): %p postponing by %" PRIi32 "\n",
                      (void *)clock, diff);
                clock->ops->set(clock, _min_u32(diff, clock->max_value >> 1));
                return;
            }
            else {
                DEBUG("ztimer_handler(): %p diff=%" PRIi32 "\n", (void *)clock,
                      diff);
            }
        }
        else {
            DEBUG("ztimer_handler(): %p intermediate\n", (void *)clock);
            clock->ops->set(clock, clock->max_value >> 1);
            return;
        }
    }
    else {
        DEBUG("ztimer_handler(): no checkpointing\n");
    }
#endif

    clock->list.offset += clock->list.next->offset;
    clock->list.next->offset = 0;

    ztimer_t *entry = _now_next(clock);
    while (entry) {
        DEBUG("ztimer_handler(): trigger %p->%p at %" PRIu32 "\n",
              (void *)entry, (void *)entry->base.next, clock->ops->now(clock));
        entry->callback(entry->arg);
        entry = _now_next(clock);
        if (!entry) {
            /* See if any more alarms expired during callback processing */
            /* This reduces the number of implicit calls to clock->ops->now() */
            ztimer_update_head_offset(clock);
            entry = _now_next(clock);
        }
    }

    _ztimer_update(clock);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _ztimer_print(clock);
    }
    DEBUG("ztimer_handler(): %p done.\n", (void *)clock);
    if (!irq_is_in()) {
        thread_yield_higher();
    }
}

static void _ztimer_print(const ztimer_clock_t *clock)
{
    const ztimer_base_t *entry = &clock->list;
    uint32_t last_offset = 0;

    do {
        printf("0x%08x:%" PRIu32 "(%" PRIu32 ")%s", (unsigned)entry,
               entry->offset, entry->offset +
               last_offset,
               entry->next ? "->" : (entry == clock->last ? "" : "!"));
        last_offset += entry->offset;

    } while ((entry = entry->next));
    puts("");
}
