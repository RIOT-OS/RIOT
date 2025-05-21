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
#include "log.h"
#include "syscalls.h"
#include "thread.h"
#if defined(MODULE_ZTIMER_MSEC) || defined(MODULE_ZTIMER_USEC)
#include "ztimer.h"
#endif
#include "timex.h"

#ifdef CPU_FAM_ESP32
#include "soc/soc.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef CPU_ESP8266
#include "rom/ets_sys.h"
#endif

#ifndef PRO_CPU_NUM
#define PRO_CPU_NUM (0)
#endif

/**
 * @brief   Architecture specific data of thread control blocks
 */
typedef struct {
    uint32_t saved_int_state;
    uint32_t critical_nesting;
    uint32_t notification_value;
    bool notification_waiting;
    bool notification_pending;
} thread_arch_ext_t;

volatile thread_arch_ext_t threads_arch_exts[KERNEL_PID_LAST + 1] = { };

BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode,
                                   const char * const pcName,
                                   const uint32_t usStackDepth,
                                   void * const pvParameters,
                                   UBaseType_t uxPriority,
                                   TaskHandle_t * const pvCreatedTask,
                                   const BaseType_t xCoreID)
{
    assert(xCoreID == 0 || xCoreID == tskNO_AFFINITY);

    /* FreeRTOS priority values have to be inverted */
    uxPriority = SCHED_PRIO_LEVELS - uxPriority - 1;

    DEBUG("%s name=%s size=%"PRIu32" prio=%u pvCreatedTask=%p xCoreId=%d\n",
          __func__, pcName, usStackDepth, uxPriority, pvCreatedTask, xCoreID);

    char* stack = malloc(usStackDepth + sizeof(thread_t));

    if (!stack) {
        LOG_TAG_ERROR("freertos", "not enough memory to create task %s with "
                      "stack size of %"PRIu32" bytes\n", pcName, usStackDepth);
        abort();
        return pdFALSE;
    }
    kernel_pid_t pid = thread_create(stack,
                                     usStackDepth + sizeof(thread_t),
                                     uxPriority,
                                     THREAD_CREATE_WOUT_YIELD |
                                     0,
                                     (void *)pvTaskCode,
                                     pvParameters, pcName);
    DEBUG("pid=%d\n", pid);

    if (pvCreatedTask) {
        *pvCreatedTask = (TaskHandle_t)(0L + pid);
    }

    return (pid < 0) ? pdFALSE : pdTRUE;
}

BaseType_t xTaskCreate(TaskFunction_t pvTaskCode,
                       const char * const pcName,
                       const uint32_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pvCreatedTask)
{
    return xTaskCreatePinnedToCore(pvTaskCode,
                                   pcName,
                                   usStackDepth,
                                   pvParameters,
                                   uxPriority,
                                   pvCreatedTask,
                                   PRO_CPU_NUM);
}

void vTaskDelete(TaskHandle_t xTaskToDelete)
{
    extern volatile thread_t *sched_active_thread;
    DEBUG("%s pid=%d task=%p\n", __func__, thread_getpid(), xTaskToDelete);

    uint32_t pid = (uint32_t)xTaskToDelete;
    if (pid == 0) {
        pid = thread_getpid();
    }
    assert(pid_is_valid(pid));

    /* remove the task from scheduling */
    thread_t* thread = (thread_t*)sched_threads[pid];
    sched_set_status(thread, STATUS_STOPPED);
    sched_threads[pid] = NULL;
    sched_num_threads--;

    free(thread->stack_start);
}

void vTaskSuspend(TaskHandle_t xTaskToSuspend)
{
    extern volatile thread_t *sched_active_thread;
    DEBUG("%s pid=%d task=%p\n", __func__, thread_getpid(), xTaskToSuspend);

    thread_t *thread = thread_get((xTaskToSuspend == NULL) ? (uint32_t)thread_getpid()
                                                           : (uint32_t)xTaskToSuspend);
    assert(thread != NULL);

    /* set status to sleeping to suspend it */
    sched_set_status(thread, STATUS_SLEEPING);

    /* trigger rescheduling if a task suspends itself */
    if (xTaskToSuspend == NULL) {
        thread_yield_higher();
    }
}

