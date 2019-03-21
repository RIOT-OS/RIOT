/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sched
 * @{
 *
 * @file
 * @brief       Scheduler implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>

#include "sched.h"
#include "thread.h"
#include "panic.h"

#include "log.h"

volatile int sched_num_threads = 1;
volatile unsigned int sched_context_switch_request = 0;
volatile thread_t *sched_active_thread = NULL;
volatile kernel_pid_t sched_active_pid = 0;

int sched_run(void)
{
    core_panic(PANIC_GENERAL_ERROR, "sched_run() called in single-threaded "
                                    "application. This should never happen.");
}

NORETURN void sched_task_exit(void)
{
    core_panic(PANIC_GENERAL_ERROR, "Only task in single-threaded application "
                                    "exited. This should never happen.");
}
