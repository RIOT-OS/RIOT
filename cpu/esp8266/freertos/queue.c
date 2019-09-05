/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef DOXYGEN

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <string.h>

#include "esp_common.h"
#include "esp_attr.h"
#include "irq_arch.h"
#include "log.h"
#include "mutex.h"
#include "rmutex.h"
#include "syscalls.h"
#include "thread.h"

#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/*
 * In FreeRTOS different types of semaphores, mutexes and queues are all
 * mapped to a single generic queue type. With all these different types,
 * single functions for send, receive, give and take are then used. To be
 * able to dsitinguish between these different types in RIOT, we need typed
 * objects.
 */
typedef struct {
    uint8_t     type;        /* type of the queue, MUST be the first element */
    list_node_t sending;     /* threads that are waiting to send */
    list_node_t receiving;   /* threads that are waiting to receive */
    uint8_t*    queue;       /* the queue of waiting items */
    uint32_t    item_size;   /* size of each item in the queue */
    uint32_t    item_num;    /* num of items that can be stored in queue */
    uint32_t    item_front;  /* first item in queue */
    uint32_t    item_tail;   /* last item in queue */
    uint32_t    item_level;  /* num of items stored in queue */
} _queue_t;

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const uint8_t ucQueueType )
{
    DEBUG("%s pid=%d len=%u size=%u type=%u ", __func__,
          thread_getpid(), uxQueueLength, uxItemSize, ucQueueType);

    uint32_t queue_size = uxQueueLength * uxItemSize;
    _queue_t* queue = malloc(sizeof(_queue_t) + queue_size);

    if (!queue) {
        return NULL;
    }

    queue->type = ucQueueType;
    queue->receiving.next = NULL;
    queue->sending.next = NULL;
    queue->queue = (queue_size) ? (uint8_t*)queue + sizeof(_queue_t) : NULL;
    queue->item_num = uxQueueLength;
    queue->item_size = uxItemSize;
    queue->item_front = 0;
    queue->item_tail = 0;
    queue->item_level = 0;

    DEBUG("queue=%p\n", queue);

    return queue;
}

#define queueSEMAPHORE_QUEUE_ITEM_LENGTH ( ( UBaseType_t ) 0 )

QueueHandle_t xQueueCreateCountingSemaphore (const UBaseType_t uxMaxCount,
                                             const UBaseType_t uxInitialCount)
{
    _queue_t* queue;

    assert(uxMaxCount != 0);
    assert(uxInitialCount <= uxMaxCount);

    queue = xQueueGenericCreate(uxMaxCount, queueSEMAPHORE_QUEUE_ITEM_LENGTH,
                                queueQUEUE_TYPE_COUNTING_SEMAPHORE);

    DEBUG("%s pid=%d queue=%p\n", __func__, thread_getpid(), queue);

    if (queue != NULL) {
        queue->item_level = uxInitialCount;
        queue->item_tail = (queue->item_front + queue->item_level) % queue->item_num;
    }

    return queue;
}

void vQueueDelete( QueueHandle_t xQueue )
{
    DEBUG("%s pid=%d queue=%p\n", __func__, thread_getpid(), xQueue);

    assert(xQueue != NULL);
    free(xQueue);
}

