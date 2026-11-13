// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
/*
 * simple_queue.c
 *
 * Description: simple and lightweight implementation of message-queues in C.
 *
 * Copyright (c) 2016, Robert Wiewel
 *
 * This file has been initially provided under the BSD 3-clause license and
 * is now provided in agreement with the original author as part of uD3TN
 * under the terms and conditions of either the Apache 2.0 or the BSD 3-clause
 * license. See the LICENSE file in the project root for details.
 *
 */
#include "platform/hal_semaphore.h"
#include "platform/posix/simple_queue.h"

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static char *increment(
    char *current, char *abs_start, char *abs_end, unsigned int item_size)
{
    char *val = current + item_size;

    if (val > abs_end) {
        val = abs_start;
    }

    return val;
}

static char *decrement(
    char *current, char *abs_start, char *abs_end, unsigned int item_size)
{
    char *val = current - item_size;

    if (val < abs_start) {
        val = abs_end - item_size;
    }

    return val;
}

Queue_t *queueCreate(unsigned int queue_length, unsigned int item_size)
{
    if (queue_length == 0 || item_size == 0) {
        // if one of the values is zero, creating a queue
        // makes no sense
        abort();
    }

    // allocate memory to store management data
    Queue_t *queue = malloc(sizeof(Queue_t));

    // initialise the queue's semaphore
    queue->semaphore = hal_semaphore_init_value(1);
    queue->sem_pop = hal_semaphore_init_value(0);
    queue->sem_push = hal_semaphore_init_value(queue_length);

    queue->item_length = queue_length;
    queue->item_size = item_size;

    // allocate enough memory to store the actual items
    queue->abs_start = malloc(queue->item_length * queue->item_size);
    queue->abs_end = queue->abs_start + (
        queue->item_length * queue->item_size);

    queue->current_start = queue->abs_start;
    queue->current_end = queue->abs_start;

    return queue;
}

void queueDelete(Queue_t *queue)
{
    // get the semaphore
    hal_semaphore_take_blocking(queue->semaphore);

    // free the data memory
    free(queue->abs_start);

    hal_semaphore_delete(queue->sem_pop);
    hal_semaphore_delete(queue->sem_push);

    // destroy the semaphore
    hal_semaphore_delete(queue->semaphore);

    // free the memory resources for the management data
    free(queue);
}

void queueReset(Queue_t *queue)
{
    hal_semaphore_take_blocking(queue->semaphore);

    // reset both current-pointers to the absolute start pointer
    // old values will be simply overwritten when new items are stored
    queue->current_start = queue->abs_start;
    queue->current_end = queue->abs_start;

    hal_semaphore_delete(queue->sem_pop);
    hal_semaphore_delete(queue->sem_push);

    queue->sem_pop = hal_semaphore_init_value(0);
    queue->sem_push = hal_semaphore_init_value(queue->item_length);

    hal_semaphore_release(queue->semaphore);
}

uint8_t queuePop(Queue_t *queue, void *targetBuffer, int64_t timeout)
{
    if (hal_semaphore_try_take(queue->sem_pop, timeout) == UD3TN_FAIL) {
        return EXIT_FAILURE;
    }

    hal_semaphore_take_blocking(queue->semaphore);

    if (queue->current_start >= queue->abs_end) {
        queue->current_start = queue->abs_start;
    }

    memcpy(targetBuffer, queue->current_start, queue->item_size);

    queue->current_start = increment(
        queue->current_start, queue->abs_start,
        queue->abs_end, queue->item_size);

    hal_semaphore_release(queue->sem_push);
    hal_semaphore_release(queue->semaphore);

    return EXIT_SUCCESS;
}


uint8_t queuePush(Queue_t *queue, const void *item, int64_t timeout, bool force)
{
    // forcefully replace the last element of the queue
    if (force) {
        hal_semaphore_take_blocking(queue->semaphore);

        if (hal_semaphore_is_blocked(queue->sem_push)) {
            memcpy(
                decrement(
                    queue->current_end, queue->abs_start,
                    queue->abs_end, queue->item_size
                    ),
                item, queue->item_size
                );
        }

        hal_semaphore_release(queue->semaphore);

        return EXIT_SUCCESS;
    }

    if (hal_semaphore_try_take(queue->sem_push, timeout) == UD3TN_FAIL) {
        return EXIT_FAILURE;
    }

    hal_semaphore_take_blocking(queue->semaphore);

    if (queue->current_end >= queue->abs_end) {
        queue->current_end = queue->abs_start;
    }

    memcpy(queue->current_end, item, queue->item_size);

    queue->current_end = increment(
        queue->current_end, queue->abs_start,
        queue->abs_end, queue->item_size);

    hal_semaphore_release(queue->sem_pop);
    hal_semaphore_release(queue->semaphore);

    return EXIT_SUCCESS;
}
