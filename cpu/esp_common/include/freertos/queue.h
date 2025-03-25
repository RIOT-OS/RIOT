/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#pragma once

#ifndef DOXYGEN

#include "freertos/FreeRTOS.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define xQueueHandle     QueueHandle_t

typedef void* QueueHandle_t;

/*
 * In FreeRTOS different types of semaphores, mutexes and queues are all
 * mapped to a single generic queue type. With all these different types,
 * single functions for send, receive, give and take are then used. To be
 * able to distinguish between these different types in RIOT, we need typed
 * objects.
 */
typedef struct {
    uint8_t     type;        /* type of the queue, MUST be the first element */
    bool        stat;        /* statically allocated queue memory */
    list_node_t sending;     /* threads that are waiting to send */
    list_node_t receiving;   /* threads that are waiting to receive */
    uint8_t*    queue;       /* the queue of waiting items */
    uint32_t    item_size;   /* size of each item in the queue */
    uint32_t    item_num;    /* num of items that can be stored in queue */
    uint32_t    item_front;  /* first item in queue */
    uint32_t    item_tail;   /* last item in queue */
    uint32_t    item_level;  /* num of items stored in queue */
} _queue_t;

typedef _queue_t StaticQueue_t;

QueueHandle_t xQueueGenericCreate (const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const uint8_t ucQueueType);

QueueHandle_t xQueueCreateStatic( const UBaseType_t uxQueueLength,
                                  const UBaseType_t uxItemSize,
                                  uint8_t *pucQueueStorageBuffer,
                                  StaticQueue_t *pxQueueBuffer );

BaseType_t xQueueGetStaticBuffers( QueueHandle_t xQueue,
                                   uint8_t ** ppucQueueStorage,
                                   StaticQueue_t ** ppxStaticQueue );

QueueHandle_t xQueueCreateCountingSemaphore (const UBaseType_t uxMaxCount,
                                             const UBaseType_t uxInitialCount);

QueueHandle_t xQueueCreateWithCaps(const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const UBaseType_t uxMemoryCaps);

void vQueueDelete (QueueHandle_t xQueue);

#define vQueueDeleteWithCaps vQueueDelete

BaseType_t xQueueReset (QueueHandle_t xQueue);

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

#define xQueueSendToBackFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) \
        xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), \
                                  ( pxHigherPriorityTaskWoken ), \
                                  queueSEND_TO_BACK )

#define xQueueOverwriteFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) \
        xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), \
                                  ( pxHigherPriorityTaskWoken ), \
                                  queueOVERWRITE )

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
