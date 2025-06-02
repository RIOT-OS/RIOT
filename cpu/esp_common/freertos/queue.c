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

#define ENABLE_DEBUG 0
#include "debug.h"

#include <assert.h>
#include <string.h>

#include "esp_common.h"
#include "esp_attr.h"
#include "irq_arch.h"
#include "log.h"
#include "mutex.h"
#include "rmutex.h"
#include "syscalls.h"
#include "thread.h"
#if IS_USED(MODULE_ZTIMER_MSEC)
#include "ztimer.h"
#endif

#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

QueueHandle_t xQueueCreateStatic( const UBaseType_t uxQueueLength,
                                  const UBaseType_t uxItemSize,
                                  uint8_t *pucQueueStorageBuffer,
                                  StaticQueue_t *pxQueueBuffer )

{
    DEBUG("%s pid=%d len=%u size=%u storage=%p buffer=%p\n", __func__,
          thread_getpid(),
          uxQueueLength, uxItemSize, pucQueueStorageBuffer, pxQueueBuffer);

    _queue_t *queue = (_queue_t *)pxQueueBuffer;
    uint32_t queue_size = uxQueueLength * uxItemSize;

    assert(queue != NULL);
    memset(queue, 0, sizeof(_queue_t));

    if (uxItemSize) {
        assert(pucQueueStorageBuffer != NULL);
        memset(pucQueueStorageBuffer, 0, uxQueueLength * uxItemSize);
    }

    queue->type = queueQUEUE_TYPE_BASE;
    queue->stat = true;
    queue->receiving.next = NULL;
    queue->sending.next = NULL;
    queue->queue = (queue_size) ? pucQueueStorageBuffer : NULL;
    queue->item_num = uxQueueLength;
    queue->item_size = uxItemSize;
    queue->item_front = 0;
    queue->item_tail = 0;
    queue->item_level = 0;

    return queue;
}

BaseType_t xQueueGetStaticBuffers( QueueHandle_t xQueue,
                                   uint8_t **ppucQueueStorage,
                                   StaticQueue_t **ppxStaticQueue )
{
    _queue_t *queue = (_queue_t *)xQueue;

    assert(queue != NULL);
    assert(ppxStaticQueue != NULL);

    if (ppucQueueStorage) {
        *ppucQueueStorage = queue->queue;
    }
    *ppxStaticQueue = queue;

    return pdTRUE;
}

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const uint8_t ucQueueType )
{
    DEBUG("%s pid=%d len=%u size=%u type=%u\n", __func__,
          thread_getpid(), uxQueueLength, uxItemSize, ucQueueType);

    uint32_t queue_size = uxQueueLength * uxItemSize;
    _queue_t *queue = malloc(sizeof(_queue_t) + queue_size);

    assert(queue != NULL);

    QueueHandle_t handle;
    handle = xQueueCreateStatic(uxQueueLength, uxItemSize,
                                (uint8_t*)queue + sizeof(_queue_t), queue);
    queue->type = ucQueueType;
    queue->stat = false;

    DEBUG("queue=%p\n", queue);

    return handle;
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

    DEBUG("%s pid=%d queue=%p max=%d initial=%d\n", __func__,
          thread_getpid(), queue, uxMaxCount, uxInitialCount);

    if (queue != NULL) {
        queue->item_level = uxInitialCount;
        queue->item_tail = (queue->item_front + queue->item_level) % queue->item_num;
    }

    return queue;
}

QueueHandle_t xQueueCreateCountingSemaphoreStatic(const UBaseType_t uxMaxCount,
                                                  const UBaseType_t uxInitialCount,
                                                  StaticQueue_t *pxStaticQueue)
{
    assert(pxStaticQueue);
    assert(uxMaxCount != 0);
    assert(uxInitialCount <= uxMaxCount);

    _queue_t *queue = (_queue_t *)pxStaticQueue;

    DEBUG("%s pid=%d queue=%p max=%d initial=%d\n", __func__,
          thread_getpid(), queue, uxMaxCount, uxInitialCount);

    if (xQueueCreateStatic(uxMaxCount, queueSEMAPHORE_QUEUE_ITEM_LENGTH,
                           NULL, pxStaticQueue) == NULL) {
        return NULL;
    }

    queue->type = queueQUEUE_TYPE_COUNTING_SEMAPHORE;
    queue->item_level = uxInitialCount;
    queue->item_tail = (queue->item_front + queue->item_level) % queue->item_num;

    return queue;
}

