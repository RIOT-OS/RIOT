/*
 * Copyright (C) 2025 Mihai Renea <mihai.renea@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_thread
 * @{
 *
 * @file
 * @brief       thread flags group implementation
 *
 * @author      Mihai Renea <mihai.renea@ml-pa.com>
 *
 * @}
 */

#include "irq.h"
#include "thread.h"
#include "thread_flags_group.h"

void thread_flags_group_join(tfg_t *group, tfg_entry_t *entry)
{
    int irq_state = irq_disable();

    entry->thread = thread_get_active();
    list_add(&group->list, &entry->node);

    irq_restore(irq_state);
}

void thread_flags_group_leave(tfg_t *group, tfg_entry_t *entry)
{
    int irq_state = irq_disable();

    list_remove(&group->list, &entry->node);

    irq_restore(irq_state);
}

void thread_flags_group_set(tfg_t *group, thread_flags_t mask)
{
    int irq_state = irq_disable();

    /* Interrupts must be disabled because the threads are not ordered by
     * priority. */
    for (tfg_entry_t *entry = (tfg_entry_t *)group->list.next; entry;
         entry = (tfg_entry_t *)entry->node.next) {
        thread_flags_set(entry->thread, mask);
    }

    irq_restore(irq_state);
}
