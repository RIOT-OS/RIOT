/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       LibCSP
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */
#include <stdio.h>

#include "csp/arch/csp_queue.h"
#include "csp_types_riot.h"

#include "ztimer.h"

static int pop_item(csp_static_queue_t *q, void *buf)
{
    if(tsrb_get(&q->tsrb, buf, q->item_size) == (int) q->item_size) {
        mutex_unlock(&q->push);
        return q->item_size;
    }
    return 0;
}

static int push_item(csp_static_queue_t *q, const void *value)
{
    if(tsrb_add(&q->tsrb, value, q->item_size) == (int) q->item_size) {
        mutex_unlock(&q->pop);
        return q->item_size;
    }
    return 0;
}

csp_queue_handle_t csp_queue_create_static(int length, size_t item_size,
                                           char *buf, csp_static_queue_t *queue)
{
    queue->item_size = item_size;
    tsrb_init(&queue->tsrb, (uint8_t*) buf, length * item_size);
    mutex_init(&queue->pop);
    mutex_init(&queue->push);
    /* since all queues begin empty lock the 'pop' mutex */
    mutex_lock(&queue->pop);
    return queue;
}

int csp_queue_enqueue(csp_queue_handle_t queue, const void *value, uint32_t timeout)
{
    /* try to acquire the 'push' lock, if it fails it means there is no data
       in the queue */
    if (ztimer_mutex_lock_timeout(ZTIMER_MSEC, &queue->push, timeout)) {
        return CSP_QUEUE_ERROR;
    }
    int res = push_item(queue, value);
    /* unlock 'push' lock, unless queue is full, next callers will wait on lock */
    if (csp_queue_free(queue)) {
        mutex_unlock(&queue->push);
    }
    return res == 0 ? CSP_QUEUE_ERROR : CSP_QUEUE_OK;
}

int csp_queue_enqueue_isr(csp_queue_handle_t queue, const void *value, int *unused)
{
    (void)unused;
    return csp_queue_enqueue(queue, value, 0);
}

int csp_queue_dequeue(csp_queue_handle_t queue, void *buf, uint32_t timeout)
{
    /* try to acquire the 'pop' lock, if it fails it means there is no data
       in the queue */
    if (ztimer_mutex_lock_timeout(ZTIMER_MSEC, &queue->pop, timeout)) {
        return CSP_QUEUE_ERROR;
    }
    int res = pop_item(queue, buf);
    /* unlock 'pop' lock, unless queue is empty, next callers will wait on lock */
    if (csp_queue_size(queue)) {
        mutex_unlock(&queue->pop);
    }
    return res == 0 ? CSP_QUEUE_ERROR : CSP_QUEUE_OK;
}

int csp_queue_dequeue_isr(csp_queue_handle_t queue, void *buf, int *unused)
{
    (void)unused;
    return csp_queue_dequeue(queue, buf, 0);
}

int csp_queue_size(csp_queue_handle_t queue)
{
    return tsrb_avail(&queue->tsrb) / queue->item_size;
}

int csp_queue_size_isr(csp_queue_handle_t queue)
{
    unsigned irq_state = irq_disable();
    int size = tsrb_avail(&queue->tsrb) / queue->item_size;

    irq_restore(irq_state);
    return size;
}

int csp_queue_free(csp_queue_handle_t queue)
{
    return tsrb_free(&queue->tsrb) / queue->item_size;
}