void vTaskSuspendAll(void)
{
    /* TODO:
     * It has to be implemented once there is a mechanism in RIOT to suspend
     * the scheduler without disabling interrupts. At the moment it is a
     * placeholder to make the linker happy. */
}

void vTaskResume(TaskHandle_t xTaskToResume)
{
    extern volatile thread_t *sched_active_thread;
    DEBUG("%s pid=%d task=%p\n", __func__, thread_getpid(), xTaskToResume);

    uint32_t pid = (uint32_t)xTaskToResume;
    assert(pid_is_valid(pid));
    thread_wakeup (pid);
}

BaseType_t xTaskResumeAll(void)
{
    /* TODO */
    return pdFALSE;
}

void vTaskDelay(const TickType_t xTicksToDelay)
{
    DEBUG("%s xTicksToDelay=%"PRIu32"\n", __func__, xTicksToDelay);

#if defined(MODULE_ZTIMER_MSEC)
    uint64_t ms = xTicksToDelay * portTICK_PERIOD_MS;
    ztimer_sleep(ZTIMER_MSEC, ms);
#elif defined(MODULE_ZTIMER_USEC)
    uint64_t us = xTicksToDelay * portTICK_PERIOD_MS * US_PER_MS;
    ztimer_sleep(ZTIMER_USEC, us);
#endif
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    DEBUG("%s pid=%d\n", __func__, thread_getpid());

    uint32_t pid = thread_getpid();
    return (TaskHandle_t)pid;
}

const char *pcTaskGetTaskName(TaskHandle_t xTaskToQuery)
{
    DEBUG("%s pid=%d task=%p\n", __func__, thread_getpid(), xTaskToQuery);

    thread_t *thread = thread_get((xTaskToQuery == NULL) ? (uint32_t)thread_getpid()
                                                         : (uint32_t)xTaskToQuery);
    assert(thread != NULL);
    return thread->name;
}

#ifdef DEVELHELP
UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask)
{
    DEBUG("%s pid=%d task=%p\n", __func__, thread_getpid(), xTask);

    thread_t *thread = thread_get((xTask == NULL) ? (uint32_t)thread_getpid()
                                                  : (uint32_t)xTask);
    assert(thread != NULL);
    return thread_measure_stack_free(thread);
}

void *pvTaskGetThreadLocalStoragePointer(TaskHandle_t xTaskToQuery,
                                         BaseType_t xIndex)
{
    (void)xTaskToQuery;
    (void)xIndex;

    /* TODO define TLS using thread_arch_t */
    return NULL;
}

void vTaskSetThreadLocalStoragePointerAndDelCallback(TaskHandle_t xTaskToSet,
                                                     BaseType_t xIndex,
                                                     void *pvValue,
                                                     TlsDeleteCallbackFunction_t pvDelCallback)
{
    (void)xTaskToSet;
    (void)xIndex;
    (void)pvValue;
    (void)pvDelCallback;

    /* TODO define TLS using thread_arch_t */
}
#endif /* DEVELHELP */

TickType_t xTaskGetTickCount (void)
{
    return system_get_time() / US_PER_MS / portTICK_PERIOD_MS;
}

void vTaskEnterCritical( portMUX_TYPE *mux )
{
#ifdef CPU_ESP8266
    /* we have to return on NMI */
    if (NMIIrqIsOn) {
        return;
    }
#endif /* CPU_ESP8266 */

    /* disable interrupts */
    uint32_t state = irq_disable();

    /* determine calling thread pid (can't fail) */
    kernel_pid_t my_pid = thread_getpid();

    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          my_pid, sched_threads[my_pid]->priority, mux);

    /* acquire the mutex with interrupts disabled */
    if (mux) {
        /* Locking the given mutex does not work here, as this function can also
           be called in the interrupt context. Therefore, the given mutex is not
           used. Instead, the basic default FreeRTOS mechanism for critical
           sections is used by simply disabling interrupts. Since context
           switches for the ESPs are also based on interrupts, there is no
           possibility that another thread will enter the critical section
           once the interrupts are disabled. */
        /* mutex_lock(mux); */ /* TODO should be only a spin lock */
    }

    /* increment nesting counter and save old interrupt level */
    threads_arch_exts[my_pid].critical_nesting++;
    if (threads_arch_exts[my_pid].critical_nesting == 1) {
        threads_arch_exts[my_pid].saved_int_state = state;
    }
}