BaseType_t IRAM_ATTR _queue_generic_send(QueueHandle_t xQueue,
                                         const void * const pvItemToQueue,
                                         const BaseType_t xCopyPosition,
                                         TickType_t xTicksToWait,
                                         BaseType_t * const pxHigherPriorityTaskWoken)
{
    DEBUG("%s pid=%d prio=%d queue=%p pos=%d wait=%u woken=%p isr=%d\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, xCopyPosition, xTicksToWait, pxHigherPriorityTaskWoken,
          irq_is_in());

    assert(xQueue != NULL);

    _queue_t* queue = (_queue_t*)xQueue;

    while (1) {
        vTaskEnterCritical(0);

        /* is there still space in the queue */
        if (queue->item_level < queue->item_num || xCopyPosition == queueOVERWRITE) {
            uint32_t write_pos;
            /* determine the write position in the queue and update positions */
            if (xCopyPosition == queueSEND_TO_BACK) {
                write_pos = queue->item_tail;
                queue->item_tail = (queue->item_tail + 1) % queue->item_num;
                queue->item_level++;
            }
            else if (xCopyPosition == queueSEND_TO_FRONT) {
                queue->item_front = (queue->item_front - 1) % queue->item_num;
                queue->item_level++;
                write_pos = queue->item_front;
            }
            else { /* queueOVERWRITE */
                write_pos = queue->item_front;
                if (queue->item_level == 0) {
                    queue->item_level++;
                }
            }

            /* if the item has no 0 size, copy it to the according place in queue */
            if (queue->item_size && queue->queue && pvItemToQueue) {
                memcpy(queue->queue + write_pos * queue->item_size,
                       pvItemToQueue, queue->item_size);
            }

            /* indicates a required context switch */
            bool ctx_switch = false;

            /* unlock waiting receiving thread */
            if (queue->receiving.next != NULL) {
                list_node_t *next = list_remove_head(&queue->receiving);
                thread_t *proc = container_of((clist_node_t*)next, thread_t, rq_entry);
                sched_set_status(proc, STATUS_PENDING);
                ctx_switch = proc->priority < sched_threads[thread_getpid()]->priority;

                DEBUG("%s pid=%d queue=%p unlock waiting pid=%d switch=%d\n",
                      __func__, thread_getpid(), xQueue, proc->pid, ctx_switch);
            }

            if (ctx_switch && pxHigherPriorityTaskWoken) {
                *pxHigherPriorityTaskWoken = pdTRUE;
                vTaskExitCritical(0);
            }
            else if (ctx_switch) {
                vTaskExitCritical(0);
                /* sets only the sched_context_switch_request in ISRs */
                thread_yield_higher();
            }
            else {
                vTaskExitCritical(0);
            }

            DEBUG("%s pid=%d queue=%p return pdPASS\n", __func__,
                  thread_getpid(), xQueue);

            /* in all cases vTaskExitCritical has been called already */
            return pdPASS;
        }
        else if (xTicksToWait == 0 || irq_is_in()) {
            /* if there was no space and timeout = 0, return with error */
            DEBUG("%s pid=%d queue=%p return errQUEUE_FULL\n", __func__,
                  thread_getpid(), xQueue);
            vTaskExitCritical(0);
            return errQUEUE_FULL;
        }
        else {
            /* suspend the calling thread to wait for space in the queue */
            thread_t *me = (thread_t*)sched_active_thread;
            sched_set_status(me, STATUS_SEND_BLOCKED);
            /* waiting list is sorted by priority */
            thread_add_to_list(&queue->sending, me);

            DEBUG("%s pid=%d queue=%p suspended calling thread\n", __func__,
                  thread_getpid(), xQueue);
            vTaskExitCritical(0);
            thread_yield_higher();

            /* TODO timeout handling with xTicksToWait */
            DEBUG("%s pid=%d queue=%p continue calling thread\n", __func__,
                  thread_getpid(), xQueue);
        }
        /* in all cases vTaskExitCritical has been called already */
    }
    return errQUEUE_FULL;
}