QueueHandle_t xQueueCreateWithCaps(const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const UBaseType_t uxMemoryCaps)
{
    (void)uxMemoryCaps;
    return xQueueGenericCreate(uxQueueLength, uxItemSize, queueQUEUE_TYPE_BASE);
}

void vQueueDelete( QueueHandle_t xQueue )
{
    DEBUG("%s pid=%d queue=%p\n", __func__, thread_getpid(), xQueue);

    assert(xQueue != NULL);

    _queue_t* queue = (_queue_t*)xQueue;
    if (!(queue->stat)) {
        free(queue);
    }
}

BaseType_t IRAM_ATTR xQueueReset( QueueHandle_t xQueue )
{
    DEBUG("%s pid=%d queue=%p\n", __func__, thread_getpid(), xQueue);

    assert(xQueue != NULL);

    vTaskEnterCritical(0);

    _queue_t* queue = (_queue_t*)xQueue;
    queue->item_front = 0;
    queue->item_tail = 0;
    queue->item_level = 0;

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
    bool ctx_switch = proc->priority < thread_get_priority(thread_get_active());

    DEBUG("%s pid=%d queue=%p unlock waiting pid=%d switch=%d\n",
          __func__, thread_getpid(), xQueue, proc->pid, ctx_switch);

    if (ctx_switch) {
        vTaskExitCritical(0);
        /* sets only the sched_context_switch_request in ISRs */
        sched_switch(proc->priority);
    }
    else {
        vTaskExitCritical(0);
    }

    return pdPASS;
}

#if IS_USED(MODULE_ZTIMER_MSEC)

/* descriptor for timeout handling for a thread that is waiting in a queue */
typedef struct {
    thread_t *thread;       /* the thread */
    list_node_t *queue;     /* the queue in which it is waiting */
    bool timeout;           /* timeout occurred */
} _queue_waiting_thread_t;

static void _queue_timeout(void *arg)
{
    _queue_waiting_thread_t *wtd = arg;

    assert(wtd != NULL);
    assert(wtd->queue != NULL);
    assert(wtd->thread != NULL);

    vTaskEnterCritical(0);

    /* remove the thread from the waiting queue */
    list_node_t *node = (list_node_t *)&(wtd->thread->rq_entry);
    list_remove(wtd->queue, node);

    /* unblock the waintg thread */
    sched_set_status(wtd->thread, STATUS_PENDING);
    sched_context_switch_request =
        wtd->thread->priority < thread_get_priority(thread_get_active());

    wtd->timeout = true;

    vTaskExitCritical(0);
}
#endif

