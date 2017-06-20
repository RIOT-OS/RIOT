/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_evtimer
 * @{
 *
 * @file
 * @brief       event timer implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include "div.h"
#include "irq.h"
#include "xtimer.h"

#include "evtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* XXX this function is intentionally non-static, since the optimizer can't
 * handle the pointer hack in this function */
void evtimer_add_event_to_list(evtimer_t *evtimer, evtimer_event_t *event)
{
    uint32_t delta_sum = 0;

    /* we want list->next to point to the first list element. thus we take the
     * *address* of evtimer->events, then cast it from (evtimer_event_t **) to
     * (evtimer_event_t*). After that, list->next actually equals
     * evtimer->events. */
    evtimer_event_t *list = (evtimer_event_t *)&evtimer->events;

    while (list->next) {
        evtimer_event_t *list_entry = list->next;
        if ((list_entry->offset + delta_sum) > event->offset) {
            break;
        }
        delta_sum += list_entry->offset;
        list = list->next;
    }

    event->next = list->next;
    if (list->next) {
        evtimer_event_t *next_entry = list->next;
        next_entry->offset += delta_sum;
        next_entry->offset -= event->offset;
    }
    event->offset -= delta_sum;

    list->next = event;
}

static void _del_event_from_list(evtimer_t *evtimer, evtimer_event_t *event)
{
    evtimer_event_t *list = (evtimer_event_t *) &evtimer->events;

    while (list->next) {
        evtimer_event_t *list_entry = list->next;
        if (list_entry == event) {
            list->next = event->next;
            if (list->next) {
                list_entry = list->next;
                list_entry->offset += event->offset;
            }
            break;
        }
        list = list->next;
    }
}

static void _set_timer(xtimer_t *timer, uint32_t offset)
{
    uint64_t offset_in_us = (uint64_t)offset * 1000;

    DEBUG("evtimer: now=%" PRIu32 " setting xtimer to %" PRIu32 ":%" PRIu32 "\n",
          xtimer_now_usec(), (uint32_t)(offset_in_us >> 32),
          (uint32_t)(offset_in_us));
    _xtimer_set64(timer, offset_in_us, offset_in_us >> 32);
}

static void _update_timer(evtimer_t *evtimer)
{
    if (evtimer->events) {
        evtimer_event_t *event = evtimer->events;
        _set_timer(&evtimer->timer, event->offset);
    }
    else {
        xtimer_remove(&evtimer->timer);
    }
}

static uint32_t _get_offset(xtimer_t *timer)
{
    uint64_t now = xtimer_now_usec64();
    uint64_t target = ((uint64_t)timer->long_target) << 32 | timer->target;

    if (target <= now) {
        return 0;
    }
    else {
        target -= now;
        /* add half of 125 so integer division rounds to nearest */
        return div_u64_by_125((target >> 3) + 62);
    }
}

static void _update_head_offset(evtimer_t *evtimer)
{
    if (evtimer->events) {
        evtimer_event_t *event = evtimer->events;
        event->offset = _get_offset(&evtimer->timer);
        DEBUG("evtimer: _update_head_offset(): new head offset %" PRIu32 "\n", event->offset);
    }
}

void evtimer_add(evtimer_t *evtimer, evtimer_event_t *event)
{
    unsigned state = irq_disable();

    DEBUG("evtimer_add(): adding event with offset %" PRIu32 "\n", event->offset);

    _update_head_offset(evtimer);
    evtimer_add_event_to_list(evtimer, event);
    if (evtimer->events == event) {
        _set_timer(&evtimer->timer, event->offset);
    }
    irq_restore(state);
    if (sched_context_switch_request) {
        thread_yield_higher();
    }
}

void evtimer_del(evtimer_t *evtimer, evtimer_event_t *event)
{
    unsigned state = irq_disable();

    DEBUG("evtimer_del(): removing event with offset %" PRIu32 "\n", event->offset);

    _update_head_offset(evtimer);
    _del_event_from_list(evtimer, event);
    _update_timer(evtimer);
    irq_restore(state);
}

static evtimer_event_t *_get_next(evtimer_t *evtimer)
{
    evtimer_event_t *event = evtimer->events;

    if (event && (event->offset == 0)) {
        evtimer->events = event->next;
        return event;
    }
    else {
        return NULL;
    }
}

static void _evtimer_handler(void *arg)
{
    DEBUG("_evtimer_handler()\n");

    evtimer_t *evtimer = (evtimer_t *)arg;

    /* this function gets called directly by xtimer if the set xtimer expired.
     * Thus the offset of the first event is down to zero. */
    evtimer_event_t *event = evtimer->events;
    event->offset = 0;

    /* iterate the event list */
    while ((event = _get_next(evtimer))) {
        evtimer->callback(event);
    }

    _update_timer(evtimer);
}

void evtimer_init(evtimer_t *evtimer, evtimer_callback_t handler)
{
    evtimer->callback = handler;
    evtimer->timer.callback = _evtimer_handler;
    evtimer->timer.arg = (void *)evtimer;
    evtimer->events = NULL;
}

void evtimer_print(const evtimer_t *evtimer)
{
    evtimer_event_t *list = evtimer->events;

    while (list->next) {
        evtimer_event_t *list_entry = list->next;
        printf("ev offset=%u\n", (unsigned)list_entry->offset);
        list = list->next;
    }
}
