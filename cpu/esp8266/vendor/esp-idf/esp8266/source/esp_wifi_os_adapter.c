// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

#include "esp_libc.h"
#include "esp_system.h"
#include "esp_attr.h"
#include "esp_wifi_os_adapter.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#ifdef RIOT_OS
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "xtensa/xtensa_api.h"

#endif /* RIOT_OS*/

#include "nvs.h"

#if defined(CONFIG_NEWLIB_LIBRARY_LEVEL_NORMAL) || defined(CONFIG_NEWLIB_LIBRARY_LEVEL_NANO)
#include "esp_newlib.h"
#endif

#ifdef RIOT_OS

extern void vPortYield(void);
extern void vPortYieldFromInt(void);
void  thread_yield_higher(void);

#endif /* RIOT_OS*/

static void *task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio)
{
    portBASE_TYPE ret;
    xTaskHandle handle;
    ret = xTaskCreate(task_func, name, stack_depth, param, prio, &handle);

    return ret == pdPASS ? handle : NULL;
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
}

static void task_yield_wrapper(void)
{
#ifdef RIOT_OS
    thread_yield_higher();
#else
    portYIELD();
#endif
}

static void task_yield_from_isr_wrapper(void)
{
#ifdef RIOT_OS
    thread_yield_higher();
#else
    portYIELD();
#endif
}

static void task_delay_wrapper(uint32_t tick)
{
    vTaskDelay(tick);
}

static void* task_get_current_task_wrapper(void)
{
    return (void *)xTaskGetCurrentTaskHandle();
}

static uint32_t task_get_max_priority_wrapper(void)
{
    return (uint32_t)(configMAX_PRIORITIES-1);
}

static uint32_t task_ms_to_tick_wrapper(uint32_t ms)
{
    return (uint32_t)(ms / portTICK_RATE_MS);
}

static void task_suspend_all_wrapper(void)
{
#ifndef RIOT_OS
    /* there is no equivalent in RIOT */
    vTaskSuspendAll();
#endif
}

static void task_resume_all_wrapper(void)
{
#ifndef RIOT_OS
    /* there is no equivalent in RIOT */
    xTaskResumeAll();
#endif
}

static void os_init_wrapper(void)
{
#ifdef RIOT_OS
    extern void esp_riot_init(void);
    esp_riot_init();
#else
#if defined(CONFIG_NEWLIB_LIBRARY_LEVEL_NORMAL) || defined(CONFIG_NEWLIB_LIBRARY_LEVEL_NANO)
    esp_newlib_init();
#endif
#endif
}

static void os_start_wrapper(void)
{
#ifdef RIOT_OS
    extern void esp_riot_start(void);
    esp_riot_start();
#else
    vTaskStartScheduler();
#endif
}

static void *semphr_create_wrapper(uint32_t max, uint32_t init)
{
    return (void *)xSemaphoreCreateCounting(max, init);
}

static void semphr_delete_wrapper(void *semphr)
{
    vSemaphoreDelete(semphr);
}

static bool semphr_take_from_isr_wrapper(void *semphr, int *hptw)
{
    signed portBASE_TYPE ret;

    ret = xSemaphoreTakeFromISR(semphr, (signed portBASE_TYPE *)hptw);

    return ret == pdPASS ? true : false;
}

static bool semphr_give_from_isr_wrapper(void *semphr, int *hptw)
{
    signed portBASE_TYPE ret;

    ret = xSemaphoreGiveFromISR(semphr, (signed portBASE_TYPE *)hptw);

    return ret == pdPASS ? true : false;
}

static bool semphr_take_wrapper(void *semphr, uint32_t block_time_tick)
{
    signed portBASE_TYPE ret;

    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        ret = xSemaphoreTake(semphr, portMAX_DELAY);
    } else {
        ret = xSemaphoreTake(semphr, block_time_tick);
    }

    return ret == pdPASS ? true : false;
}

static bool semphr_give_wrapper(void *semphr)
{
    signed portBASE_TYPE ret;

    ret = xSemaphoreGive(semphr);

    return ret == pdPASS ? true : false;
}

static void *mutex_create_wrapper(void)
{
    return (void *)xSemaphoreCreateRecursiveMutex();
}

static void mutex_delete_wrapper(void *mutex)
{
    vSemaphoreDelete(mutex);
}

static bool mutex_lock_wrapper(void *mutex)
{
    signed portBASE_TYPE ret;

    ret = xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

    return ret == pdPASS ? true : false;
}

