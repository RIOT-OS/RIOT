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
#if MODULE_PM_LAYERED && !MODULE_ZTIMER_ONDEMAND
#include "pm_layered.h"
#endif
#include "ztimer.h"
#include "log.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _add_entry_to_list(ztimer_clock_t *clock, ztimer_base_t *entry);
static bool _del_entry_from_list(ztimer_clock_t *clock, ztimer_base_t *entry);
static void _ztimer_update(ztimer_clock_t *clock);
static void _ztimer_print(const ztimer_clock_t *clock);
static uint32_t _ztimer_update_head_offset(ztimer_clock_t *clock);

#ifdef MODULE_ZTIMER_EXTEND
static inline uint32_t _min_u32(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}
#endif

#if MODULE_ZTIMER_ONDEMAND
static bool _ztimer_acquire(ztimer_clock_t *clock)
{
    bool first_clock_user = false;
    unsigned state = irq_disable();

    DEBUG("ztimer_acquire(): %p: %" PRIu16 " user(s)\n",
          (void *)clock, (uint16_t)(clock->users + 1));

    if (clock->users++ == 0) {
        if (clock->ops->start) {
            clock->ops->start(clock);
        }

        first_clock_user = true;
    }

    irq_restore(state);

    return first_clock_user;
}

bool ztimer_acquire(ztimer_clock_t *clock)
{
    bool first_clock_user = _ztimer_acquire(clock);

    if (first_clock_user) {
        /* if the clock just has been enabled, make sure to set possibly
         * required checkpoints for clock extension */
        _ztimer_update(clock);
    }

    return first_clock_user;
}

bool ztimer_release(ztimer_clock_t *clock)
{
    bool no_clock_user_left = false;
    unsigned state = irq_disable();

    assert(clock->users > 0);

    DEBUG("ztimer_release(): %p: %" PRIu16 " user(s)\n",
          (void *)clock, (uint16_t)(clock->users - 1));

    if (--clock->users == 0) {
        /* make sure the timer isn't armed before turning off */
        clock->ops->cancel(clock);

        if (clock->ops->stop) {
            clock->ops->stop(clock);
        }

        no_clock_user_left = true;
    }

    irq_restore(state);

    return no_clock_user_left;
}
#endif /* MODULE_ZTIMER_ONDEMAND */

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

bool ztimer_remove(ztimer_clock_t *clock, ztimer_t *timer)
{
    bool was_removed = false;
    bool no_clock_user_left = false;
    unsigned state = irq_disable();

    if (_is_set(clock, timer)) {
        _ztimer_update_head_offset(clock);
        was_removed = _del_entry_from_list(clock, &timer->base);

#if MODULE_ZTIMER_ONDEMAND
        if (was_removed) {
            no_clock_user_left = ztimer_release(clock);
        }
#endif

        if (!no_clock_user_left) {
            _ztimer_update(clock);
        }
    }

    irq_restore(state);
    return was_removed;
}

uint32_t ztimer_set(ztimer_clock_t *clock, ztimer_t *timer, uint32_t val)
{
    unsigned state = irq_disable();

#if MODULE_ZTIMER_ONDEMAND
    /* warm up our clock ... */
    if (_ztimer_acquire(clock) == true) {
        /* compensate delay that turning on the clock has introduced */
        if (val > clock->adjust_clock_start) {
            val -= clock->adjust_clock_start;
        }
        else {
            val = 0;
        }
    }
#endif

    DEBUG("ztimer_set(): %p: set %p at %" PRIu32 " offset %" PRIu32 "\n",
          (void *)clock, (void *)timer, clock->ops->now(clock), val);

    uint32_t now = _ztimer_update_head_offset(clock);

    bool was_set = false;
    if (_is_set(clock, timer)) {
        was_set = _del_entry_from_list(clock, &timer->base);
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

    /* the clock is armed now
     * everything down below doesn't impact timing */

#if MODULE_ZTIMER_ONDEMAND
    if (was_set) {
        /* the given ztimer_t was set in the past
         * remove the previously set instance */
        ztimer_release(clock);
    }
#else
    (void)was_set;
#endif

    return now;
}

static void _add_entry_to_list(ztimer_clock_t *clock, ztimer_base_t *entry)
{
    uint32_t delta_sum = 0;

    ztimer_base_t *list = &clock->list;

#if MODULE_PM_LAYERED && !MODULE_ZTIMER_ONDEMAND
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

static uint32_t _ztimer_update_head_offset(ztimer_clock_t *clock)
{
    uint32_t old_base = clock->list.offset;
    uint32_t now = ztimer_now(clock);
    uint32_t diff = now - old_base;

    ztimer_base_t *entry = clock->list.next;

    DEBUG(
        "clock %p: _ztimer_update_head_offset(): diff=%" PRIu32 " old head %p\n",
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
            "ztimer %p: _ztimer_update_head_offset(): now=%" PRIu32 " new head %p",
            (void *)clock, now, (void *)entry);
        if (entry) {
            DEBUG(" offset %" PRIu32 "\n", entry->offset);
        }
        else {
            DEBUG("\n");
        }
    }

    clock->list.offset = now;
    return now;
}

static bool _del_entry_from_list(ztimer_clock_t *clock, ztimer_base_t *entry)
{
    bool was_removed = false;

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

            was_removed = true;
            /* reset the entry's next pointer so _is_set() considers it unset */
            entry->next = NULL;
            break;
        }
        list = list->next;
    }

