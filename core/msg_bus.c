/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core_msg
 *
 * @{
 *
 * @file
 * @brief       Messaging Bus API for inter process message broadcast.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "irq.h"
#include "msg_bus.h"
#include "thread.h"

void msg_bus_init(msg_bus_t *bus)
{
    static uint16_t bus_count;

    bus->subs.next = NULL;
    bus->id = bus_count++;
}

void msg_bus_attach(msg_bus_t *bus, msg_bus_entry_t *entry)
{
    unsigned state;

    entry->next.next = NULL;
    entry->event_mask = 0;
    entry->pid = thread_getpid();

    state = irq_disable();
    list_add(&bus->subs, &entry->next);
    irq_restore(state);
}

void msg_bus_detach(msg_bus_t *bus, msg_bus_entry_t *entry)
{
    unsigned state;

    state = irq_disable();
    list_remove(&bus->subs, &entry->next);
    irq_restore(state);
}

msg_bus_entry_t *msg_bus_get_entry(msg_bus_t *bus)
{
    msg_bus_entry_t *s = NULL;
    unsigned state = irq_disable();

    for (list_node_t *e = bus->subs.next; e; e = e->next) {

        msg_bus_entry_t *subscriber = container_of(e, msg_bus_entry_t, next);

        if (subscriber->pid == thread_getpid()) {
            s = subscriber;
            break;
        }
    }

    irq_restore(state);
    return s;
}