BaseType_t IRAM_ATTR _queue_generic_send(QueueHandle_t xQueue,
                                         const void * const pvItemToQueue,
                                         const BaseType_t xCopyPosition,
                                         TickType_t xTicksToWait,
                                         BaseType_t * const pxHigherPriorityTaskWoken)
{
    DEBUG("%s pid=%d prio=%d queue=%p pos=%d wait=%"PRIu32" woken=%p isr=%d\n", __func__,
          thread_getpid(), thread_get_priority(thread_get_active()),
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
                ctx_switch = proc->priority < thread_get_priority(thread_get_active());

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
            thread_t *me = thread_get_active();
            sched_set_status(me, STATUS_MUTEX_BLOCKED);
            /* waiting list is sorted by priority */
            thread_add_to_list(&queue->sending, me);

            DEBUG("%s pid=%d queue=%p suspended calling thread\n", __func__,
                  thread_getpid(), xQueue);

#if IS_USED(MODULE_ZTIMER_MSEC)
            _queue_waiting_thread_t wdt = { .queue = &queue->sending,
                                            .thread = me,
                                            .timeout = false };
            ztimer_t tm = { .callback = _queue_timeout,
                            .arg = &wdt };
            if (xTicksToWait < portMAX_DELAY) {
                ztimer_set(ZTIMER_MSEC, &tm, xTicksToWait * portTICK_PERIOD_MS);
            }
#else
            assert((xTicksToWait == 0) || (xTicksToWait == portMAX_DELAY));
#endif
            vTaskExitCritical(0);
            thread_yield_higher();

#if IS_USED(MODULE_ZTIMER_MSEC)
            vTaskEnterCritical(0);
            if (xTicksToWait < portMAX_DELAY) {
                ztimer_remove(ZTIMER_MSEC, &tm);
                if (wdt.timeout) {
                    vTaskExitCritical(0);
                    return errQUEUE_FULL;
                }
            }
            vTaskExitCritical(0);
#endif
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
    DEBUG("%s pid=%d prio=%d queue=%p wait=%"PRIu32" peek=%u woken=%p isr=%d\n", __func__,
          thread_getpid(), thread_get_priority(thread_get_active()),
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
            bool ctx_switch = proc->priority < thread_get_priority(thread_get_active());

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
            thread_t *me = thread_get_active();
            sched_set_status(me, STATUS_MUTEX_BLOCKED);
            /* waiting list is sorted by priority */
            thread_add_to_list(&queue->receiving, me);

            DEBUG("%s pid=%d queue=%p suspended calling thread\n", __func__,
                  thread_getpid(), xQueue);

#if IS_USED(MODULE_ZTIMER_MSEC)
            _queue_waiting_thread_t wdt = { .queue = &queue->receiving,
                                            .thread = me,
                                            .timeout = false };
            ztimer_t tm = { .callback = _queue_timeout,
                            .arg = &wdt };
            if (xTicksToWait < portMAX_DELAY) {
                ztimer_set(ZTIMER_MSEC, &tm, xTicksToWait * portTICK_PERIOD_MS);
            }
#else
            assert((xTicksToWait == 0) || (xTicksToWait == portMAX_DELAY));
#endif
            vTaskExitCritical(0);
            thread_yield_higher();

#if IS_USED(MODULE_ZTIMER_MSEC)
            vTaskEnterCritical(0);
            if (xTicksToWait < portMAX_DELAY) {
                ztimer_remove(ZTIMER_MSEC, &tm);
                if (wdt.timeout) {
                    vTaskExitCritical(0);
                    return errQUEUE_FULL;
                }
            }
            vTaskExitCritical(0);
#endif
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
    DEBUG("%s pid=%d prio=%d queue=%p wait=%"PRIu32" pos=%d\n", __func__,
          thread_getpid(), thread_get_priority(thread_get_active()),
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
          thread_getpid(), thread_get_priority(thread_get_active()),
          xQueue, xCopyPosition, pxHigherPriorityTaskWoken);

    return _queue_generic_send(xQueue, pvItemToQueue, xCopyPosition,
                               0, pxHigherPriorityTaskWoken);
}

BaseType_t IRAM_ATTR xQueueGenericReceive (QueueHandle_t xQueue,
                                           void * const pvBuffer,
                                           TickType_t xTicksToWait,
                                           const BaseType_t xJustPeeking)
{
    DEBUG("%s pid=%d prio=%d queue=%p wait=%"PRIu32" peek=%d\n", __func__,
          thread_getpid(), thread_get_priority(thread_get_active()),
          xQueue, xTicksToWait, xJustPeeking);

    return _queue_generic_recv(xQueue, pvBuffer, xTicksToWait,
                               xJustPeeking, NULL);
}

BaseType_t IRAM_ATTR xQueueReceiveFromISR (QueueHandle_t xQueue,
                                           void * const pvBuffer,
                                           BaseType_t * const pxHigherPriorityTaskWoken)
{
    DEBUG("%s pid=%d prio=%d queue=%p woken=%p\n", __func__,
          thread_getpid(), thread_get_priority(thread_get_active()),
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
    DEBUG("%s pid=%d prio=%d queue=%p woken=%p\n", __func__,
          thread_getpid(), thread_get_priority(thread_get_active()),
          xQueue, pxHigherPriorityTaskWoken);

    return _queue_generic_send(xQueue, NULL, queueSEND_TO_BACK,
                               0, pxHigherPriorityTaskWoken);
}

#endif /* DOXYGEN */
