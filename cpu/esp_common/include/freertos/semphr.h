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
#include "freertos/task.h"

#include <stdlib.h>
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* SemaphoreHandle_t;

SemaphoreHandle_t xSemaphoreCreateMutex(void);
SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void);

void vSemaphoreDelete (SemaphoreHandle_t xSemaphore);

BaseType_t xSemaphoreGive (SemaphoreHandle_t xSemaphore);
BaseType_t xSemaphoreTake (SemaphoreHandle_t xSemaphore,
                           TickType_t xTicksToWait);
BaseType_t xSemaphoreGiveRecursive (SemaphoreHandle_t xSemaphore);
BaseType_t xSemaphoreTakeRecursive (SemaphoreHandle_t xSemaphore,
                                    TickType_t xTicksToWait);

TaskHandle_t xSemaphoreGetMutexHolder(SemaphoreHandle_t xMutex);

#define vPortCPUInitializeMutex(m)  mutex_init(m)

void vPortCPUAcquireMutex (portMUX_TYPE *mux);
void vPortCPUReleaseMutex (portMUX_TYPE *mux);

/*
 * PLEASE NOTE: Following definitions were copied directly from the FreeRTOS
 * distribution and are under the following copyright:
 *
 * FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
 * All rights reserved
 *
 * FreeRTOS is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by the
 * Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.
 *
 * Full license text is available on the following
 * link: http://www.freertos.org/a00114.html
 */

#define semSEMAPHORE_QUEUE_ITEM_LENGTH        ( ( uint8_t ) 0U )

#define xSemaphoreCreateBinary() \
        xQueueGenericCreate( ( UBaseType_t ) 1, \
                             semSEMAPHORE_QUEUE_ITEM_LENGTH, \
                             queueQUEUE_TYPE_BINARY_SEMAPHORE )
#define xSemaphoreCreateCounting( uxMaxCount, uxInitialCount ) \
        xQueueCreateCountingSemaphore( ( uxMaxCount ), ( uxInitialCount ) )

#define xSemaphoreTakeFromISR( xSemaphore, pxHigherPriorityTaskWoken ) \
        xQueueReceiveFromISR( ( QueueHandle_t ) ( xSemaphore ), \
                              NULL, ( pxHigherPriorityTaskWoken ) )

#define xSemaphoreGiveFromISR( xSemaphore, pxHigherPriorityTaskWoken ) \
        xQueueGiveFromISR( ( QueueHandle_t ) ( xSemaphore ), \
                           ( pxHigherPriorityTaskWoken ) )

#define xSemaphoreCreateCountingStatic( uxMaxCount, uxInitialCount, pxSemaphoreBuffer ) \
        xQueueCreateCountingSemaphoreStatic( ( uxMaxCount ), ( uxInitialCount ), \
                                             ( pxSemaphoreBuffer ) )

#define uxSemaphoreGetCount( xSemaphore ) \
        uxQueueMessagesWaiting( ( QueueHandle_t ) ( xSemaphore ) )

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
