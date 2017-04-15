/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *               2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_sema Semaphores
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
 * @author  Víctor Ariño <victor.arino@zii.aero>
 */

#ifndef SEMA_H
#define SEMA_H

#include <stdint.h>

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Creates semaphore statically.
 *
 * @param[in] value Initial value for the semaphore (can't be 0). For a 0
 *                  initialized semaphore @see SEMA_CREATE_LOCKED
 *
 * @return  Statically initialized semaphore.
 */
#define SEMA_CREATE(value)         { (value), SEMA_OK, MUTEX_INIT }

/**
 * @brief   Creates semaphore statically initialized to 0
 * @return  Statically initialized semaphore.
 */
#define SEMA_CREATE_LOCKED()        { (0), SEMA_OK, MUTEX_INIT_LOCKED }

/**
 * @brief A Semaphore states.
 */
typedef enum {
    SEMA_OK = 0,
    SEMA_DESTROY,
} sema_state_t;

/**
 * @brief A Semaphore.
 */
typedef struct {
    unsigned int value;             /**< value of the semaphore */
    sema_state_t state;             /**< state of the semaphore */
    mutex_t mutex;                  /**< mutex of the semaphore */
} sema_t;

/**
 * @brief   Creates semaphore dynamically.
 *
 * @pre `(sema != NULL)`
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_init.html">
 *          The Open Group Base Specifications Issue 7, sem_init()
 *      </a> (without `pshared` parameter)
 *
 * @param[out] sema The created semaphore.
 * @param[in] value Initial value for the semaphore.
 */
void sema_create(sema_t *sema, unsigned int value);

/**
 * @brief   Destroys a semaphore.
 *
 * @pre `(sema != NULL)`
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_destroy.html">
 *          The Open Group Base Specifications Issue 7, sem_destroy()
 *      </a>
 *
 * Destroying a semaphore upon which other threads are currently blocked
 * will wake the other threads causing the @ref sema_wait (or
 * @ref sema_wait_timed) to return error (-ECANCELLED).
 *
 * @param[in] sema  The semaphore to destroy.
 */
void sema_destroy(sema_t *sema);

/**
 * @brief   Wait for a semaphore being posted.
 *
 * @pre `(sema != NULL)`
 *
 * @param[in]  sema     A semaphore.
 * @param[in]  timeout  Time in microseconds until the semaphore times out. 0 for no timeout.
 *
 * @return  0 on success
 * @return  -ETIMEDOUT, if the semaphore times out.
 * @return  -ECANCELED, if the semaphore was destroyed.
 */
int sema_wait_timed(sema_t *sema, uint64_t timeout);

/**
 * @brief   Wait for a semaphore being posted (without timeout).
 *
 * @pre `(sema != NULL)`
 *
 * @param[in]  sema A semaphore.
 *
 * @return  0 on success
 * @return  -ECANCELED, if the semaphore was destroyed.
 */
static inline int sema_wait(sema_t *sema)
{
    return sema_wait_timed(sema, 0);
}

/**
 * @brief   Test if the semaphore is posted
 *
 * @pre `(sema != NULL)`
 *
 * This is a non-blocking alternative to @ref sema_wait.
 *
 * @return 0 on success
 * @return  -EAGAIN, if the semaphore is not posted.
 * @return  -ECANCELED, if the semaphore was destroyed.
 */
int sema_try_wait(sema_t *sema);

/**
 * @brief   Signal semaphore.
 *
 * @pre `(sema != NULL)`
 *
 * @param[in] sema  A semaphore.
 *
 * @return  0, on success
 * @return  -EOVERFLOW, if the semaphore's value would overflow.
 */
int sema_post(sema_t *sema);

#ifdef __cplusplus
}
#endif

#endif /* SEMA_H */
/** @} */
