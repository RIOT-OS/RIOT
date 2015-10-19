/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    posix_semaphore POSIX semaphores
 * @ingroup     posix
 * @{
 * @file
 * @brief   Semaphores
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/semaphore.h.html">
 *              The Open Group Base Specifications Issue 7, <semaphore.h>
 *          </a>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 */

#ifndef POSIX_SEMAPHORE_H_
#define POSIX_SEMAPHORE_H_

#include <errno.h>
#include <time.h>

#include "sema.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Value returned if `sem_open' failed.
 */
#define SEM_FAILED      ((sem_t *) 0)

/**
 * @brief Initialize semaphore.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_init.html">
 *          The Open Group Base Specifications Issue 7, sem_init()
 *      </a>
 *
 * The sem_init() function shall initialize the unnamed semaphore referred to by @p sem. The value
 * of the initialized semaphore shall be @p value. Following a successful call to sem_init(),
 * the semaphore may be used in subsequent calls to sem_wait(), sem_timedwait(),
 * sem_trywait(), sem_post(), and sem_destroy(). This semaphore shall remain usable until the
 * semaphore is destroyed.
 *
 * @param[out] sem      Semaphore to initialize.
 * @param[in] pshared   **(unused, since RIOT only has threads)**
 *                      Semaphore is shared between processes not threads.
 * @param[in] value     Value to set.
 *
 * @return  0 on success.
 * @return  -EINVAL, if semaphore is invalid.
 */
#define sem_init(sem, pshared, value)   sem_create(sem, value)

/**
 * @brief Open a named semaphore @p name with open flags @p oflag.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_open.html">
 *          The Open Group Base Specifications Issue 7, sem_open()
 *      </a>
 *
 * @todo  named semaphore are currently not supported
 *
 * @param[in] name  Name to set.
 * @param[in] oflag Flags to set.
 *
 * @return  Always @ref SEM_FAILED, since it is not implemented currently.
 */
#define sem_open(name, oflag, ...)      (SEM_FAILED)

/**
 * @brief Close descriptor for named semaphore @p sem.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_close.html">
 *          The Open Group Base Specifications Issue 7, sem_close()
 *      </a>
 *
 * @todo  named semaphore are currently not supported
 *
 * @param[in] sem   Semaphore to close.
 *
 * @return  Always -1, since it is not implemented currently.
 */
#define sem_close(sem)                  (-1)

/**
 * @brief Remove named semaphore @p name.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_unlink.html">
 *          The Open Group Base Specifications Issue 7, sem_unlink()
 *      </a>
 *
 * @todo  named semaphore are currently not supported
 *
 * @param[in] name  Name to unlink.
 *
 * @return  Always -1, since it is not implemented currently.
 */
#define sem_unlink(name)                (-1)

/**
 * @brief Similar to `sem_wait' but wait only until @p abstime.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_timedwait.html">
 *          The Open Group Base Specifications Issue 7, sem_timedwait()
 *      </a>
 *
 * The sem_timedwait() function shall lock the semaphore referenced by @p sem as in the sem_wait()
 * function. However, if the semaphore cannot be locked without waiting for another process or
 * thread to unlock the semaphore by performing a sem_post() function, this wait shall be
 * terminated when the specified timeout expires.
 *
 * @param[in] sem       Semaphore to wait on.
 * @param[in] abstime   Absolute time (that is when the clock on which temouts are based equals
 *                      this value) the timeout for the wait shall expire. If the value specified
 *                      has already passed the timeout expires immediately.
 *
 * @return  0 on success
 * @return  -EINVAL, if semaphore is invalid.
 * @return  -ETIMEDOUT, if the semaphore times out.
 * @return  -ECANCELED, if the semaphore was destroyed.
 */
int sem_timedwait(sem_t *sem, const struct timespec *abstime);

/**
 * @brief Test whether SEM is posted.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_trywait.html">
 *          The Open Group Base Specifications Issue 7, sem_trywait()
 *      </a>
 *
 * @param[in] sem   Semaphore to try to wait on
 *
 * @return  0 on success
 * @return  -EINVAL, if semaphore is invalid.
 * @return  -EAGAIN, if the semaphore was already locked.
 */
int sem_trywait(sem_t *sem);

/**
 * @brief Get current value of SEM and store it in *SVAL.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_getvalue.html">
 *          The Open Group Base Specifications Issue 7, sem_getvalue()
 *      </a>
 *
 * @param[in] sem   Semaphore to get the value from.
 * @param[out] sval Place where value goes to.
 *
 * @return  0 on success
 * @return  -EINVAL, if semaphore is invalid.
 */
static inline int sem_getvalue(sem_t *sem, int *sval)
{
    if (sem != NULL) {
        *sval = (int)sem->value;
        return 0;
    }
    return -EINVAL;
}

#ifdef __cplusplus
}
#endif

#endif  /* POSIX_SEMAPHORE_H_ */
/** @} */
