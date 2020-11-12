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
 * @brief       uwb-core DPL (Decawave Porting Layer) mutex wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_MUTEX_H
#define DPL_DPL_MUTEX_H

#include "dpl_types.h"
#include "dpl_error.h"

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl mutex wrapper
 */
struct dpl_mutex {
    mutex_t mutex;  /**< the mutex */
};

/**
 * @brief Initializes a mutex object.
 *
 * @param[out]  mu  pre-allocated mutex structure, must not be NULL.
 */
dpl_error_t dpl_mutex_init(struct dpl_mutex *mu);

/**
 * @brief Pend (wait) for a mutex.
 *
 * @param[in]   mu      Pointer to mutex.
 * @param[in]   timeout Timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of OS_TIMEOUT_NEVER means wait forever.
 *
 * @return dpl_error_t
 *      DPL_INVALID_PARM    mutex passed in was NULL
 *      DPL_OK              no error
 */
dpl_error_t dpl_mutex_pend(struct dpl_mutex *mu, dpl_time_t timeout);

/**
 *
 * @brief Release a mutex.
 *
 * @return dpl_error_t
 *      DPL_INVALID_PARM    mutex was NULL
 *      DPL_OK              no error
 */
dpl_error_t dpl_mutex_release(struct dpl_mutex *mu);

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_MUTEX_H */
