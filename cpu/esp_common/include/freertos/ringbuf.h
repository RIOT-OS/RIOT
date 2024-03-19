/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 *
 * At the moment this header file is empty and exists only to avoid
 * compilation errors for files of the ESP-IDF that include `ringbuf.h`,
 * such as `uart.h` even if they do not need definitions from `ringbuf.h`.
 */

#ifndef FREERTOS_RINGBUF_H
#define FREERTOS_RINGBUF_H

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
#endif /* FREERTOS_RINGBUF_H */
