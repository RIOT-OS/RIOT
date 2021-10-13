/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       Implementation of internal/rcvbuf.h
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */
#include <errno.h>
#include <mutex.h>
#include <stdint.h>
#include "net/gnrc/tcp/config.h"
#include "include/gnrc_tcp_common.h"
#include "include/gnrc_tcp_rcvbuf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Receive buffer entry.
 */
typedef struct {
    uint8_t used;                          /**< Flag: Is buffer in use? */
    uint8_t buffer[GNRC_TCP_RCV_BUF_SIZE]; /**< Receive buffer storage */
} _rcvbuf_entry_t;

/**
 * @brief Struct holding receive buffers.
 */
typedef struct {
    mutex_t lock;                                         /**< Access lock */
    _rcvbuf_entry_t entries[CONFIG_GNRC_TCP_RCV_BUFFERS]; /**< Buffers */
} _rcvbuf_t;

/**
 * @brief Internal struct holding receive buffers.
 */
static _rcvbuf_t _static_buf;

/**
 * @brief Allocate receive buffer.
 *
 * @returns   Not NULL if a receive buffer was allocated.
 *            NULL if allocation failed.
 */
static void* _rcvbuf_alloc(void)
{
    TCP_DEBUG_ENTER;
    void *result = NULL;
    mutex_lock(&(_static_buf.lock));
    for (size_t i = 0; i < CONFIG_GNRC_TCP_RCV_BUFFERS; ++i) {
        if (_static_buf.entries[i].used == 0) {
            _static_buf.entries[i].used = 1;
            result = (void *)(_static_buf.entries[i].buffer);
            break;
        }
    }
    mutex_unlock(&(_static_buf.lock));
    TCP_DEBUG_LEAVE;
    return result;
}

/**
 * @brief Release allocated receive buffer.
 *
 * @param[in] buf   Pointer to buffer that should be released.
 */
static void _rcvbuf_free(void * const buf)
{
    TCP_DEBUG_ENTER;
    mutex_lock(&(_static_buf.lock));
    for (size_t i = 0; i < CONFIG_GNRC_TCP_RCV_BUFFERS; ++i) {
        if ((_static_buf.entries[i].used == 1) && (buf == _static_buf.entries[i].buffer)) {
            _static_buf.entries[i].used = 0;
        }
    }
    mutex_unlock(&(_static_buf.lock));
    TCP_DEBUG_LEAVE;
}

void _gnrc_tcp_rcvbuf_init(void)
{
    TCP_DEBUG_ENTER;
    mutex_init(&(_static_buf.lock));
    for (size_t i = 0; i < CONFIG_GNRC_TCP_RCV_BUFFERS; ++i) {
        _static_buf.entries[i].used = 0;
    }
    TCP_DEBUG_LEAVE;
}

int _gnrc_tcp_rcvbuf_get_buffer(gnrc_tcp_tcb_t *tcb)
{
    TCP_DEBUG_ENTER;
    if (tcb->rcv_buf_raw == NULL) {
        tcb->rcv_buf_raw = _rcvbuf_alloc();
        if (tcb->rcv_buf_raw == NULL) {
            TCP_DEBUG_ERROR("-ENOMEM: Failed to allocate receive buffer.");
            TCP_DEBUG_LEAVE;
            return -ENOMEM;
        }
        else {
            ringbuffer_init(&tcb->rcv_buf, (char *) tcb->rcv_buf_raw, GNRC_TCP_RCV_BUF_SIZE);
        }
    }
    TCP_DEBUG_LEAVE;
    return 0;
}

void _gnrc_tcp_rcvbuf_release_buffer(gnrc_tcp_tcb_t *tcb)
{
    TCP_DEBUG_ENTER;
    if (tcb->rcv_buf_raw != NULL) {
        _rcvbuf_free(tcb->rcv_buf_raw);
        tcb->rcv_buf_raw = NULL;
    }
    TCP_DEBUG_LEAVE;
}
