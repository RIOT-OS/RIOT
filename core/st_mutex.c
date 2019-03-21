/*
 * Copyright (C) 2013 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sync
 * @{
 *
 * @file
 * @brief       Kernel mutex implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "mutex.h"
#include "irq.h"
#include "thread.h"
#include "sched.h"
#include "periph/pm.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int _mutex_lock(mutex_t *mutex, int blocking)
{
    unsigned irqstate = irq_disable();
    if (*mutex && !blocking) {
        irq_restore(irqstate);
        return 0;
    }

    while (*mutex) {
        irq_restore(irqstate);
        pm_set_lowest();
        irqstate = irq_disable();
    }

    *mutex = 1;
    irq_restore(irqstate);
    return 1;
}

void mutex_unlock(mutex_t *mutex)
{
    unsigned irqstate = irq_disable();
    *mutex = 0;
    irq_restore(irqstate);
}

void mutex_unlock_and_sleep(mutex_t *mutex)
{
    mutex_unlock(mutex);
    pm_set_lowest();
}
