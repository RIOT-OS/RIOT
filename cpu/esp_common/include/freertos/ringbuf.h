/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#pragma once

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#define RINGBUF_TYPE_NOSPLIT    0
#define RINGBUF_TYPE_BYTEBUF    2

typedef unsigned RingbufferType_t;
typedef void * RingbufHandle_t;

RingbufHandle_t xRingbufferCreate(size_t xBufferSize, RingbufferType_t xBufferType);

void vRingbufferDelete(RingbufHandle_t xRingbuffer);

void *xRingbufferReceiveUpTo(RingbufHandle_t xRingbuffer,
                             size_t *pxItemSize,
                             TickType_t xTicksToWait,
                             size_t xMaxSize);

BaseType_t xRingbufferSendFromISR(RingbufHandle_t xRingbuffer,
                                  const void *pvItem,
                                  size_t xItemSize,
                                  BaseType_t *pxHigherPriorityTaskWoken);

void *xRingbufferReceiveUpToFromISR(RingbufHandle_t xRingbuffer,
                                    size_t *pxItemSize, size_t xMaxSize);

void *xRingbufferReceiveFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize);

void vRingbufferReturnItemFromISR(RingbufHandle_t xRingbuffer, void *pvItem,
                                  BaseType_t *pxHigherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
