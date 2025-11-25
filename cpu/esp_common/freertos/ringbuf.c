/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#include <assert.h>
#include <stdlib.h>

#include "irq_arch.h"
#include "ringbuffer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct {
    ringbuffer_t rbuf;
    uint16_t item_size;
    char *buf;
} rbuf_handle_t;

RingbufHandle_t xRingbufferCreate(size_t xBufferSize, RingbufferType_t xBufferType)
{
    /* only byte and no split buffers are supported for now */
    assert((xBufferType == RINGBUF_TYPE_BYTEBUF) ||
           (xBufferType == RINGBUF_TYPE_NOSPLIT));

    /* allocate the space for rbuf_handle_t including the buffer */
    rbuf_handle_t *handle = malloc(xBufferSize + sizeof(uint16_t) + sizeof(ringbuffer_t));
    if (handle == NULL) {
        return NULL;
    }
    handle->item_size = 0;
    handle->buf = NULL;
    ringbuffer_init((ringbuffer_t *)handle, handle->buf, xBufferSize);

    return handle;
}

void vRingbufferDelete(RingbufHandle_t xRingbuffer)
{
    assert(xRingbuffer != NULL);
    free(xRingbuffer);
}

void *xRingbufferReceiveUpToFromISR(RingbufHandle_t xRingbuffer,
                                    size_t *pxItemSize, size_t xMaxSize)
{
    rbuf_handle_t *handle = xRingbuffer;
    size_t data_len = 0;

    assert(handle != NULL);

    /* determine the number of bytes to be read */
    if (handle->rbuf.avail) {
        data_len = ((xMaxSize == 0) || (handle->rbuf.avail < xMaxSize)) ? handle->rbuf.avail
                                                                        : xMaxSize;
    }
    /* ESP-IDF ring buffers require two read operation if the data wrap around */
    if (data_len > (handle->rbuf.size - handle->rbuf.start)) {
        data_len = handle->rbuf.size - handle->rbuf.start;
    }
    handle->item_size = data_len;
    *pxItemSize = data_len;

    return handle->rbuf.buf + handle->rbuf.start;
}

void *xRingbufferReceiveFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize)
{
    return xRingbufferReceiveUpToFromISR(xRingbuffer, pxItemSize, 0);
}

void vRingbufferReturnItemFromISR(RingbufHandle_t xRingbuffer, void *pvItem,
                                  BaseType_t *pxHigherPriorityTaskWoken)
{
    rbuf_handle_t *handle = xRingbuffer;
    if (handle->item_size) {
        ringbuffer_remove(&handle->rbuf, handle->item_size);
    }
}

BaseType_t xRingbufferSendFromISR(RingbufHandle_t xRingbuffer,
                                  const void *pvItem,
                                  size_t xItemSize,
                                  BaseType_t *pxHigherPriorityTaskWoken)
{
    rbuf_handle_t *handle = xRingbuffer;

    assert(handle != NULL);

    /* return immediately if there is not enough space in the ring buffer */
    if (ringbuffer_get_free(&handle->rbuf) < xItemSize) {
        return pdFALSE;
    }

    /* add data to the ringbuffer */
    return ringbuffer_add(&handle->rbuf, pvItem, xItemSize) == xItemSize;
}
