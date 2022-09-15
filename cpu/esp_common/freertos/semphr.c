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
#include "irq_arch.h"
#include "log.h"
#include "mutex.h"
#include "rmutex.h"

#include "freertos/FreeRTOS.h"

/*
 * In FreeRTOS different types of semaphores, mutexes and queues are all
 * mapped to a single generic queue type. With all these different types,
 * single functions for send, receive, give and take are then used. To be
 * able to dsitinguish between these different types in RIOT, we need typed
 * objects.
 */
typedef struct {
    uint8_t      type;   /* type of the mutex, MUST be the first element */
    kernel_pid_t pid;    /* PID of the holder if the mutex is locked */
    mutex_t      mutex;  /* the RIOT mutex */
} _sem_t;

typedef struct {
    uint8_t      type;   /* type of the mutex, MUST be the first element */
    kernel_pid_t pid;    /* PID of the holder if the mutex is locked */
    rmutex_t     rmutex; /* the RIOT mutex */
} _rsem_t;

SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    _sem_t* _tmp = (_sem_t*)malloc (sizeof(_sem_t));
    _tmp->type = queueQUEUE_TYPE_MUTEX;
    _tmp->pid = KERNEL_PID_UNDEF;
    mutex_init(&_tmp->mutex);

    DEBUG("%s mutex=%p\n", __func__, _tmp);
    return _tmp;
}

void vSemaphoreDelete(SemaphoreHandle_t xSemaphore)
{
    DEBUG("%s mutex=%p\n", __func__, xSemaphore);

    assert(xSemaphore != NULL);
    free(xSemaphore);
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore)
{
    DEBUG("%s mutex=%p\n", __func__, xSemaphore);

    /* if scheduler is not running, we must not lock the mutex */
    if (thread_getpid() == KERNEL_PID_UNDEF) {
        return pdPASS;
    }

    assert(xSemaphore != NULL);

    _sem_t* sem = (_sem_t*)xSemaphore;

    switch (sem->type) {
        case queueQUEUE_TYPE_MUTEX:
            mutex_unlock(&sem->mutex);
            if (sem->mutex.queue.next == NULL) {
                sem->pid = KERNEL_PID_UNDEF;
            }
            break;
        case queueQUEUE_TYPE_RECURSIVE_MUTEX:
            return xSemaphoreGiveRecursive(xSemaphore);
        default:
            return xQueueGenericSend(xSemaphore, NULL, 0, queueSEND_TO_BACK);
    }

    return pdTRUE;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,
                          TickType_t xTicksToWait)
{
    DEBUG("%s mutex=%p wait=%"PRIu32"\n", __func__, xSemaphore, xTicksToWait);

    /* if scheduler is not running, we must not lock the mutex */
    if (thread_getpid() == KERNEL_PID_UNDEF) {
        return pdPASS;
    }

    assert(xSemaphore != NULL);

    _sem_t* sem = (_sem_t*)xSemaphore;

    switch (sem->type) {
        case queueQUEUE_TYPE_MUTEX:
        {
            if (xTicksToWait == 0) {
                if (mutex_trylock(&sem->mutex) == 1) {
                    sem->pid = thread_getpid();
                    return pdPASS;
                }
                else {
                    return pdFAIL;
                }
            }
            else {
                mutex_lock(&sem->mutex);
                sem->pid = thread_getpid();
                /* TODO timeout handling */
                return pdTRUE;
            }
            break;
        }
        case queueQUEUE_TYPE_RECURSIVE_MUTEX:
            return xSemaphoreTakeRecursive(xSemaphore, xTicksToWait);

        default:
            return xQueueGenericReceive(xSemaphore, NULL, xTicksToWait, pdFALSE);
    }
}

SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void)
{
    _rsem_t* _tmp = (_rsem_t*)malloc (sizeof(_rsem_t));
    _tmp->type = queueQUEUE_TYPE_RECURSIVE_MUTEX;
    _tmp->pid = KERNEL_PID_UNDEF;
    rmutex_init(&_tmp->rmutex);

    DEBUG("%s rmutex=%p\n", __func__, _tmp);

    return _tmp;
}

BaseType_t xSemaphoreGiveRecursive(SemaphoreHandle_t xSemaphore)
{
    DEBUG("%s rmutex=%p\n", __func__, xSemaphore);

    /* if scheduler is not running, we must not lock the mutex */
    if (thread_getpid() == KERNEL_PID_UNDEF) {
        return pdPASS;
    }

    _rsem_t* rsem = (_rsem_t*)xSemaphore;

    assert(rsem != NULL);
    assert(rsem->type == queueQUEUE_TYPE_RECURSIVE_MUTEX);

    rmutex_unlock(&rsem->rmutex);
    if (rsem->rmutex.mutex.queue.next == NULL) {
        rsem->pid = KERNEL_PID_UNDEF;
    }
    return pdTRUE;
}

BaseType_t xSemaphoreTakeRecursive(SemaphoreHandle_t xSemaphore,
                                   TickType_t xTicksToWait)
{
    DEBUG("%s rmutex=%p wait=%"PRIu32"\n", __func__, xSemaphore, xTicksToWait);

    /* if scheduler is not running, we must not lock the rmutex */
    if (thread_getpid() == KERNEL_PID_UNDEF) {
        return pdPASS;
    }

    _rsem_t* rsem = (_rsem_t*)xSemaphore;

    assert(rsem != NULL);
    assert(rsem->type == queueQUEUE_TYPE_RECURSIVE_MUTEX);

    BaseType_t ret = pdTRUE;

    if (xTicksToWait == 0) {
        ret = (rmutex_trylock(&rsem->rmutex) == 1) ? pdPASS : pdFAIL;
        if (ret == pdPASS) {
            rsem->pid = thread_getpid();
        }
    }
    else {
        rmutex_lock(&rsem->rmutex);
        rsem->pid = thread_getpid();
        /* TODO timeout handling */
    }

    return ret;
}

TaskHandle_t xSemaphoreGetMutexHolder(SemaphoreHandle_t xMutex)
{
    DEBUG("%s mutex=%p\n", __func__, xMutex);

    assert(xMutex != NULL);
    return (TaskHandle_t)(0L + ((_sem_t*)xMutex)->pid);
}

void vPortCPUAcquireMutex(portMUX_TYPE *mux)
{
    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority, mux);
    critical_enter();
    mutex_lock(mux); /* lock the mutex with interrupts disabled */
    critical_exit();
}

void vPortCPUReleaseMutex(portMUX_TYPE *mux)
{
    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority, mux);
    critical_enter();
    mutex_unlock(mux); /* unlock the mutex with interrupts disabled */
    critical_exit();
}

#endif /* DOXYGEN */
