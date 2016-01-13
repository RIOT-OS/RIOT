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

typedef sema_t sem_t;

/**
 * @brief Value returned if `sem_open' failed.
 */
#define SEM_FAILED      ((sem_t *) 0)

/**
 * @brief Initialize an unnamed semaphore.
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
 * @return  -1, on error and errno set to indicate the error.
 */
static inline int sem_init(sem_t *sem, int pshared, unsigned value)
{
    int res = sema_create((sema_t *)sem, value);
    (void)pshared;
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return 0;
}

/**
 * @brief destroy an unnamed semaphore
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_destroy.html">
 *          The Open Group Base Specifications Issue 7, sem_destroy()
 *      </a>
 *
 * The sem_destroy() function shall destroy the unnamed semaphore indicated by @p sem. Only a
 * semaphore that was created using sem_init() may be destroyed using sem_destroy(); the effect of
 * calling sem_destroy() with a named semaphore is undefined. The effect of subsequent use of the
 * semaphore @p sem is undefined until sem is reinitialized by another call to sem_init().
 *
 * It is safe to destroy an initialized semaphore upon which no threads are currently blocked.
 * The effect of destroying a semaphore upon which other threads are currently blocked is
 * undefined.
 *
 * @param sem   A semaphore.
 *
 * @return  0 on success.
 * @return  -1, on error and errno set to indicate the error.
 */
static inline int sem_destroy(sem_t *sem)
{
    int res = sema_destroy((sema_t *)sem);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return 0;
}

/**
 * @brief Unlock a semaphore.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_post.html">
 *          The Open Group Base Specifications Issue 7, sem_post()
 *      </a>
 *
 * The sem_post() function shall unlock the semaphore referenced by @p sem by performing a
 * semaphore unlock operation on that semaphore.
 *
 * If the semaphore value resulting from this operation is positive, then no threads were blocked
 * waiting for the semaphore to become unlocked; the semaphore value is simply incremented.
 *
 * If the value of the semaphore resulting from this operation is zero, then one of the threads
 * blocked waiting for the semaphore shall be allowed to return successfully from its call to
 * sem_wait().
 *
 * @param sem   A semaphore
 *
 * @return  0 on success.
 * @return  -1, on error and errno set to indicate the error.
 */
static inline int sem_post(sem_t *sem)
{
    int res = sema_post((sema_t *)sem);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return 0;
}

/**
 * @brief Lock a semaphore.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_wait.html">
 *          The Open Group Base Specifications Issue 7, sem_wait()
 *      </a>
 *
 * The sem_wait() function shall lock the semaphore referenced by @p sem by performing a semaphore
 * lock operation on that semaphore. If the semaphore value is currently zero, then the calling
 * thread shall not return from the call to sem_wait() until it either locks the semaphore or the
 * call is interrupted by a signal.
 *
 * @param sem   A semaphore.
 *
 * @return  0 on success.
 * @return  -1, on error and errno set to indicate the error.
 */
static inline int sem_wait(sem_t *sem)
{
    int res = sema_wait((sema_t *)sem);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return 0;
}

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
static inline sem_t *sem_open(const char *name, int oflag, ...)
{
    (void)name;
    (void)oflag;
    errno = ENOMEM;
    return SEM_FAILED;
}

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
static inline int sem_close(sem_t *sem)
{
    (void)sem;
    errno = EINVAL;
    return -1;
}

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
static inline int sem_unlink(const char *name)
{
    (void)name;
    errno = ENOENT;
    return -1;
}

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
 * @return  0 on success.
 * @return  -1, on error and errno set to indicate the error.
 */
int sem_timedwait(sem_t *sem, const struct timespec *abstime);

/**
 * @brief Test whether @p sem is posted.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_trywait.html">
 *          The Open Group Base Specifications Issue 7, sem_trywait()
 *      </a>
 *
 * @param[in] sem   Semaphore to try to wait on
 *
 * @return  0 on success.
 * @return  -1, on error and errno set to indicate the error.
 */
int sem_trywait(sem_t *sem);

/**
 * @brief Get current value of @p sem and store it in @p sval.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_getvalue.html">
 *          The Open Group Base Specifications Issue 7, sem_getvalue()
 *      </a>
 *
 * @param[in] sem   Semaphore to get the value from.
 * @param[out] sval Place where value goes to.
 *
 * @return  0 on success.
 * @return  -1, on error and errno set to indicate the error.
 */
static inline int sem_getvalue(sem_t *sem, int *sval)
{
    if (sem != NULL) {
        *sval = (int)sem->value;
        return 0;
    }
    errno = EINVAL;
    return -1;
}

#ifdef __cplusplus
}
#endif

#endif  /* POSIX_SEMAPHORE_H_ */
/** @} */
