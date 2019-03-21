/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Mock threading implementation
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "irq.h"
#include "periph/pm.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static uint8_t sleeping = 0;

int thread_getstatus(kernel_pid_t pid)
{
    if (pid == 0) {
        return (int)STATUS_RUNNING;
    }
    return (int)STATUS_NOT_FOUND;
}

const char *thread_getname(kernel_pid_t pid)
{
#ifdef DEVELHELP
    if (pid == 0) {
        return "main";
    }
#endif
    (void)pid;
    return NULL;
}

void thread_sleep(void)
{
    unsigned state = irq_disable();
    sleeping = 1;
    while (sleeping) {
        irq_restore(state);
        pm_set_lowest();
        state = irq_disable();
    }
    irq_restore(state);
}

int thread_wakeup(kernel_pid_t pid)
{
    if (pid != 0) {
        return (int)STATUS_NOT_FOUND;
    }
    unsigned old_state = irq_disable();

    sleeping = 0;

    irq_restore(old_state);
    return 1;
}

void thread_yield(void)
{
    /* nothing to do */
}
