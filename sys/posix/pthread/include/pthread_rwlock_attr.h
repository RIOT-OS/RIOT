/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Implementation of a fair, POSIX conforming reader/writer lock (attribute set).
 * @note    Do not include this header file directly, but pthread.h.
 */


#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Attributes for a new reader/writer lock.
 * @details   The options set in this struct will be ignored by pthread_rwlock_init().
 */
typedef struct
{
    /**
     * @brief     Whether to share lock with child processes.
     * @details   Valid values are `PTHREAD_PROCESS_SHARED` and `PTHREAD_PROCESS_PRIVATE`.
     *            Since RIOT is a single-process operating system, this value is ignored.
     */
    int pshared;
} pthread_rwlockattr_t;

/**
 * @brief           Initialize the attribute set with the defaults.
 * @details         Default value for pshared: `PTHREAD_PROCESS_PRIVATE`.
 *                  A zeroed out datum is initialized.
 * @param[in,out]   attr   Attribute set to initialize.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);

/**
 * @brief           Destroy an attribute set.
 * @details         This function does nothing, don't bother calling it.
 * @param[in,out]   attr   Attribute set to destroy.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);

/**
 * @brief           Read whether to share the lock with child processes.
 * @details         There are not child processes in RIOT.
 * @param[in]       attr      Attribute set to query.
 * @param[out]      pshared   Either `PTHREAD_PROCESS_SHARED` or `PTHREAD_PROCESS_PRIVATE`.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr == NULL`.
 */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);

/**
 * @brief           Set whether to share the lock with child processes.
 * @details         There are not child processes in RIOT.
 * @param[in,out]   attr      Attribute set to operate on.
 * @param[in]       pshared   Either `PTHREAD_PROCESS_SHARED` or `PTHREAD_PROCESS_PRIVATE`.
 * @returns         `0` on success.
 *                  `EINVAL` if `attr == NULL` or a wrong value for `pshared` was supplied.
 */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

#ifdef __cplusplus
}
#endif


/**
 * @}
 */
