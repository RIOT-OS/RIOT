/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mynewt_core
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
#include "os/os_sem.h"

os_error_t os_sem_init(struct os_sem *sem, uint16_t tokens)
{
    sema_create(&sem->sema, tokens);
    return OS_OK;
}

os_error_t os_sem_release(struct os_sem *sem)
{
    int ret = sema_post(&sem->sema);
    return (ret) ? OS_ERROR : OS_OK;
}

os_error_t os_sem_pend(struct os_sem *sem, os_time_t timeout)
{
    int ret = sema_wait_timed_ztimer(&sem->sema, ZTIMER_MSEC, timeout);
    return (ret) ? OS_ERROR : OS_OK;
}
