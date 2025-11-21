/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core
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

void thread_flags_group_set(thread_flags_group_t *group, thread_flags_t mask)
{
    /* Interrupts must be disabled because the threads are not ordered by
     * priority. */
    unsigned irq_state = irq_disable();

    for (kernel_pid_t i = 0; i < (kernel_pid_t)ARRAY_SIZE(group->members); i++) {
        unsigned pid_block = group->members[i];
        kernel_pid_t const pid_base = i * UINT_WIDTH;
        uint8_t pid_offs = 0;

        while (pid_block) {
            pid_block = bitarithm_test_and_clear(pid_block, &pid_offs);
            thread_flags_set(thread_get(pid_base + pid_offs), mask);
        }
    }

    irq_restore(irq_state);
}
