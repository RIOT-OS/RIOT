/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_newlib
 * @{
 *
 * @file
 * @brief       Newlib thread-safe recursive lock implementation
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#ifndef THREAD_SAFE_NEWLIB_LOCK_H_
#define THREAD_SAFE_NEWLIB_LOCK_H_

#include <sys/reent.h>

#include <thread.h>
#include <mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   An recursive mutex implementation that can be used for newlib's
 *          thread-safe implementation
 */
typedef struct recursive_mutex
{
    volatile kernel_pid_t owner;    /**< owner of the recursive mutex */
    atomic_int_t recursion;         /**< number of recursive lock calls */
    mutex_t mutex;                  /**< mutex that is locked recursively */
} recursive_mutex_t;

/**
 * @brief Initialization values for a recursive mutex
 */
#define RECURSIVE_MUTEX_INIT    { KERNEL_PID_UNDEF, { 0 }, MUTEX_INIT }

/**
 * @brief initialize the recursive mutex if it's not yet initialized
 *
 * @param[in] rm    recursive mutex structure
 */
void __recursive_init(recursive_mutex_t *rm);

/**
 * @brief   lock the mutex recursively, try to lock the mutex.
 *          - If the mutex is free claim ownership.
 *          - If the mutex is locked by us already; increase the recursion count
 *          - If the mutex is locked by another task; wait for the mutex to be freed
 *
 * @param[in] rm    recursive mutex structure
 */
void __recursive_lock(recursive_mutex_t *rm);

/**
 * @brief   release the mutex recursively, if the last lock is freeed release the mutex
 *
 * @param[in] rm    recursive mutex structure
 */
void __recursive_unlock(recursive_mutex_t *rm);

#ifdef __cplusplus
};
#endif

#endif /* THREAD_SAFE_NEWLIB_LOCK_H_ */
