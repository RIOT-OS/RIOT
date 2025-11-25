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
 * @brief       uwb-core DPL (Decawave Porting Layer) semapahore wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_SEM_H
#define DPL_DPL_SEM_H

#include <stdint.h>

#include "dpl_types.h"
#include "dpl_error.h"
#include "os/os_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl semaphore wrapper
 */
struct dpl_sem {
    struct os_sem sem;      /**< the semaphore */
};

/**
 * @brief Initialize a semaphore
 *
 * @param[in]   sem     pointer to semaphore
 * @param[in]   tokens  # of tokens the semaphore should contain initially.
 *
 * @return dpl_error_t
 *      DPL_INVALID_PARM     Semaphore passed in was NULL.
 *      DPL_OK               no error.
 */
static inline dpl_error_t dpl_sem_init(struct dpl_sem *sem, uint16_t tokens)
{
    return (dpl_error_t) os_sem_init(&sem->sem, tokens);
}

/**
 * @brief Pend (wait) for a semaphore.
 *
 * @param[in]   sem     pointer to semaphore.
 * @param[in]   timeout timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of DPL_TIMEOUT_NEVER means wait forever.
 *
 *
 * @return dpl_error_t
 *      DPL_INVALID_PARM     semaphore passed in was NULL.
 *      DPL_TIMEOUT          semaphore was owned by another task and timeout=0
 *      DPL_OK               no error
 */
static inline dpl_error_t dpl_sem_pend(struct dpl_sem *sem, dpl_time_t timeout)
{
    return (dpl_error_t) os_sem_pend(&sem->sem, timeout);
}

/**
 * @brief Release a semaphore.
 *
 * @param[in]   sem     pointer to the semaphore to be released
 *
 * @return dpl_error_t
 *      DPL_INVALID_PARM    semaphore passed in was NULL.
 *      DPL_OK              no error
 */
static inline dpl_error_t dpl_sem_release(struct dpl_sem *sem)
{
    return (dpl_error_t) os_sem_release(&sem->sem);
}

/**
 * @brief Get current semaphore's count
 */
static inline int16_t dpl_sem_get_count(struct dpl_sem *sem)
{
    return os_sem_get_count(&sem->sem);
}

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_SEM_H */