BaseType_t IRAM_ATTR _queue_generic_recv (QueueHandle_t xQueue,
                                          void * const pvBuffer,
                                          TickType_t xTicksToWait,
                                          const BaseType_t xJustPeeking,
                                          BaseType_t * const pxHigherPriorityTaskWoken)
{
    DEBUG("%s pid=%d prio=%d queue=%p wait=%u peek=%u woken=%p isr=%d\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, xTicksToWait, xJustPeeking, pxHigherPriorityTaskWoken,
          irq_is_in());

    assert(xQueue != NULL);

    _queue_t* queue = (_queue_t*)xQueue;

    while (1) {
        vTaskEnterCritical(0);

        /* if there is at least one item in the queue */
        if (queue->item_level > 0) {
            /* if the item has no 0 size, copy it from queue to buffer */
            if (queue->item_size && queue->item_num && queue->queue && pvBuffer) {
                memcpy(pvBuffer,
                       queue->queue + queue->item_front * queue->item_size,
                       queue->item_size);
            }


            /* when only peeking leave the element in queue */
            if (xJustPeeking == pdTRUE) {
                DEBUG("%s pid=%d queue=%p return pdPASS\n", __func__,
                      thread_getpid(), xQueue);
                vTaskExitCritical(0);
                return pdPASS;
            }

            /* remove element from queue */
            queue->item_front = (queue->item_front + 1) % queue->item_num;
            queue->item_level--;

            /* return if there is no waiting sending thread */
            if (queue->sending.next == NULL) {
                DEBUG("%s pid=%d queue=%p return pdPASS\n", __func__,
                      thread_getpid(), xQueue);
                vTaskExitCritical(0);
                return pdPASS;
            }

            /* otherwise unlock the waiting sending thread */
            list_node_t *next = list_remove_head(&queue->sending);
            thread_t *proc = container_of((clist_node_t*)next, thread_t, rq_entry);
            sched_set_status(proc, STATUS_PENDING);

            /* test whether context switch is required */
            bool ctx_switch = proc->priority < sched_threads[thread_getpid()]->priority;

            DEBUG("%s pid=%d queue=%p unlock waiting pid=%d switch=%d\n",
            __func__, thread_getpid(), xQueue, proc->pid, ctx_switch);

            if (ctx_switch && pxHigherPriorityTaskWoken) {
                *pxHigherPriorityTaskWoken = pdTRUE;
                vTaskExitCritical(0);
            }
            else if (ctx_switch) {
                vTaskExitCritical(0);
                /* sets only the sched_context_switch_request in ISRs */
                sched_switch(proc->priority);
            }
            else {
                vTaskExitCritical(0);
            }

            DEBUG("%s pid=%d queue=%p return pdPASS\n", __func__,
                  thread_getpid(), xQueue);

            /* in all cases vTaskExitCritical has been called already */
            return pdPASS;
        }
        else if (xTicksToWait == 0 || irq_is_in()) {
            /* if there was no item in the queue and timeout is 0, return with error */
            DEBUG("%s pid=%d queue=%p return errQUEUE_EMPTY\n", __func__,
                  thread_getpid(), xQueue);
            vTaskExitCritical(0);
            return errQUEUE_EMPTY;
        }
        else {
            /* suspend the calling thread to wait for an item in the queue */
            thread_t *me = (thread_t*)sched_active_thread;
            sched_set_status(me, STATUS_RECEIVE_BLOCKED);
            /* waiting list is sorted by priority */
            thread_add_to_list(&queue->receiving, me);

            DEBUG("%s pid=%d queue=%p suspended calling thread\n", __func__,
                  thread_getpid(), xQueue);

            vTaskExitCritical(0);
            thread_yield_higher();

            /* TODO timeout handling with xTicksToWait */
            DEBUG("%s pid=%d queue=%p continue calling thread\n", __func__,
                  thread_getpid(), xQueue);
        }
        /* in all cases vTaskExitCritical has been called already */
    }
}

BaseType_t IRAM_ATTR xQueueGenericSend( QueueHandle_t xQueue,
                                        const void * const pvItemToQueue,
                                        TickType_t xTicksToWait,
                                        const BaseType_t xCopyPosition )
{
    DEBUG("%s pid=%d prio=%d queue=%p wait=%u pos=%d\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, xTicksToWait, xCopyPosition);

    return _queue_generic_send(xQueue, pvItemToQueue, xCopyPosition,
                               xTicksToWait, NULL);
}

BaseType_t IRAM_ATTR xQueueGenericSendFromISR( QueueHandle_t xQueue,
                                               const void * const pvItemToQueue,
                                               BaseType_t * const pxHigherPriorityTaskWoken,
                                               const BaseType_t xCopyPosition )
{
    DEBUG("%s pid=%d prio=%d queue=%p pos=%d woken=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, xCopyPosition, pxHigherPriorityTaskWoken);

    return _queue_generic_send(xQueue, pvItemToQueue, xCopyPosition,
                               0, pxHigherPriorityTaskWoken);
}

BaseType_t IRAM_ATTR xQueueGenericReceive (QueueHandle_t xQueue,
                                           void * const pvBuffer,
                                           TickType_t xTicksToWait,
                                           const BaseType_t xJustPeeking)
{
    DEBUG("%s pid=%d prio=%d queue=%p wait=%u peek=%d\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, xTicksToWait, xJustPeeking);

    return _queue_generic_recv(xQueue, pvBuffer, xTicksToWait,
                               xJustPeeking, NULL);
}

BaseType_t IRAM_ATTR xQueueReceiveFromISR (QueueHandle_t xQueue,
                                           void * const pvBuffer,
                                           BaseType_t * const pxHigherPriorityTaskWoken)
{
    DEBUG("%s pid=%d prio=%d queue=%p woken=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, pxHigherPriorityTaskWoken);

    return _queue_generic_recv(xQueue, pvBuffer, 0,
                               0, pxHigherPriorityTaskWoken);
}

UBaseType_t uxQueueMessagesWaiting( QueueHandle_t xQueue )
{
    assert(xQueue != NULL);

    _queue_t* queue = (_queue_t*)xQueue;
    return queue->item_level;
}

BaseType_t xQueueGiveFromISR (QueueHandle_t xQueue,
                              BaseType_t * const pxHigherPriorityTaskWoken)
{
    /* this function only satisfies the linker and should not be called */
    assert(0);

    DEBUG("%s\n", __func__);
    return pdFALSE;
}

#endif /* DOXYGEN */
