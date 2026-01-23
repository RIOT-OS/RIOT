/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       OpenWSN scheduler mock
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include "scheduler.h"

scheduler_vars_t scheduler_mock_vars;

void scheduler_init(void)
{
    /* nothing to do here */
   scheduler_mock_vars.last_task = TASKPRIO_NONE;
}

void scheduler_start(unsigned state)
{
    irq_restore(state);
}

void scheduler_push_task(task_cbt cb, task_prio_t prio)
{
    (void) cb;
    scheduler_mock_vars.last_task = prio;
}
