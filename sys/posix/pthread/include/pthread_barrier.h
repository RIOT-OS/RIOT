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
 * @brief   Synchronization barriers.
 * @note    Do not include this header file directly, but pthread.h.
 */


#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     PTHREAD_PROCESS_SHARED
 * @brief   Share the structure with child processes (default).
 * @note    RIOT is a single-process OS.
 *          Setting the value of `pshared` does not change anything.
 */
#define PTHREAD_PROCESS_SHARED (0)
/**
 * @def     PTHREAD_PROCESS_PRIVATE
 * @brief   Don't share the structure with child processes.
 * @note    RIOT is a single-process OS.
 *          Setting the value of `pshared` does not change anything.
 */
#define PTHREAD_PROCESS_PRIVATE (1)

/**
 * @brief   Internal structure to store the list of waiting threads.
 */
typedef struct pthread_barrier_waiting_node
{
    struct pthread_barrier_waiting_node *next; /**< The next waiting thread. */
    kernel_pid_t pid; /**< The current thread to wake up. */
    volatile int cont; /**< 0 = spurious wake up, 1 = wake up */
} pthread_barrier_waiting_node_t;

/**
 * @brief     A synchronization barrier.
 * @details   Initialize with pthread_barrier_init().
 *            For a zeroed out datum you do not need to call the initializer,
 *            it is enough to set pthread_barrier_t::count.
 */
typedef struct
{
    struct pthread_barrier_waiting_node *next; /**< The first waiting thread. */
    mutex_t mutex; /**< Mutex to unlock to wake the thread up. */
    volatile int count; /**< Wait for N more threads before waking everyone up. */
} pthread_barrier_t;

/**
 * @brief     Details for a pthread_barrier_t.
 * @details   RIOT does not need this structure, because it is a single process OS.
 *            This is only here to POSIX compatibility.
 */
typedef struct
{
    int pshared; /**< See pthread_barrierattr_setpshared() and pthread_barrierattr_getpshared(). */
} pthread_barrierattr_t;

/**
 * @brief     Initializes a pthread_barrier_t
 * @param     barrier   Datum to initialize
 * @param     attr      (unused)
 * @param     count     Number of thread to wait for
 * @returns   0, the invocation cannot fail
 */
int pthread_barrier_init(pthread_barrier_t *barrier,
                         const pthread_barrierattr_t *attr,
                         unsigned int count);

/**
 * @brief     Destroys a pthread_barrier_t
 * @details   To use the barrier again you will need to call pthread_barrier_init() again.
 *            Destroying a barrier while threads are currently waiting for it causes indefined behavior.
 * @param     barrier   Barrier to destroy
 * @returns   0, the invocation cannot fail
 */
int pthread_barrier_destroy(pthread_barrier_t *barrier);

/**
 * @brief     Waiting on a synchronization barrier
 * @details   The barrier need to be initialized with pthread_barrier_init().
 * @param     barrier   Barrier to wait for
 * @returns   0, the invocation cannot fail
 */
int pthread_barrier_wait(pthread_barrier_t *barrier);

/**
 * @brief     Initialize a pthread_barrierattr_t
 * @details   A zeroed out datum is initialized.
 * @param     attr   Datum to initialize.
 * @returns   0, the invocation cannot fail
 */
int pthread_barrierattr_init(pthread_barrierattr_t *attr);

/**
 * @brief     Destroy a pthread_barrierattr_t
 * @details   This function does nothing.
 * @param     attr   Datum to destroy
 * @returns   0, the invocation cannot fail
 */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);

/**
 * @brief     Returns whether the barrier attribute was set to be shared
 * @param     attr      Barrier attribute to read
 * @param     pshared   The value previously stored with pthread_barrierattr_setpshared().
 * @returns   0, the invocation cannot fail
 */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);

/**
 * @brief     Set if the barrier should be shared with child processes
 * @details   Since RIOT is a single process OS, pthread_barrier_init() will ignore the value.
 * @param     attr      Attribute set for pthread_barrier_init()
 * @param     pshared   Either #PTHREAD_PROCESS_PRIVATE or #PTHREAD_PROCESS_SHARED
 * @returns   0, the invocation cannot fail
 */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

#ifdef __cplusplus
}
#endif


/**
 * @}
 */
