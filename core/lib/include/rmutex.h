/*
 * SPDX-FileCopyrightText: 2016 Theobroma Systems Design & Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    core_sync_rmutex Recursive Mutex
 * @ingroup     core_sync
 * @brief       Recursive Mutex for thread synchronization
 *
 * @{
 *
 * @file
 * @brief       Recursive Mutex for thread synchronization
 *
 * @author      Martin Elshuber <martin.elshuber@theobroma-systems.com>
 *
 */

#include <stdint.h>

#include "mutex.h"
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mutex structure. Must never be modified by the user.
 */
typedef struct rmutex_t {
    /* fields are managed by mutex functions, don't touch */
    /**
     * @brief The mutex used for locking. **Must never be changed by
     *        the user.**
     * @internal
     */
    mutex_t mutex;

    /**
     * @brief   Number of locks owned by the thread owner
     * @internal
     */
    uint16_t refcount;

    /**
     * @brief   Owner thread of the mutex.
     * @details Owner is written by the mutex holder, and read
     *          concurrently to ensure consistency,
     *          atomic_int_least16_t is used. Note @ref kernel_pid_t is an int16
     * @internal
     */
    kernel_pid_t owner;
} rmutex_t;

/**
 * @brief Static initializer for rmutex_t.
 * @details This initializer is preferable to rmutex_init().
 */
#define RMUTEX_INIT { MUTEX_INIT, 0, KERNEL_PID_UNDEF }

/**
 * @brief Initializes a recursive mutex object.
 * @details For initialization of variables use RMUTEX_INIT instead.
 *          Only use the function call for dynamically allocated mutexes.
 * @param[out] rmutex    pre-allocated mutex structure, must not be NULL.
 */
static inline void rmutex_init(rmutex_t *rmutex)
{
    rmutex_t empty_rmutex = RMUTEX_INIT;

    *rmutex = empty_rmutex;
}

/**
 * @brief Tries to get a recursive mutex, non-blocking.
 *
 * @param[in] rmutex Recursive mutex object to lock. Has to be
 *                   initialized first. Must not be NULL.
 *
 * @retval 1 if the mutex was unlocked or locked by same thread, it is locked now.
 * @retval 0 if the mutex was locked by another thread.
 */
int rmutex_trylock(rmutex_t *rmutex);

/**
 * @brief Locks a recursive mutex, blocking.
 *
 * @param[in] rmutex Recursive mutex object to lock. Has to be
 *                   initialized first. Must not be NULL.
 */
void rmutex_lock(rmutex_t *rmutex);

/**
 * @brief Tries to get a recursive mutex without exceeding a maximum recursion depth,
 *        non-blocking.
 *
 * @param[in] rmutex Recursive mutex object to lock. Has to be
 *                   initialized first. Must not be NULL.
 * @param[in] max    maximum recursion depth, must be > 0.
 *
 * @retval 1 if the mutex was unlocked, now it is locked.
 * @retval 0 if the mutex was locked or the recursion depth has been exceeded.
 */
int rmutex_trylock_max(rmutex_t *rmutex, uint16_t max);

/**
 * @brief Tries to get a recursive mutex without exceeding a maximum recursion depth,
 *        blocking.
 *
 * @param[in] rmutex Recursive mutex object to lock. Has to be
 *                   initialized first. Must not be NULL.
 * @param[in] max    maximum recursion depth.
 *
 * @retval 1 if the mutex was unlocked, now it is locked.
 * @retval 0 if recursion depth was exceeded.
 */
int rmutex_lock_max(rmutex_t *rmutex, uint16_t max);

/**
 * @brief Unlocks the recursive mutex.
 *
 * @param[in] rmutex Recursive mutex object to unlock, must not be NULL.
 */
void rmutex_unlock(rmutex_t *rmutex);

#ifdef __cplusplus
}
#endif

/** @} */
