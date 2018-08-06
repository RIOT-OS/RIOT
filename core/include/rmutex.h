/*
 * Copyright (C) 2016 Theobroma Systems Design & Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sync
 * @brief       Recursive Mutex for thread synchronization
 * @{
 *
 * @file
 * @brief       RIOT synchronization API
 *
 * @author      Martin Elshuber <martin.elshuber@theobroma-systems.com>
 *
 */

#ifndef RMUTEX_H
#define RMUTEX_H

#include <stdint.h>
#include <stdatomic.h>

#include "mutex.h"
#include "kernel_types.h"

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
    atomic_int_least16_t owner;
} rmutex_t;

/**
 * @brief Static initializer for rmutex_t.
 * @details This initializer is preferable to rmutex_init().
 */
#define RMUTEX_INIT { MUTEX_INIT, 0, ATOMIC_VAR_INIT(KERNEL_PID_UNDEF) }

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
 *                  initialized first. Must not be NULL.
 *
 * @return 1 if mutex was unlocked, now it is locked.
 * @return 0 if the mutex was locked.
 */
int rmutex_trylock(rmutex_t *rmutex);

/**
 * @brief Locks a recursive mutex, blocking.
 *
 * @param[in] rmutex Recursive mutex object to lock. Has to be
 *                 initialized first. Must not be NULL.
 */
void rmutex_lock(rmutex_t *rmutex);

/**
 * @brief Unlocks the recursive mutex.
 *
 * @param[in] rmutex Recursive mutex object to unlock, must not be NULL.
 */
void rmutex_unlock(rmutex_t *rmutex);

#ifdef __cplusplus
}
#endif

#endif /* RMUTEX_H */
/** @} */
