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
 * @brief       Decawave Porting Layer mutex RIOT wrapper
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "mutex.h"
#include "dpl/dpl_mutex.h"

dpl_error_t dpl_mutex_init(struct dpl_mutex *mu)
{
    if (!mu) {
        return DPL_INVALID_PARAM;
    }
    mutex_init(&mu->mutex);
    return DPL_OK;
}

dpl_error_t dpl_mutex_release(struct dpl_mutex *mu)
{
    if (!mu) {
        return DPL_INVALID_PARAM;
    }

    mutex_unlock(&mu->mutex);
    return DPL_OK;
}

dpl_error_t dpl_mutex_pend(struct dpl_mutex *mu, uint32_t timeout)
{
    int rc = DPL_OK;

    if (!mu) {
        return DPL_INVALID_PARAM;
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
