/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       LibCSP semaphore implementation
 *
 * @author      Pierre Millot <pierre.millot@grenoble-inp.org>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "sema.h"
#include "ztimer.h"

#include "csp_types_riot.h"
#include "csp_semaphore.h"

void csp_bin_sem_init(csp_bin_sem_t *sem)
{
    sema_create(sem, 1);
}

int csp_bin_sem_wait(csp_bin_sem_t *sem, unsigned int timeout)
{
    return sema_wait_timed_ztimer(sem, ZTIMER_MSEC,
                                  timeout) == SEMA_OK ? CSP_SEMAPHORE_OK : CSP_SEMAPHORE_ERROR;
}

int csp_bin_sem_post(csp_bin_sem_t *sem)
{
    return sema_post(sem) == SEMA_OK ? CSP_SEMAPHORE_OK : CSP_SEMAPHORE_ERROR;
}

int csp_bin_sem_post_isr(csp_bin_sem_t *sem, int *unused)
{
    (void)unused;
    return sema_post(sem) == SEMA_OK ? CSP_SEMAPHORE_OK : CSP_SEMAPHORE_ERROR;
}
