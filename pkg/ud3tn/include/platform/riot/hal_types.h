/* SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0 */
/**
 * @file
 * @brief RIOT-specific types and timeout limits for the uD3TN HAL.
 */
#ifndef HAL_TYPES_H_INCLUDED
#define HAL_TYPES_H_INCLUDED

#include "platform/riot/simple_queue.h"

#include <fcntl.h>
#include "thread.h"
#include "sema.h"

/**
 * @brief HAL semaphore handle backed by a RIOT semaphore.
 */
typedef struct Semaphore {
    sema_t sem;
} *Semaphore_t;

/**
 * @brief HAL queue identifier backed by the RIOT queue implementation.
 */
#define QueueIdentifier_t Queue_t *

/**
 * @brief HAL task identifier represented by a RIOT kernel process ID.
 */
#define TaskIdentifier_t kernel_pid_t

/**
 * @brief Timeout threshold in milliseconds above which a semaphore wait
 * blocks indefinitely.
 */
#define HAL_SEMAPHORE_MAX_DELAY_MS 9223372036854ULL

/**
 * @brief Timeout threshold in milliseconds above which a queue wait blocks
 * indefinitely.
 */
#define HAL_QUEUE_MAX_DELAY_MS HAL_SEMAPHORE_MAX_DELAY_MS

#endif /* HAL_TYPES_H_INCLUDED */
