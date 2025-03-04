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

void thread_flags_group_set(tfg_t *group, thread_flags_t mask)
{
    /* Interrupts must be disabled because the threads are not ordered by
     * priority. */
    int irq_state = irq_disable();

    for (unsigned i = 0; i < ARRAY_SIZE(group->members); i++) {
        /* Make a PID block copy to prevent unnecessary value reloading. */
        uint8_t pid_block = group->members[i];
        if (!pid_block) {
            continue;
        }

        for (unsigned j = 0; j < 8; j++) {
            if (pid_block & (1 << j)) {
                thread_t *thread = thread_get(i * 8 + j);
                assert(thread);
                thread_flags_set(thread, mask);
            }
        }
    }

    irq_restore(irq_state);
}