static bool mutex_unlock_wrapper(void *mutex)
{
    signed portBASE_TYPE ret;

    ret = xSemaphoreGiveRecursive(mutex);

    return ret == pdPASS ? true : false;
}

static void *queue_create_wrapper(uint32_t queue_len, uint32_t item_size)
{
    return (void *)xQueueCreate(queue_len, item_size);
}

static void queue_delete_wrapper(void *queue)
{
    vQueueDelete(queue);
}

static bool queue_send_wrapper(void *queue, void *item, uint32_t block_time_tick, uint32_t pos)
{
    signed portBASE_TYPE ret;
    BaseType_t os_pos;

    if (pos == OSI_QUEUE_SEND_BACK)
        os_pos = queueSEND_TO_BACK;
    else if (pos == OSI_QUEUE_SEND_FRONT)
        os_pos = queueSEND_TO_FRONT;
    else
        os_pos = queueOVERWRITE;

    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        ret = xQueueGenericSend(queue, item, portMAX_DELAY, os_pos);
    } else {
        ret = xQueueGenericSend(queue, item, block_time_tick, os_pos);
    }

    return ret == pdPASS ? true : false;
}

static bool queue_send_from_isr_wrapper(void *queue, void *item, int *hptw)
{
    signed portBASE_TYPE ret;

    ret = xQueueSendFromISR(queue, item, (signed portBASE_TYPE *)hptw);

    return ret == pdPASS ? true : false;
}

static bool queue_recv_wrapper(void *queue, void *item, uint32_t block_time_tick)
{
    signed portBASE_TYPE ret;

    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        ret = xQueueReceive(queue, item, portMAX_DELAY);
    } else {
        ret = xQueueReceive(queue, item, block_time_tick);
    }

    return ret == pdPASS ? true : false;
}

static bool queue_recv_from_isr_wrapper(void *queue, void *item, int *hptw)
{
    signed portBASE_TYPE ret;

    ret = xQueueReceiveFromISR(queue, item, (signed portBASE_TYPE *)hptw);

    return ret == pdPASS ? true : false;
}

static uint32_t queue_msg_waiting_wrapper(void *queue)
{
    return (uint32_t)uxQueueMessagesWaiting(queue);
}

static uint32_t get_free_heap_size_wrapper(void)
{
    return (uint32_t)esp_get_free_heap_size();
}

static void *timer_create_wrapper(const char *name, uint32_t period_ticks, bool auto_load, void *arg, void (*cb)(void *timer))
{
#ifdef MODULE_ESP_WIFI_ANY
    return xTimerCreate(name, period_ticks, auto_load, arg, (tmrTIMER_CALLBACK)cb);
#else
    return NULL;
#endif
}

static void *timer_get_arg_wrapper(void *timer)
{
#ifdef MODULE_ESP_WIFI_ANY
    return pvTimerGetTimerID(timer);
#else
    return false;
#endif
}

static bool timer_reset_wrapper(void *timer, uint32_t ticks)
{
#ifdef MODULE_ESP_WIFI_ANY
    return xTimerReset(timer, ticks);
#else
    return false;
#endif
}

static bool timer_stop_wrapper(void *timer, uint32_t ticks)
{
#ifdef MODULE_ESP_WIFI_ANY
    return xTimerStop(timer, ticks);
#else
    return false;
#endif
}

static bool timer_delete_wrapper(void *timer, uint32_t ticks)
{
#ifdef MODULE_ESP_WIFI_ANY
    return xTimerDelete(timer, ticks);
#else
    return false;
#endif
}

static void *malloc_wrapper(uint32_t s, uint32_t cap, const char *file, size_t line)
{
    uint32_t os_caps;

    if (cap & (OSI_MALLOC_CAP_8BIT | OSI_MALLOC_CAP_DMA))
        os_caps = MALLOC_CAP_8BIT | MALLOC_CAP_DMA;
    else
        os_caps = MALLOC_CAP_32BIT;

    return _heap_caps_malloc(s, os_caps, file, line);
}

static void *zalloc_wrapper(uint32_t s, uint32_t cap, const char *file, size_t line)
{
    uint32_t os_caps;

    if (cap & (OSI_MALLOC_CAP_8BIT | OSI_MALLOC_CAP_DMA))
        os_caps = MALLOC_CAP_8BIT | MALLOC_CAP_DMA;
    else
        os_caps = MALLOC_CAP_32BIT;

    return _heap_caps_zalloc(s, os_caps, file, line);
}

