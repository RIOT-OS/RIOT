/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Attributes for pthread mutexes.
 * @note    Do not include this header file directly, but pthread.h.
 */

#ifndef PTHREAD_MUTEX_ATTR_H
#define PTHREAD_MUTEX_ATTR_H

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def             PTHREAD_MUTEX_NORMAL
 * @brief           A non-error correcting mutex (default).
 */
/**
 * @def             PTHREAD_MUTEX_RECURSIVE
 * @brief           A mutex that allows recursive locking.
 * @note            Not implemented, yet.
 */
/**
 * @def             PTHREAD_MUTEX_ERRORCHECK
 * @brief           A mutex that fails with `EDEADLK` if recursive locking was detected.
 * @note            Not implemented, yet.
 */
/**
 * @def             PTHREAD_MUTEX_DEFAULT
 * @brief           The default mutex kind for RIOT.
 * @note            Not implemented, yet.
 */
#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

/**
 * @def             PTHREAD_PRIO_NONE
 * @brief           No priority inheritance.
 * @details         Prone to inversed priorities.
 *                  The default mutex protocol.
 */
/**
 * @def             PTHREAD_PRIO_INHERIT
 * @brief           If a thread attempts to acquire a held lock,
 *                  the holding thread gets its dynamic priority increased up to
 *                  the priority of the blocked thread
 * @note            Not implemented, yet.
 */
#define PTHREAD_PRIO_NONE        0
#define PTHREAD_PRIO_INHERIT     1

/**
 * @def             PTHREAD_PRIO_PROTECT
 * @brief           Not implemented, yet.
 */
/**
 * @def             PTHREAD_MUTEX_STALLED
 * @brief           Mutexes aren't automatically released on the end of a thread.
 * @see             #pthread_cleanup_push() for an option to unlock mutexes on the end of a thread.
 * @note            This is the default.
 */
/**
 * @def             PTHREAD_MUTEX_ROBUST
 * @brief           Mutexes that are held at the exit of a thread get released automatically.
 * @note            Not implemented, yet.
 */
#define PTHREAD_PRIO_PROTECT     2
#define PTHREAD_MUTEX_STALLED    0
#define PTHREAD_MUTEX_ROBUST     1

/**
 * @brief           This type is unused right now, and only exists for POSIX compatibility.
 */
typedef struct
{
    int pshared;    /**< Whether to share the mutex with child processes. */
    int kind;       /**< Type of the mutex. */
    int protocol;   /**< Priority inheritance of the mutex. */
    int robustness; /**< What to do if a thread terminates while it holds a mutex. */
} pthread_mutexattr_t;

/**
 * @brief           Initialize a pthread_mutexattr_t
 * @details         A zeroed out datum is initialized
 * @note            This function is not implemented, yet.
 * @param[out]      attr   Datum to initialize.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_mutexattr_init(pthread_mutexattr_t *attr);

/**
 * @brief           Destroys a pthread_mutexattr_t
 * @details         There is no need to ever call this function.
 *                  This function is a no-op.
 * @note            This function is not implemented, yet.
 * @param[in,out]   attr   Datum to destroy.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

/**
 * @brief           Queries the attribute set whether to share the mutex with child processes.
 * @note            Since RIOT does not implement processes, this value is unused.
 * @param[in]       attr      Attribute set to query.
 * @param[out]      pshared   Either #PTHREAD_PROCESS_SHARED or #PTHREAD_PROCESS_PRIVATE.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr` or `pshared` are `NULL`.
 */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);

/**
 * @brief           Set whether to share the mutex with child processes.
 * @note            Since RIOT does not implement processes, this value is unused.
 * @param[in,out]   attr      Attribute set to change.
 * @param[in]       pshared   Either #PTHREAD_PROCESS_SHARED or #PTHREAD_PROCESS_PRIVATE.
 * @returns         `0` on success.
 *                  `EINVAL` if `pshared` is neither #PTHREAD_PROCESS_SHARED nor #PTHREAD_PROCESS_PRIVATE.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);

/**
 * @brief            Query the type of the mutex to create.
 * @note             This implementation only supports PTHREAD_MUTEX_NORMAL mutexes.
 * @param[in]        attr   Attribute set to query.
 * @param[out]       kind   Either #PTHREAD_MUTEX_NORMAL or #PTHREAD_MUTEX_RECURSIVE or #PTHREAD_MUTEX_ERRORCHECK.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr` or `kind` are `NULL`.
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *kind);

/**
 * @brief            Sets the type of the mutex to create.
 * @note             This implementation only supports `PTHREAD_MUTEX_NORMAL` mutexes.
 * @param[in,out]    attr   Attribute set to change.
 * @param[in]        kind   Either #PTHREAD_MUTEX_NORMAL or #PTHREAD_MUTEX_RECURSIVE or #PTHREAD_MUTEX_ERRORCHECK.
 * @returns         `0` on success.
 *                  `EINVAL` if the value of `kind` is unsupported.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);

/**
 * @brief            Query the priority inheritance of the mutex to create.
 * @note             This implementation only supports `PTHREAD_PRIO_NONE` mutexes.
 * @param[in]        attr       Attribute set to query
 * @param[out]       protocol   Either #PTHREAD_PRIO_NONE or #PTHREAD_PRIO_INHERIT or #PTHREAD_PRIO_PROTECT.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr` or `protocol` are `NULL`.
 */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol);

/**
 * @brief            Sets the priority inheritance of the mutex to create.
 * @note             This implementation only supports `PTHREAD_PRIO_NONE` mutexes.
 * @param[in,out]    attr       Attribute set to change.
 * @param[in]        protocol   Either #PTHREAD_PRIO_NONE or #PTHREAD_PRIO_INHERIT or #PTHREAD_PRIO_PROTECT.
 * @returns         `0` on success.
 *                  `EINVAL` if the value of `protocol` is unsupported.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);

/* Return in *PRIOCEILING the mutex prioceiling attribute in *ATTR.  */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling);

/* Set the mutex prioceiling attribute in *ATTR to PRIOCEILING.  */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling);

/**
 * @brief           Query the attribute set whether to create a robut mutex.
 * @details         A "robust" mutex releases gets released automagically if the threads exits while it hold the mutex.
 *                  If the thread is cancellable, only rubust mutex should be held.
 * @note            This implementation does not support robust mutexes, yet.
 *                  As it doesn't implement cancellation points, yet, this should not pose a problem.
 * @param[in]       attr         Attribute set to query.
 * @param[out]      robustness   Either #PTHREAD_MUTEX_STALLED or #PTHREAD_MUTEX_ROBUST.
 * @returns         `0` on success.
 *                  `EINVAL` if the value of `protocol` is unsupported.
 *                  `EINVAL` if `attr` or `robustness` is `NULL`.
 */
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *attr, int *robustness);

/**
 * @brief           Set whether the mutex to create should be robust.
 * @details         A "robust" mutex releases gets released automagically if the threads exits while it hold the mutex.
 *                  If the thread is cancellable, only rubust mutex should be held.
 * @note            This implementation does not support robust mutexes, yet.
 *                  As it doesn't implement cancellation points, yet, this should not pose a problem.
 * @param[in,out]   attr         Attribute set to change.
 * @param[in]       robustness   Either #PTHREAD_MUTEX_STALLED or #PTHREAD_MUTEX_ROBUST.
 * @returns         `0` on success.
 *                  `EINVAL` if the value of `robustness` is unsupported.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robustness);

#ifdef __cplusplus
}
#endif

#endif /* PTHREAD_MUTEX_ATTR_H */

/**
 * @}
 */
