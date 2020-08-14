/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       Decawave Porting Layer semaphore RIOT wrapper
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdio.h>

#include "irq.h"
#include "dpl/dpl_sem.h"

dpl_error_t dpl_sem_init(struct dpl_sem *sem, uint16_t tokens)
{
    if (!sem) {
        return DPL_INVALID_PARAM;
    }

    sema_create(&sem->sema, tokens);
    return DPL_OK;
}

dpl_error_t dpl_sem_release(struct dpl_sem *sem)
{
    int ret;

    if (!sem) {
        return DPL_INVALID_PARAM;
    }

    ret = sema_post(&sem->sema);

    return (ret) ? DPL_ERROR : DPL_OK;
}

uint16_t dpl_sem_get_count(struct dpl_sem *sem)
{
    unsigned state = irq_disable();
    unsigned int value = sem->sema.value;
    irq_restore(state);
    return value;
}

dpl_error_t dpl_sem_pend(struct dpl_sem *sem, dpl_time_t timeout)
{
    int ret = sema_wait_timed(&sem->sema, timeout);
    return (ret) ? DPL_ERROR : DPL_OK;
}
