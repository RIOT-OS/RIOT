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

#include "bitarithm.h"
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
        tfg_int_t pid_block = group->members[i];
        while (pid_block) {
            unsigned const bit = bitarithm_lsb(pid_block);

            thread_t *thread = thread_get(i * TFG_INT_BITS + bit);
            assert(thread);
            thread_flags_set(thread, mask);

            pid_block &= ~(1 << bit);
        }
    }

    irq_restore(irq_state);
}
