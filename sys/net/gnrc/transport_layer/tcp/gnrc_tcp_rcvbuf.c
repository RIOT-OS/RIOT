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
#include "internal/rcvbuf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

rcvbuf_t _static_buf;   /**< Staticly allocated receive buffers */

void _rcvbuf_init(void)
{
    DEBUG("gnrc_tcp_rcvbuf.c : _rcvbuf_init() : Entry\n");
    mutex_init(&(_static_buf.lock));
    for (size_t i = 0; i < GNRC_TCP_RCV_BUFFERS; ++i) {
        _static_buf.entries[i].used = 0;
    }
}

static void* _rcvbuf_alloc(void)
{
    void *result = NULL;
    DEBUG("gnrc_tcp_rcvbuf.c : _rcvbuf_alloc() : Entry\n");
    mutex_lock(&(_static_buf.lock));
    for (size_t i = 0; i < GNRC_TCP_RCV_BUFFERS; ++i) {
        if (_static_buf.entries[i].used == 0) {
            _static_buf.entries[i].used = 1;
            result = (void *)(_static_buf.entries[i].buffer);
            break;
        }
    }
    mutex_unlock(&(_static_buf.lock));
    return result;
}

static void _rcvbuf_free(void * const buf)
{
    DEBUG("gnrc_tcp_rcvbuf.c : _rcvbuf_free() : Entry\n");
    mutex_lock(&(_static_buf.lock));
    for (size_t i = 0; i < GNRC_TCP_RCV_BUFFERS; ++i) {
        if (_static_buf.entries[i].used == 1 && buf == _static_buf.entries[i].buffer) {
            _static_buf.entries[i].used = 0;
        }
    }
    mutex_unlock(&(_static_buf.lock));
}

int _rcvbuf_get_buffer(gnrc_tcp_tcb_t *tcb)
{
    if (tcb->rcv_buf_raw == NULL) {
        tcb->rcv_buf_raw = _rcvbuf_alloc();
        if (tcb->rcv_buf_raw == NULL) {
            DEBUG("gnrc_tcp_rcvbuf.c : _rcvbuf_get_buffer() : Can't allocate rcv_buf_raw\n");
            return -ENOMEM;
        }
        else {
            ringbuffer_init(&tcb->rcv_buf, (char *) tcb->rcv_buf_raw, GNRC_TCP_RCV_BUF_SIZE);
        }
    }
    return 0;
}

void _rcvbuf_release_buffer(gnrc_tcp_tcb_t *tcb)
{
    if (tcb->rcv_buf_raw != NULL) {
        _rcvbuf_free(tcb->rcv_buf_raw);
        tcb->rcv_buf_raw = NULL;
    }
}
