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
#include "xtimer.h"

#ifdef MCU_ESP32
#include "soc/soc.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MHZ 1000000

#ifdef MCU_ESP8266
#include "rom/ets_sys.h"

#define PRO_CPU_NUM (0)
#endif

/**
 * @brief   Architecture specific data of thread control blocks
 */
typedef struct {
    uint32_t saved_int_state;
    uint32_t critical_nesting;
} thread_arch_ext_t;

volatile thread_arch_ext_t threads_arch_exts[KERNEL_PID_LAST + 1] = {};

BaseType_t xTaskCreatePinnedToCore (TaskFunction_t pvTaskCode,
                                    const char * const pcName,
                                    const uint32_t usStackDepth,
                                    void * const pvParameters,
                                    UBaseType_t uxPriority,
                                    TaskHandle_t * const pvCreatedTask,
                                    const BaseType_t xCoreID)
{
    /* FreeRTOS priority values have to be inverted */
    uxPriority = SCHED_PRIO_LEVELS - uxPriority - 1;

    DEBUG("%s name=%s size=%d prio=%d pvCreatedTask=%p ",
          __func__, pcName, usStackDepth, uxPriority, pvCreatedTask);

    char* stack = malloc(usStackDepth + sizeof(thread_t));

    if (!stack) {
        LOG_TAG_ERROR("freertos", "not enough memory to create task %s with "
                      "stack size of %d bytes\n", pcName, usStackDepth);
        abort();
        return pdFALSE;
    }
    kernel_pid_t pid = thread_create(stack,
                                     usStackDepth + sizeof(thread_t),
                                     uxPriority,
                                     THREAD_CREATE_WOUT_YIELD |
                                     THREAD_CREATE_STACKTEST,
                                     (thread_task_func_t)pvTaskCode,
                                     pvParameters, pcName);
    DEBUG("pid=%d\n", pid);

    if (pvCreatedTask) {
        *pvCreatedTask = (TaskHandle_t)(0L + pid);
    }

    return (pid < 0) ? pdFALSE : pdTRUE;
}

BaseType_t xTaskCreate (TaskFunction_t pvTaskCode,
                        const char * const pcName,
                        const uint32_t usStackDepth,
                        void * const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t * const pvCreatedTask)
{
    return xTaskCreatePinnedToCore (pvTaskCode,
                                    pcName,
                                    usStackDepth,
                                    pvParameters,
                                    uxPriority,
                                    pvCreatedTask,
                                    PRO_CPU_NUM);
}

void vTaskDelete (TaskHandle_t xTaskToDelete)
{
    extern volatile thread_t *sched_active_thread;
    DEBUG("%s pid=%d task=%p\n", __func__, thread_getpid(), xTaskToDelete);

    assert(xTaskToDelete != NULL);

    uint32_t pid = (uint32_t)xTaskToDelete;

    /* remove old task from scheduling */
    thread_t* thread = (thread_t*)sched_threads[pid];
    sched_set_status(thread, STATUS_STOPPED);
    sched_threads[pid] = NULL;
    sched_num_threads--;
    sched_active_thread = NULL;

    /* determine the new running task */
    sched_run();
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    DEBUG("%s pid=%d\n", __func__, thread_getpid());

    uint32_t pid = thread_getpid();
    return (TaskHandle_t)pid;
}

void vTaskDelay( const TickType_t xTicksToDelay )
{
    DEBUG("%s xTicksToDelay=%d\n", __func__, xTicksToDelay);
#if defined(MODULE_ESP_WIFI_ANY)
    uint64_t us = xTicksToDelay * MHZ / xPortGetTickRateHz();
    xtimer_usleep(us);
#endif
}

TickType_t xTaskGetTickCount (void)
{
    return system_get_time() / USEC_PER_MSEC / portTICK_PERIOD_MS;
}

void vTaskEnterCritical( portMUX_TYPE *mux )
{
#ifdef MCU_ESP8266
    /* we have to return on NMI */
    if (NMIIrqIsOn) {
        return;
    }
#endif /* MCU_ESP8266 */

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
#ifdef MCU_ESP8266
    /* we have to return on NMI */
    if (NMIIrqIsOn) {
        return;
    }
#endif /* MCU_ESP8266 */

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
    DEBUG("%s xTicksToJump=%d\n", __func__, xTicksToJump);
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

#endif /* DOXYGEN */
