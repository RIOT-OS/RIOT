/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <errno.h>

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
    int ret;

    if (timeout == OS_TIMEOUT_NEVER) {
        ret = sema_wait(&sem->sema);
    }
    else {
        ret = sema_wait_timed_ztimer(&sem->sema, ZTIMER_MSEC, timeout);
    }
    return ret == 0 ? OS_OK : ret == -ETIMEDOUT ? OS_TIMEOUT: OS_ERROR;
}
