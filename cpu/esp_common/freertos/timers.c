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
#include "xtimer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

typedef struct {
    xtimer_t    xtimer;         /* xtimer object */
    const char* name;           /* FreeRTOS timer name */
    uint32_t    period;         /* in us */
    bool        autoreload;     /* FreeRTOS timer reload indicator */
    const void* timerid;        /* FreeRTOS timer id */
    TimerCallbackFunction_t cb; /* FreeRTOS callback function */
} freertos_xtimer_t;

static void IRAM_ATTR _xtimer_callback (void *arg)
{
    assert(arg != NULL);

    freertos_xtimer_t* timer = (freertos_xtimer_t*)arg;

    if (timer->autoreload) {
        xtimer_set(&timer->xtimer, timer->period);
    }

    if (timer->cb) {
        timer->cb(arg);
    }
}

TimerHandle_t xTimerCreate (const char * const pcTimerName,
                            const TickType_t xTimerPeriod,
                            const UBaseType_t uxAutoReload,
                            void * const pvTimerID,
                            TimerCallbackFunction_t pxCallbackFunction)
{
    freertos_xtimer_t* timer = malloc(sizeof(freertos_xtimer_t));
    if (timer == NULL) {
        return NULL;
    }

    /* FreeRTOS timer parameter */
    timer->name = pcTimerName;
    timer->period = xTimerPeriod * portTICK_PERIOD_MS * USEC_PER_MSEC;
    timer->autoreload = uxAutoReload;
    timer->timerid = pvTimerID;
    timer->cb = pxCallbackFunction;

    /* xtimer parameter */
    timer->xtimer.callback = _xtimer_callback;
    timer->xtimer.arg = timer;

    DEBUG("%s %p %s %d %u\n", __func__, timer, pcTimerName, xTimerPeriod, uxAutoReload);
    return timer;
}

BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %d\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_xtimer_t* timer = (freertos_xtimer_t*)xTimer;
    xtimer_remove(&timer->xtimer);
    free(timer);

    return pdTRUE;
}

BaseType_t xTimerStart (TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %d\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_xtimer_t* timer = (freertos_xtimer_t*)xTimer;
    xtimer_set(&timer->xtimer, timer->period);

    return pdTRUE;
}

BaseType_t xTimerStop  (TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %d\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_xtimer_t* timer = (freertos_xtimer_t*)xTimer;
    xtimer_remove(&timer->xtimer);

    return pdTRUE;
}

BaseType_t xTimerReset (TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %d\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_xtimer_t* timer = (freertos_xtimer_t*)xTimer;
    xtimer_set(&timer->xtimer, xBlockTime * portTICK_PERIOD_MS * USEC_PER_MSEC);

    return pdTRUE;
}

void *pvTimerGetTimerID(const TimerHandle_t xTimer)
{
    assert(xTimer != NULL);

    freertos_xtimer_t* timer = (freertos_xtimer_t*)xTimer;
    return (void*)timer->timerid;
}

#endif /* DOXYGEN */
