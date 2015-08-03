/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_sem Semaphores
 * @ingroup     sys
 * @brief       Lightweight semaphore implementation
 * @{
 *
 * @file
 * @brief   Semaphore definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 */
#ifndef SEM_H_
#define SEM_H_

#include "priority_queue.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A Semaphore.
 */
typedef struct {
    volatile unsigned int value;    /**< value of the semaphore */
    priority_queue_t queue;         /**< list of threads waiting for the semaphore */
} sem_t;

/**
 * @brief   Creates semaphore.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_init.html">
 *          The Open Group Base Specifications Issue 7, sem_init()
 *      </a> (without `pshared` parameter)
 *
 * @param[out] sem  The created semaphore.
 * @param[in] value Initial value for the semaphore.
 *
 * @return  0 on success.
 * @return  -EINVAL, if semaphore is invalid.
 */
int sem_create(sem_t *sem, unsigned int value);

/**
 * @brief   Destroys a semaphore.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_destroy.html">
 *          The Open Group Base Specifications Issue 7, sem_destroy()
 *      </a>
 *
 * @param[in] sem   The semaphore to destroy.
 *
 * @return  0 on success.
 * @return  -EINVAL, if semaphore is invalid.
 */
int sem_destroy(sem_t *sem);

/**
 * @brief   Wait for a semaphore being posted.
 *
 * @pre Message queue of active thread is initialized (see @ref msg_init_queue()).
 *
 * @param[in]  sem      A semaphore.
 * @param[in]  timeout  Time until the semaphore times out. NULL for no timeout.
 * @param[out] msg      Container for a spurious message during the timed wait (result == -EAGAIN).
 *
 * @return  0 on success
 * @return  -EINVAL, if semaphore is invalid.
 * @return  -ETIMEDOUT, if the semaphore times out.
 * @return  -ECANCELED, if the semaphore was destroyed.
 * @return  -EAGAIN, if the thread received a message while waiting for the lock.
 */
int sem_wait_timed_msg(sem_t *sem, timex_t *timeout, msg_t *msg);

/**
 * @brief   Wait for a semaphore being posted (without timeout).
 *
 * @param[in]  sem  A semaphore.
 * @param[out] msg  Container for a spurious message during the timed wait (result == -EAGAIN).
 *
 * @return  0 on success
 * @return  -EINVAL, if semaphore is invalid.
 * @return  -ECANCELED, if the semaphore was destroyed.
 * @return  -EAGAIN, if the thread received a message while waiting for the lock.
 */
static inline int sem_wait_msg(sem_t *sem, msg_t *msg)
{
    return sem_wait_timed_msg(sem, NULL, msg);
}

/**
 * @brief   Wait for a semaphore being posted (dropping spurious messages).
 * @details Any spurious messages received while waiting for the semaphore are silently dropped.
 *
 * @param[in]  sem      A semaphore.
 * @param[in]  timeout  Time until the semaphore times out. NULL for no timeout.
 *
 * @return  0 on success
 * @return  -EINVAL, if semaphore is invalid.
 * @return  -ETIMEDOUT, if the semaphore times out.
 * @return  -ECANCELED, if the semaphore was destroyed.
 */
int sem_wait_timed(sem_t *sem, timex_t *timeout);

/**
 * @brief   Wait for a semaphore being posted (without timeout, dropping spurious messages).
 *
 * @param[in]  sem  A semaphore.
 *
 * @return  0 on success
 * @return  -EINVAL, if semaphore is invalid.
 * @return  -ECANCELED, if the semaphore was destroyed.
 */
static inline int sem_wait(sem_t *sem)
{
    return sem_wait_timed(sem, NULL);
}

/**
 * @brief   Signal semaphore.
 *
 * @param[in] sem   A semaphore.
 *
 * @return  -EINVAL, if semaphore is invalid.
 * @return  -EOVERFLOW, if the semaphore's value would overflow.
 */
int sem_post(sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif /* SEM_H_ */
/** @} */
