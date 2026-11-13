// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#ifndef HAL_TYPES_H_INCLUDED
#define HAL_TYPES_H_INCLUDED

#include "platform/riot/simple_queue.h"

#include <fcntl.h>
#include "thread.h"
#include "sema.h"

typedef struct Semaphore {
    sema_t sem;
} *Semaphore_t;

#define QueueIdentifier_t Queue_t *

#define TaskIdentifier_t kernel_pid_t

#define HAL_SEMAPHORE_MAX_DELAY_MS 9223372036854ULL
#define HAL_QUEUE_MAX_DELAY_MS HAL_SEMAPHORE_MAX_DELAY_MS

#endif // HAL_TYPES_H_INCLUDED
