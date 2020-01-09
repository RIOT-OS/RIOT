/*
 * Copyright (C) 2018 Gunar Schorcht
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
#include "log.h"
#include "mutex.h"
#include "rmutex.h"
#include "thread.h"

#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#undef  portENTER_CRITICAL
#undef  portEXIT_CRITICAL
#define portENTER_CRITICAL(mux) vTaskEnterCritical(mux)
#define portEXIT_CRITICAL(mux)  vTaskExitCritical(mux)

/*
 * In FreeRTOS different types of semaphores, mutexes and queues are all
 * mapped to a single generic queue type. With all these different types,
 * single functions for send, receive, give and take are then used. To be
 * able to dsitinguish between these different types in RIOT, we need typed
 * objects.
 */
typedef struct {
    uint8_t     type;        /* type of the queue, MUST be the first element */
    mutex_t     mutex;       /* mutex to secure operations on the queue */
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

    mutex_init(&queue->mutex);

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

    CHECK_PARAM_RET(uxMaxCount != 0, NULL);
    CHECK_PARAM_RET(uxInitialCount <= uxMaxCount, NULL);

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

    CHECK_PARAM(xQueue != NULL);
    free(xQueue);
}

BaseType_t IRAM_ATTR _queue_generic_send(QueueHandle_t xQueue,
                                         const void * const pvItemToQueue,
                                         const BaseType_t xCopyPosition,
                                         TickType_t xTicksToWait,
                                         BaseType_t * const pxHigherPriorityTaskWoken)
{
    DEBUG("%s pid=%d prio=%d queue=%p pos=%d wait=%u woken=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, xCopyPosition, xTicksToWait, pxHigherPriorityTaskWoken);

    _queue_t* queue = (_queue_t*)xQueue;

    CHECK_PARAM_RET(queue != NULL, pdFAIL);

    while (1) {
        taskENTER_CRITICAL(&queue->mutex);

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

            /* unlock waiting receiving thread */
            if (queue->receiving.next != NULL) {
                list_node_t *next = list_remove_head(&queue->receiving);
                thread_t *process = container_of((clist_node_t*)next, thread_t, rq_entry);
                uint8_t process_priority = process->priority;
                uint8_t my_priority = sched_threads[thread_getpid()]->priority;

                if (pxHigherPriorityTaskWoken) {
                    *pxHigherPriorityTaskWoken = process_priority < my_priority;
                }

                DEBUG("%s pid=%d queue=%p unlock waiting\n", __func__,
                      thread_getpid(), xQueue);

                sched_set_status(process, STATUS_PENDING);
                sched_switch(process_priority);
            }

            DEBUG("%s pid=%d queue=%p return pdPASS\n", __func__,
                  thread_getpid(), xQueue);
            taskEXIT_CRITICAL(&queue->mutex);
            return pdPASS;
        }
        else if (xTicksToWait == 0) {
            /* if there was no space and timeout = 0, return with error */

            DEBUG("%s pid=%d queue=%p return errQUEUE_FULL\n", __func__,
                  thread_getpid(), xQueue);

            taskEXIT_CRITICAL(&queue->mutex);
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

            taskEXIT_CRITICAL(&queue->mutex);
            thread_yield_higher();

            /* TODO timeout handling with xTicksToWait */
            DEBUG("%s pid=%d queue=%p continue calling thread\n", __func__,
                  thread_getpid(), xQueue);
        }
    }
    return errQUEUE_FULL;
}

BaseType_t IRAM_ATTR _queue_generic_recv (QueueHandle_t xQueue,
                                          void * const pvBuffer,
                                          TickType_t xTicksToWait,
                                          const BaseType_t xJustPeeking,
                                          BaseType_t * const pxHigherPriorityTaskWoken)
{
    DEBUG("%s pid=%d prio=%d queue=%p wait=%u peek=%u woken=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          xQueue, xTicksToWait, xJustPeeking, pxHigherPriorityTaskWoken);

    _queue_t* queue = (_queue_t*)xQueue;

    CHECK_PARAM_RET(queue != NULL, pdFAIL);

    while (1) {
        taskENTER_CRITICAL(&queue->mutex);

        if (queue->item_level == 0 && xTicksToWait == 0) {
            /* if there was no element in queue and timeout = 0, return with error */

            DEBUG("%s pid=%d queue=%p return errQUEUE_EMPTY\n", __func__,
                  thread_getpid(), xQueue);

            taskEXIT_CRITICAL(&queue->mutex);
            return errQUEUE_EMPTY;
        }

        if (queue->item_level > 0) {
            /* if the item has no 0 size, copy it from queue to buffer */
            if (queue->item_size && queue->item_num && queue->queue && pvBuffer) {
                memcpy(pvBuffer,
                       queue->queue + queue->item_front * queue->item_size,
                       queue->item_size);
            }
            /* when only peeking leave the element in queue */
            if (xJustPeeking == pdFALSE) {
                queue->item_front = (queue->item_front + 1) % queue->item_num;
                queue->item_level--;

                /* unlock waiting sending thread */
                if (queue->sending.next != NULL) {
                    list_node_t *next = list_remove_head(&queue->sending);
                    thread_t *process = container_of((clist_node_t*)next,
                                                     thread_t, rq_entry);
                    uint16_t process_priority = process->priority;
                    uint8_t my_priority = sched_threads[thread_getpid()]->priority;

                    if (pxHigherPriorityTaskWoken) {
                        *pxHigherPriorityTaskWoken = process_priority < my_priority;
                    }

                    DEBUG("%s pid=%d queue=%p unlock waiting\n", __func__,
                          thread_getpid(), xQueue);

                    sched_set_status(process, STATUS_PENDING);
                    sched_switch(process_priority);
                }
            }

            DEBUG("%s pid=%d queue=%p return pdPASS\n", __func__,
                  thread_getpid(), xQueue);

            taskEXIT_CRITICAL(&queue->mutex);
            return pdPASS;
        }
        else {
            /* suspend the calling thread to wait for an item in the queue */
            thread_t *me = (thread_t*)sched_active_thread;
            sched_set_status(me, STATUS_RECEIVE_BLOCKED);
            /* waiting list is sorted by priority */
            thread_add_to_list(&queue->receiving, me);

            DEBUG("%s pid=%d queue=%p suspended calling thread\n", __func__,
                  thread_getpid(), xQueue);

            taskEXIT_CRITICAL(&queue->mutex);
            thread_yield_higher();

            /* TODO timeout handling with xTicksToWait */
            DEBUG("%s pid=%d queue=%p continue calling thread\n", __func__,
                  thread_getpid(), xQueue);
        }
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
    _queue_t* queue = (_queue_t*)xQueue;

    CHECK_PARAM_RET(queue != NULL, 0);

    return queue->item_level;
}

BaseType_t xQueueGiveFromISR (QueueHandle_t xQueue,
                              BaseType_t * const pxHigherPriorityTaskWoken)
{
    ets_printf("%s\n", __func__);
    return pdFALSE;
}

#endif /* DOXYGEN */
