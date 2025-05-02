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
        kernel_pid_t pid = i * TFG_INT_BITS;

        while (pid_block) {
            static_assert(sizeof(tfg_int_t) == sizeof(unsigned),
                          "sizeof(tfg_int_t) != sizeof(unsigned)");
            /* Just clearing the LSB is inefficient on architectures not
             * supporting LSB. Although bitarithm_lsb() is inlined, the compiler
             * isn't smart enough to understand that leading bits are always
             * cleared on each loop run, so it iterates over all leading zeroes.
             * Therefore we shift. */
            unsigned const bit = bitarithm_lsb((unsigned)pid_block);
            pid += bit;
            pid_block >>= bit + 1;
            thread_flags_set(thread_get(pid++), mask);
        }
    }

    irq_restore(irq_state);
}