static void *realloc_wrapper(void *ptr, uint32_t s, uint32_t cap, const char *file, size_t line)
{
    uint32_t os_caps;

    if (cap & (OSI_MALLOC_CAP_8BIT | OSI_MALLOC_CAP_DMA))
        os_caps = MALLOC_CAP_8BIT | MALLOC_CAP_DMA;
    else
        os_caps = MALLOC_CAP_32BIT;

    return _heap_caps_realloc(ptr, s, os_caps, file, line);
}

static void *calloc_wrapper(uint32_t cnt, uint32_t s, uint32_t cap, const char *file, size_t line)
{
    uint32_t os_caps;

    if (cap & (OSI_MALLOC_CAP_8BIT | OSI_MALLOC_CAP_DMA))
        os_caps = MALLOC_CAP_8BIT | MALLOC_CAP_DMA;
    else
        os_caps = MALLOC_CAP_32BIT;

    return _heap_caps_calloc(cnt , s, os_caps, file, line);
}

static void free_wrapper(void *ptr, const char *file, size_t line)
{
    _heap_caps_free(ptr, file, line);
}

static void srand_wrapper(uint32_t seed)
{
    /* empty function */
}

static int32_t rand_wrapper(void)
{
    return (int32_t)esp_random();
}

void *osi_task_top_sp(void)
{
    thread_t *active_thread = thread_get_active();
    return active_thread ? active_thread->sp : 0;
}

const wifi_osi_funcs_t s_wifi_osi_funcs = {
    .version = ESP_WIFI_OS_ADAPTER_VERSION,

    .task_create = task_create_wrapper,
    .task_delete = task_delete_wrapper,
    .task_yield = task_yield_wrapper,
    .task_yield_from_isr = task_yield_from_isr_wrapper,
    .task_delay = task_delay_wrapper,
    .task_get_current_task = task_get_current_task_wrapper,
    .task_get_max_priority = task_get_max_priority_wrapper,

    .task_ms_to_tick = task_ms_to_tick_wrapper,

    .task_suspend_all = task_suspend_all_wrapper,
    .task_resume_all = task_resume_all_wrapper,

    .os_init = os_init_wrapper,
    .os_start = os_start_wrapper,

    .semphr_create = semphr_create_wrapper,
    .semphr_delete = semphr_delete_wrapper,
    .semphr_take_from_isr = semphr_take_from_isr_wrapper,
    .semphr_give_from_isr = semphr_give_from_isr_wrapper,
    .semphr_take = semphr_take_wrapper,
    .semphr_give = semphr_give_wrapper,

    .mutex_create = mutex_create_wrapper,
    .mutex_delete = mutex_delete_wrapper,
    .mutex_lock = mutex_lock_wrapper,
    .mutex_unlock = mutex_unlock_wrapper,

    .queue_create = queue_create_wrapper,
    .queue_delete = queue_delete_wrapper,
    .queue_send = queue_send_wrapper,
    .queue_send_from_isr = queue_send_from_isr_wrapper,
    .queue_recv = queue_recv_wrapper,
    .queue_recv_from_isr = queue_recv_from_isr_wrapper,
    .queue_msg_waiting = queue_msg_waiting_wrapper,

    .timer_create = timer_create_wrapper,
    .timer_get_arg = timer_get_arg_wrapper,
    .timer_reset = timer_reset_wrapper,
    .timer_stop = timer_stop_wrapper,
    .timer_delete = timer_delete_wrapper,

    .malloc = malloc_wrapper,
    .zalloc = zalloc_wrapper,
    .realloc = realloc_wrapper,
    .calloc = calloc_wrapper,
    .free = free_wrapper,
    .get_free_heap_size = get_free_heap_size_wrapper,

    .srand = srand_wrapper,
    .rand = rand_wrapper,

    .nvs_set_i8 = nvs_set_i8,
    .nvs_get_i8 = nvs_get_i8,
    .nvs_set_u8 = nvs_set_u8,
    .nvs_get_u8 = nvs_get_u8,
    .nvs_set_u16 = nvs_set_u16,
    .nvs_get_u16 = nvs_get_u16,
    .nvs_open = nvs_open,
    .nvs_close = nvs_close,
    .nvs_commit = nvs_commit,
    .nvs_set_blob = nvs_set_blob,
    .nvs_get_blob = nvs_get_blob,
    .nvs_erase_key = nvs_erase_key,

    .magic = ESP_WIFI_OS_ADAPTER_MAGIC,
};