#if MODULE_PM_LAYERED && !MODULE_ZTIMER_ONDEMAND
    /* The last timer just got removed from the clock's linked list */
    if (clock->list.next == NULL &&
        clock->block_pm_mode != ZTIMER_CLOCK_NO_REQUIRED_PM_MODE) {
        pm_unblock(clock->block_pm_mode);
    }
#endif

    return was_removed;
}

static ztimer_t *_now_next(ztimer_clock_t *clock)
{
    ztimer_base_t *entry = clock->list.next;

    if (entry && (entry->offset == 0)) {
        clock->list.next = entry->next;
        if (!entry->next) {
            /* The last timer just got removed from the clock's linked list */
            clock->last = NULL;
#if MODULE_PM_LAYERED && !MODULE_ZTIMER_ONDEMAND
            if (clock->block_pm_mode != ZTIMER_CLOCK_NO_REQUIRED_PM_MODE) {
                pm_unblock(clock->block_pm_mode);
            }
#endif
        }
        else {
            /* reset next pointer so ztimer_is_set() works */
            entry->next = NULL;
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
            clock->ops->cancel(clock);
        }
    }
}

void ztimer_handler(ztimer_clock_t *clock)
{
    bool no_clock_user_left = false;

    DEBUG("ztimer_handler(): %p now=%" PRIu32 "\n", (void *)clock, clock->ops->now(
              clock));
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _ztimer_print(clock);
    }

#if MODULE_ZTIMER_EXTEND
    if (clock->max_value < UINT32_MAX) {
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

    if (clock->list.next) {
        _ztimer_update_head_offset(clock);

        ztimer_t *entry = _now_next(clock);

        if (!entry && IS_ACTIVE(ENABLE_DEBUG))
        {
            DEBUG("ztimer_handler(): called %" PRIu32 " ticks too early\n"
                , clock->list.next->offset
                );
        }

        while (entry) {
            DEBUG("ztimer_handler(): trigger %p->%p at %" PRIu32 "\n",
                  (void *)entry, (void *)entry->base.next, clock->ops->now(
                      clock));
            entry->callback(entry->arg);
#if MODULE_ZTIMER_ONDEMAND
            no_clock_user_left = ztimer_release(clock);
            if (no_clock_user_left) {
                break;
            }
#endif
            entry = _now_next(clock);
            if (!entry) {
                /* See if any more alarms expired during callback processing */
                /* This reduces the number of implicit calls to clock->ops->now() */
                _ztimer_update_head_offset(clock);
                entry = _now_next(clock);
            }
        }
    }

    /* only arm the clock if there are users left requiring the clock */
    if (!no_clock_user_left) {
        _ztimer_update(clock);
    }

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
        printf("0x%08" PRIxPTR ":%" PRIu32 "(%" PRIu32 ")%s", (uintptr_t)entry,
               entry->offset, entry->offset +
               last_offset,
               entry->next ? "->" : (entry == clock->last ? "" : "!"));
        last_offset += entry->offset;

    } while ((entry = entry->next));
    puts("");
}

#if MODULE_ZTIMER_ONDEMAND && DEVELHELP
void _ztimer_assert_clock_active(ztimer_clock_t *clock)
{
    if (clock->users == 0) {
        LOG_WARNING("WARNING! You are accessing ztimer_now() on a non-active clock!\n"
                    "         Make sure to call ztimer_acquire() before accessing ztimer_now().\n"
                    "         Once you've finished don't forget to call ztimer_release().\n");
    }
#if MODULE_ZTIMER_ONDEMAND_STRICT
    assert(clock->users > 0);
#endif
}
#endif
