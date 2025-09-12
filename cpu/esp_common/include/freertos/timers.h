/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#pragma once

#ifndef DOXYGEN

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(void*);
#define tmrTIMER_CALLBACK        TimerCallbackFunction_t

TimerHandle_t xTimerCreate (const char * const pcTimerName,
                            const TickType_t xTimerPeriod,
                            const UBaseType_t uxAutoReload,
                            void * const pvTimerID,
                            TimerCallbackFunction_t pxCallbackFunction);
BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xBlockTime);
BaseType_t xTimerStart (TimerHandle_t xTimer, TickType_t xBlockTime);
BaseType_t xTimerStop  (TimerHandle_t xTimer, TickType_t xBlockTime);
BaseType_t xTimerReset (TimerHandle_t xTimer, TickType_t xTicksToWait);

void *pvTimerGetTimerID(const TimerHandle_t xTimer);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
