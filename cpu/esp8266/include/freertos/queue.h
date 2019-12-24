/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef FREERTOS_QUEUE_H
#define FREERTOS_QUEUE_H

#ifndef DOXYGEN

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define xQueueHandle     QueueHandle_t

typedef void* QueueHandle_t;

QueueHandle_t xQueueGenericCreate (const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const uint8_t ucQueueType);

QueueHandle_t xQueueCreateCountingSemaphore (const UBaseType_t uxMaxCount,
                                             const UBaseType_t uxInitialCount);

void vQueueDelete (QueueHandle_t xQueue);

BaseType_t xQueueGenericReset (QueueHandle_t xQueue, BaseType_t xNewQueue);

BaseType_t xQueueGenericReceive (QueueHandle_t xQueue,
                                 void * const pvBuffer,
                                 TickType_t xTicksToWait,
                                 const BaseType_t xJustPeeking);

BaseType_t xQueueGenericSend (QueueHandle_t xQueue,
                              const void * const pvItemToQueue,
                              TickType_t xTicksToWait,
                              const BaseType_t xCopyPosition);

BaseType_t xQueueReceiveFromISR (QueueHandle_t xQueue, void * const pvBuffer,
                                 BaseType_t * const pxHigherPriorityTaskWoken);

BaseType_t xQueueGenericSendFromISR (QueueHandle_t xQueue,
                                     const void * const pvItemToQueue,
                                     BaseType_t * const pxHigherPriorityTaskWoken,
                                     const BaseType_t xCopyPosition );

BaseType_t xQueueGiveFromISR (QueueHandle_t xQueue,
                              BaseType_t * const pxHigherPriorityTaskWoken);

UBaseType_t uxQueueMessagesWaiting( QueueHandle_t xQueue );

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

#define queueSEND_TO_BACK         ( ( BaseType_t ) 0 )
#define queueSEND_TO_FRONT        ( ( BaseType_t ) 1 )
#define queueOVERWRITE            ( ( BaseType_t ) 2 )

#define queueQUEUE_TYPE_BASE                ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_SET                 ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_MUTEX               ( ( uint8_t ) 1U )
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE  ( ( uint8_t ) 2U )
#define queueQUEUE_TYPE_BINARY_SEMAPHORE    ( ( uint8_t ) 3U )
#define queueQUEUE_TYPE_RECURSIVE_MUTEX     ( ( uint8_t ) 4U )

#define errQUEUE_EMPTY      ( ( BaseType_t ) 0 )
#define errQUEUE_FULL       ( ( BaseType_t ) 0 )
#define errQUEUE_BLOCKED    ( -4 )
#define errQUEUE_YIELD      ( -5 )

#define xQueueCreate( uxQueueLength, uxItemSize ) \
        xQueueGenericCreate( ( uxQueueLength ), ( uxItemSize ), ( queueQUEUE_TYPE_BASE ) )

#define xQueueReceive( xQueue, pvBuffer, xTicksToWait ) \
        xQueueGenericReceive( ( xQueue ), ( pvBuffer ), ( xTicksToWait ), \
                              pdFALSE )

#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait ) \
        xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), \
                           queueSEND_TO_BACK )

#define xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait ) \
        xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), \
                           queueSEND_TO_BACK )

#define xQueueSendFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) \
        xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), \
                                  ( pxHigherPriorityTaskWoken ), \
                                  queueSEND_TO_BACK )

#define xQueueReset( xQueue ) xQueueGenericReset( xQueue, pdFALSE )

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_QUEUE_H */
