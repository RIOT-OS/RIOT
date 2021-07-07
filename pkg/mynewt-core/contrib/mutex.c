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
 * @brief       Decawave Porting Layer mutex RIOT wrapper
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "os/os_mutex.h"

os_error_t os_mutex_init(struct os_mutex *mu)
{
    if (!mu) {
        return OS_INVALID_PARM;
    }
    mutex_init(&mu->mutex);
    return OS_OK;
}

os_error_t os_mutex_release(struct os_mutex *mu)
{
    if (!mu) {
        return OS_INVALID_PARM;
    }

    mutex_unlock(&mu->mutex);
    return OS_OK;
}

os_error_t os_mutex_pend(struct os_mutex *mu, uint32_t timeout)
{
    int rc = OS_OK;

    if (!mu) {
        return OS_INVALID_PARM;
    }

    if (!timeout) {
        rc = mutex_trylock(&mu->mutex);
    }
    else {
        /* TODO: no timeout equivalent */
        mutex_lock(&mu->mutex);
    }
    return rc;
}