void vTaskExitCritical( portMUX_TYPE *mux )
{
#ifdef CPU_ESP8266
    /* we have to return on NMI */
    if (NMIIrqIsOn) {
        return;
    }
#endif /* CPU_ESP8266 */

    /* determine calling thread pid (can't fail) */
    kernel_pid_t my_pid = thread_getpid();

    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          my_pid, sched_threads[my_pid]->priority, mux);

    /* release the mutex with interrupts disabled */
    if (mux) {
        /* mutex_unlock(mux); */ /* TODO should be only a spin lock */
    }

    /* decrement nesting counter and restore old interrupt level */
    if (threads_arch_exts[my_pid].critical_nesting) {
        threads_arch_exts[my_pid].critical_nesting--;
        if (threads_arch_exts[my_pid].critical_nesting == 0) {
            irq_restore(threads_arch_exts[my_pid].saved_int_state);
        }
    }
}

void vTaskStepTick(const TickType_t xTicksToJump)
{
    DEBUG("%s xTicksToJump=%"PRIu32"\n", __func__, xTicksToJump);
    /*
     * TODO:
     * At the moment, only the calling task is set to sleep state. Usually, the
     * complete system should sleep but not only the task.
     */
    vTaskDelay(xTicksToJump);
}

TickType_t prvGetExpectedIdleTime(void)
{
    DEBUG("%s\n", __func__);
    /*
     * TODO:
     * Since we are not able to estimate the time the system will be idle,
     * we simply return 0.
     */
    return 0;
}

BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue,
                       eNotifyAction eAction)
{
    uint32_t pid = (uint32_t)xTaskToNotify;
    thread_t* thread = thread_get(pid);

    DEBUG("%s task=%d notifies=%"PRIu32" value=%"PRIu32" action=%u\n", __func__,
          thread_getpid(), pid, ulValue, eAction);

    assert(pid_is_valid(pid));
    assert(thread != NULL);

    vTaskEnterCritical(0);

    switch (eAction) {
        case eSetBits:
            threads_arch_exts[pid].notification_value |= ulValue;
            break;
        case eIncrement:
            threads_arch_exts[pid].notification_value++;
            break;
        case eSetValueWithoutOverwrite:
            if (threads_arch_exts[pid].notification_pending) {
                /* if a notificatoin is pending, return with error */
                vTaskExitCritical(0);
                return pdFALSE;
            }
            /* fallthrough */
        case eSetValueWithOverwrite:
            threads_arch_exts[pid].notification_value = ulValue;
            break;
        default:
            /* no action */
            break;
    }

    if (threads_arch_exts[pid].notification_waiting) {
        /* if the task is waiting for a notification, wake it up */
        sched_set_status(thread, STATUS_PENDING);
        vTaskExitCritical(0);
        thread_yield_higher();
        return pdTRUE;
    }

    threads_arch_exts[pid].notification_pending = true;

    vTaskExitCritical(0);
    return pdTRUE;
}

BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry,
                           uint32_t ulBitsToClearOnExit,
                           uint32_t *pulNotificationValue,
                           TickType_t xTicksToWait)
{
    kernel_pid_t pid = thread_getpid();

    DEBUG("%s task=%d entry=%08"PRIx32" exit=%08"PRIx32" wait=%"PRIu32"\n", __func__,
          pid, ulBitsToClearOnEntry, ulBitsToClearOnExit, xTicksToWait);

    assert(pid_is_valid(pid));
    assert((xTicksToWait != 0) && !irq_is_in());

    vTaskEnterCritical(0);

    if (!threads_arch_exts[pid].notification_pending) {
        /* bits to clear on entry if notification was not pending */
        threads_arch_exts[pid].notification_value ^= ulBitsToClearOnEntry;

        /* suspend the calling thread to wait for notification */
        threads_arch_exts[pid].notification_waiting = true;
        thread_t *me = thread_get_active();
        sched_set_status(me, STATUS_SLEEPING);

        DEBUG("%s pid=%d suspend calling thread\n", __func__, pid);

#if IS_USED(MODULE_ZTIMER_MSEC)
        ztimer_t tm = { };
        uint32_t to = xTicksToWait * portTICK_PERIOD_MS;
        /* set the timeout if given */
        if (xTicksToWait < portMAX_DELAY) {
            ztimer_set_timeout_flag(ZTIMER_MSEC, &tm, to);
            ztimer_set_wakeup(ZTIMER_MSEC, &tm, to + 1, pid);
        }
#else
        assert(xTicksToWait == portMAX_DELAY);
#endif
        vTaskExitCritical(0);
        thread_yield_higher();

#if IS_USED(MODULE_ZTIMER_MSEC)
        vTaskEnterCritical(0);
        if (xTicksToWait < portMAX_DELAY) {
            ztimer_remove(ZTIMER_MSEC, &tm);
            if (me->flags & THREAD_FLAG_TIMEOUT) {
                vTaskExitCritical(0);
                return pdFALSE;
            }
        }
#else
        assert(xTicksToWait == portMAX_DELAY);
#endif
    }

    if (pulNotificationValue) {
        /* save the notification value before clearing bits on exit */
        *pulNotificationValue = threads_arch_exts[pid].notification_value;
    }
    threads_arch_exts[pid].notification_value ^= ulBitsToClearOnExit;

    vTaskExitCritical(0);
    return pdTRUE;
}

BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify)
{
    DEBUG("%s pid=%d task=%p\n", __func__, thread_getpid(), xTaskToNotify);
    vTaskNotifyGiveFromISR(xTaskToNotify, NULL);
    return pdPASS;
}

void vTaskNotifyGiveFromISR(TaskHandle_t xTaskToNotify,
                            BaseType_t *pxHigherPriorityTaskWoken)
{
    uint32_t pid = (uint32_t)xTaskToNotify;
    thread_t* thread = thread_get(pid);

    DEBUG("%s task=%d notifies=%"PRIu32"\n", __func__, thread_getpid(), pid);

    assert(pid_is_valid(pid));
    assert(thread);

    vTaskEnterCritical(0);

    threads_arch_exts[pid].notification_value++;

    if (threads_arch_exts[pid].notification_waiting) {
        /* if the task is waiting for notification, set its status to pending */
        sched_set_status(thread, STATUS_PENDING);

        if (thread->priority < sched_threads[thread_getpid()]->priority) {
            /* a context switch is needed */
            if (pxHigherPriorityTaskWoken) {
                *pxHigherPriorityTaskWoken = pdTRUE;
            }

            vTaskExitCritical(0);
            /* sets only the sched_context_switch_request in ISRs */
            thread_yield_higher();
            return;
        }
    }
    vTaskExitCritical(0);
}

uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit,
                          TickType_t xTicksToWait)
{
    DEBUG("%s pid=%d\n", __func__, thread_getpid());

    kernel_pid_t pid = thread_getpid();

    assert(pid_is_valid(pid));

    vTaskEnterCritical(0);

    uint32_t prev_value = threads_arch_exts[pid].notification_value;

    if (prev_value) {
        /* notification was pending */
        threads_arch_exts[pid].notification_value--;
        vTaskExitCritical(0);
    }
    else if (xTicksToWait == 0 || irq_is_in()) {
        /* if delaying is not allowed */
        DEBUG("%s pid=%d delaying not allowed\n", __func__, thread_getpid());
        assert(0);
    }
    else {
        /* suspend the calling thread to wait for notification */
        threads_arch_exts[pid].notification_waiting = true;
        thread_t *me = thread_get_active();
        sched_set_status(me, STATUS_SLEEPING);

        DEBUG("%s pid=%d suspend calling thread\n", __func__, thread_getpid());

        vTaskExitCritical(0);
        thread_yield_higher();

        /* TODO timeout handling with xTicksToWait */
        DEBUG("%s pid=%d continue calling thread\n", __func__, thread_getpid());
    }
    threads_arch_exts[pid].notification_waiting = false;
    if (xClearCountOnExit) {
        threads_arch_exts[pid].notification_value = 0;
    }
    return prev_value;
}

#endif /* DOXYGEN */
