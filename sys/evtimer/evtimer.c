/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Freie Universität Berlin
 *               2018 Josua Arndt
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
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */

#include "div.h"
#include "irq.h"

#include "evtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _add_event_to_list(evtimer_t *evtimer, evtimer_event_t *event)
{
    DEBUG("evtimer: new event offset %" PRIu32 " ms\n", event->offset);
    evtimer_event_t **list = &evtimer->events;

    while (*list) {
        /* Stop when new event time is nearer then next */
        if (event->offset < (*list)->offset) {
            DEBUG("evtimer: next %" PRIu32 " < %" PRIu32 " ms\n",
                  event->offset, (*list)->offset);
            break;
        }
        /* Set event offset relative to previous event */
        event->offset -= (*list)->offset;
        list = &(*list)->next;
    }

    DEBUG("evtimer: new event relative offset %" PRIu32 " ms\n", event->offset);

    /* Set found next bigger event after new event */
    event->next = *list;
    if (*list) {
        /* Set offset following event relative to new event */
        DEBUG("evtimer: recalculate offset for %" PRIu32 " ms\n",
              (*list)->offset);

        (*list)->offset -= event->offset;

        DEBUG("evtimer: resulting new event offset %" PRIu32 " ms\n",
              (*list)->offset);
    }

    *list = event;
}

static void _del_event_from_list(evtimer_t *evtimer, evtimer_event_t *event)
{
    evtimer_event_t **list = &evtimer->events;

    /* Find the entry to delete from the list */
    while (*list) {
        if (*list == event) {
            break;
        }
        list = &(*list)->next;
    }

    if (*list) {
        /* This is the entry we want to remove */
        *list = (*list)->next;
        /* If the deleted entry was _not_ the last one
         * then update 'offset' of the entry that
         * followed.
         */
        if (*list) {
            (*list)->offset += event->offset;
        }
    }
}

static void _set_timer(evtimer_t *evtimer)
{
    evtimer_event_t *next_event = evtimer->events;

#if IS_USED(MODULE_EVTIMER_ON_ZTIMER)
    evtimer->base = ztimer_set(ZTIMER_MSEC, &evtimer->timer, next_event->offset);
    DEBUG("evtimer: now=%" PRIu32 " ms setting ztimer to %" PRIu32 " ms\n",
          evtimer->base, next_event->offset);
#else
    uint64_t offset_us = (uint64_t)next_event->offset * US_PER_MS;

    DEBUG("evtimer: now=%" PRIu32 " us setting xtimer to %" PRIu32 ":%" PRIu32 " us\n",
          xtimer_now_usec(), (uint32_t)(offset_us >> 32), (uint32_t)(offset_us));

    xtimer_set64(&evtimer->timer, offset_us);
#endif
}

static void _update_timer(evtimer_t *evtimer)
{
    if (evtimer->events) {
        _set_timer(evtimer);
    }
    else {
#if IS_USED(MODULE_EVTIMER_ON_ZTIMER)
        ztimer_remove(ZTIMER_MSEC, &evtimer->timer);
#else
        xtimer_remove(&evtimer->timer);
#endif
    }
}

#if IS_USED(MODULE_EVTIMER_ON_ZTIMER)
static void _update_head_offset(evtimer_t *evtimer)
{
    if (evtimer->events) {
        evtimer_event_t *event = evtimer->events;
        uint32_t now = ztimer_now(ZTIMER_MSEC);
        uint32_t elapsed = now - evtimer->base;
        if (elapsed > event->offset) {
            event->offset = 0;
        } else {
            event->offset -= elapsed;
        }
        evtimer->base = now;
    }
}
#else /* IS_USED(MODULE_EVTIMER_ON_ZTIMER) */
static uint32_t _get_offset(xtimer_t *timer)
{
    uint64_t left = xtimer_left_usec(timer);
    /* add half of 125 so integer division rounds to nearest */
    return div_u64_by_125((left >> 3) + 62);
}

static void _update_head_offset(evtimer_t *evtimer)
{
    if (evtimer->events) {
        evtimer_event_t *event = evtimer->events;
        event->offset = _get_offset(&evtimer->timer);
        DEBUG("evtimer: _update_head_offset(): new head offset %" PRIu32 "\n", event->offset);
    }
}
#endif /* !IS_USED(MODULE_EVTIMER_ON_ZTIMER) */

void evtimer_add(evtimer_t *evtimer, evtimer_event_t *event)
{
    unsigned state = irq_disable();

    DEBUG("evtimer_add(): adding event with offset %" PRIu32 "\n", event->offset);

    _update_head_offset(evtimer);
    _add_event_to_list(evtimer, event);
    if (evtimer->events == event) {
        _set_timer(evtimer);
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
    int nr = 0;

    while (list) {
        nr++;
        printf("ev #%d offset=%u\n", nr, (unsigned)list->offset);
        list = list->next;
    }
}
