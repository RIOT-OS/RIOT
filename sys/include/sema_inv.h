/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_sema_inv    inverse Semaphores
 * @ingroup     sys
 * @brief       Lightweight inverse semaphore implementation
 * @{
 *
 * @file
 * @brief   Inverse Semaphore definitions
 *
 *          Inverse Semaphores can be used to synchronize on multiple
 *          threads / objects.
 *
 *          The inverse semaphore can be used eiher in counter or in
 *          mask mode.
 *
 *          ### Counter Mode ###
 *
 *          In this mode the inverse Semaphore is initialized with a
 *          counter variable `n`.
 *          After `n` calls to @ref sema_inv_post, the waiting thread
 *          is unblocked.
 *
 *          ### Mask Mode ###
 *
 *          In this mode the inverse Semaphore is initialized with a
 *          bit mask `n`.
 *          A call to @ref sema_inv_post_mask clears one or multiple bits.
 *          Clearing the same bit multiple times has no effect.
 *          The thread is unblocked if all bits have been cleared.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef SEMA_INV_H
#define SEMA_INV_H

#include "atomic_utils.h"
#include "mutex.h"

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An Inverse Semaphore.
 */
typedef struct {
    uint32_t value; /**< value of the semaphore */
    mutex_t lock;   /**< mutex of the semaphore */
} sema_inv_t;

/**
 * @brief   Signal semaphore (counter mode).
 *
 *          Decrements the semaphore counter by one.
 *          If the counter reaches zero, the waiting thread is woken.
 *
 * @param   s       an inverse semaphore
 *
 * @retval  `true`  the value of the semaphore has reached zero
 *                  and the waiting thread has been woken
 * @retval  `false` the semaphore has not reached zero yet
 */
bool sema_inv_post(sema_inv_t *s);

/**
 * @brief   Signal semaphore (mask mode).
 *
 *          Clears the bits specified by @p mask from the semaphore value.
 *          If the value reaches zero, the waiting thread is woken.
 *
 * @param   s       an inverse semaphore
 * @param   mask    bit mask to clear from the semaphore value
 *
 * @retval  `true`  the value of the semaphore has reached zero
 *                  and the waiting thread has been woken
 * @retval  `false` the semaphore has not reached zero yet
 */
bool sema_inv_post_mask(sema_inv_t *s, uint32_t mask);

/**
 * @brief   Initialize an inverse semaphore
 *
 * @param   s       an inverse semaphore
 * @param   value   start value, either a counter or a bit mask
 */
static inline void sema_inv_init(sema_inv_t *s, uint32_t value)
{
    const mutex_t locked = MUTEX_INIT_LOCKED;
    s->lock = locked;
    s->value = value;
}

/**
 * @brief   Wait for the inverse semaphore value to reach zero.
 *
 * @param   s   An inverse semaphore.
 */
static inline void sema_inv_wait(sema_inv_t *s)
{
    mutex_lock(&s->lock);
}

/**
 * @brief   Check if the inverse semaphore value has reached zero.
 *
 * @param   s   An inverse semaphore.
 *
 * @return  1   if the semaphore value has reached zero
 *          0   otherwise
 */
static inline int sema_inv_try_wait(sema_inv_t *s)
{
    return mutex_trylock(&s->lock);
}

#if defined(MODULE_XTIMER) || DOXYGEN
/**
 * @brief   Wait for the inverse semaphore value to reach zero or
 *          a timeout being reached.
 *
 * @param   s   An inverse semaphore.
 * @param   us  Time in microseconds until the semaphore times out.
 *
 * @return  0   if the semaphore value has reached zero
 *         -1   when the timeout occurred
 */
static inline int sema_inv_wait_timeout(sema_inv_t *s, uint32_t us)
{
    return xtimer_mutex_lock_timeout(&s->lock, us);
}
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SEMA_INV_H */
