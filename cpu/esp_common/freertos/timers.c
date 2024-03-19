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
#include "ztimer.h"
#include "timex.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

typedef struct {
    ztimer_t    ztimer;         /* ztimer object */
    const char* name;           /* FreeRTOS timer name */
    uint32_t    period;         /* in us */
    bool        autoreload;     /* FreeRTOS timer reload indicator */
    const void* timerid;        /* FreeRTOS timer id */
    TimerCallbackFunction_t cb; /* FreeRTOS callback function */
} freertos_ztimer_t;

static void IRAM_ATTR _ztimer_callback (void *arg)
{
    assert(arg != NULL);

    freertos_ztimer_t* timer = (freertos_ztimer_t*)arg;

    if (timer->autoreload) {
        ztimer_set(ZTIMER_MSEC, &timer->ztimer, timer->period);
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
    freertos_ztimer_t* timer = malloc(sizeof(freertos_ztimer_t));
    if (timer == NULL) {
        return NULL;
    }

    /* FreeRTOS timer parameter */
    timer->name = pcTimerName;
    timer->period = xTimerPeriod * portTICK_PERIOD_MS;
    timer->autoreload = uxAutoReload;
    timer->timerid = pvTimerID;
    timer->cb = pxCallbackFunction;

    /* ztimer parameter */
    timer->ztimer.callback = _ztimer_callback;
    timer->ztimer.arg = timer;

    DEBUG("%s %p %s %"PRIu32" %u\n",
          __func__, timer, pcTimerName, xTimerPeriod, uxAutoReload);
    return timer;
}

BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %"PRIu32"\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_ztimer_t* timer = (freertos_ztimer_t*)xTimer;
    ztimer_remove(ZTIMER_MSEC, &timer->ztimer);
    free(timer);

    return pdTRUE;
}

BaseType_t xTimerStart (TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %"PRIu32"\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_ztimer_t* timer = (freertos_ztimer_t*)xTimer;
    ztimer_set(ZTIMER_MSEC, &timer->ztimer, timer->period);

    return pdTRUE;
}

BaseType_t xTimerStop  (TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %"PRIu32"\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_ztimer_t* timer = (freertos_ztimer_t*)xTimer;
    ztimer_remove(ZTIMER_MSEC, &timer->ztimer);

    return pdTRUE;
}

BaseType_t xTimerReset (TimerHandle_t xTimer, TickType_t xBlockTime)
{
    DEBUG("%s %p %"PRIu32"\n", __func__, xTimer, xBlockTime);
    assert(xTimer != NULL);

    freertos_ztimer_t* timer = (freertos_ztimer_t*)xTimer;
    ztimer_set(ZTIMER_MSEC, &timer->ztimer, xBlockTime * portTICK_PERIOD_MS * US_PER_MS);

    return pdTRUE;
}

void *pvTimerGetTimerID(const TimerHandle_t xTimer)
{
    assert(xTimer != NULL);

    freertos_ztimer_t* timer = (freertos_ztimer_t*)xTimer;
    return (void*)timer->timerid;
}

#endif /* DOXYGEN */
