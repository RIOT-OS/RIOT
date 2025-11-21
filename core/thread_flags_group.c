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

 #include <stdbool.h>

#include "bitarithm.h"
#include "irq.h"
#include "thread.h"
#include "thread_flags_group.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void thread_flags_group_set(thread_flags_group_t *group, thread_flags_t mask)
{
    /* Interrupts must be disabled because the threads are not ordered by
     * priority. */
    unsigned irq_state = irq_disable();

    DEBUG("thread_flags_group_set(%p, %x):\n", (void *)group, (unsigned)mask);
    DEBUG("| TID | Flags | Status (old) | Status (new) |\n");

    bool yield = false;
    for (kernel_pid_t i = 0; i < (kernel_pid_t)ARRAY_SIZE(group->members); i++) {
        unsigned pid_block = group->members[i];
        kernel_pid_t const pid_base = i * UINT_WIDTH;
        uint8_t pid_offs = 0;

        while (pid_block) {
            pid_block = bitarithm_test_and_clear(pid_block, &pid_offs);
            kernel_pid_t target_pid = pid_base + pid_offs;
            thread_t *target = thread_get(target_pid);
            if (!target) {
                DEBUG("|  %02u |   n/a |   n/a (dead) |   n/a (dead) |\n",
                      target_pid);
                continue;
            }
            thread_status_t old_status = target->status;
            bool awoken = thread_flags_set_internal(target, mask);
            thread_status_t new_status = target->status;
            /* NOTE: wait_data is shared by thread_flags with other
             *       mechanisms and may e.g. be a pointer to an `msg_t`.
             *       Some interpretation of the output is needed by the
             *       user of the debug output. */
            thread_flags_t wait_data = (uint16_t)(uintptr_t)target->wait_data;
            DEBUG("|  %02u |  %04x | %12s | %12s |\n",
                  (unsigned)target_pid, (unsigned)wait_data,
                  thread_state_to_string(old_status),
                  thread_state_to_string(new_status));
            yield = yield || awoken;
        }
    }

    irq_restore(irq_state);
    if (yield) {
        thread_yield_higher();
    }
}
