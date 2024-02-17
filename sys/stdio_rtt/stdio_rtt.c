/*
 * Copyright (C) 2016 Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 * This file was based on SEGGER's reference implementation
 * which can be found here: https://www.segger.com/jlink-rtt.html
 * (c) 2014-2016 SEGGER Microcontroller GmbH & Co. KG
 * This implementation bore the following license notes:
 **********************************************************************
 *               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
 *       Solutions for real time microcontroller applications         *
 **********************************************************************
 *                                                                    *
 *       (c) 2014 - 2016  SEGGER Microcontroller GmbH & Co. KG        *
 *                                                                    *
 *       www.segger.com     Support: support@segger.com               *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 *       SEGGER RTT * Real Time Transfer for embedded targets         *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 * All rights reserved.                                               *
 *                                                                    *
 * SEGGER strongly recommends to not make any changes                 *
 * to or modify the source code of this software in order to stay     *
 * compatible with the RTT protocol and J-Link.                       *
 *                                                                    *
 * Redistribution and use in source and binary forms, with or         *
 * without modification, are permitted provided that the following    *
 * conditions are met:                                                *
 *                                                                    *
 * o Redistributions of source code must retain the above copyright   *
 *   notice, this list of conditions and the following disclaimer.    *
 *                                                                    *
 * o Redistributions in binary form must reproduce the above          *
 *   copyright notice, this list of conditions and the following      *
 *   disclaimer in the documentation and/or other materials provided  *
 *   with the distribution.                                           *
 *                                                                    *
 * o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
 *   nor the names of its contributors may be used to endorse or      *
 *   promote products derived from this software without specific     *
 *   prior written permission.                                        *
 *                                                                    *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                            *
 *                                                                    *
 **********************************************************************/

/**
 * @ingroup sys
 * @{
 *
 * @file
 * @brief SEGGER RTT stdio implementation
 *
 * This file implements RIOTs STDIO interface and works with a ringbuffer that
 * complies with SEGGER RTT.
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 * @}
 */

#include <string.h>

#include "macros/utils.h"
#include "mutex.h"
#include "stdio_rtt.h"
#include "thread.h"
#include "ztimer/periodic.h"

/* This parameter affects the bandwidth of both input and output. Decreasing
   it will significantly improve bandwidth at the cost of CPU time. */
#ifndef STDIO_POLL_INTERVAL_MS
#define STDIO_POLL_INTERVAL_MS 50U
#endif

#ifndef STDIO_TX_BUFSIZE
#define STDIO_TX_BUFSIZE    (512)
#endif

#ifndef STDIO_RX_BUFSIZE
#define STDIO_RX_BUFSIZE    (32)
#endif

#if !defined(MODULE_STDIN) && !defined(STDIO_RTT_DISABLE_STDIN)
#define STDIO_RTT_DISABLE_STDIN 1
#endif

/**
 * @brief buffer holding stdout
 */
static char up_buffer   [STDIO_TX_BUFSIZE];

/**
 * @brief buffer holding stdin
 */
static char down_buffer [STDIO_RX_BUFSIZE];

/**
 * @brief flag that enables stdout blocking/polling
 */
static char blocking_stdout = IS_USED(STDIO_RTT_ENABLE_BLOCKING_STDOUT);

static ztimer_periodic_t stdin_timer;

/**
 * @brief SEGGER's ring buffer implementation
 */
typedef struct {
    const char* channel_name; /* Optional name. Standard names so far are:
                                 "Terminal", "VCom" */
    char*  buf_ptr;           /* Pointer to start of buffer */
    int32_t buf_size;         /* Buffer size in bytes. Note that one byte is
                                 lost, as this implementation does not fill up
                                 the buffer in order to avoid the problem of
                                 being unable to distinguish between full and
                                 empty. */
    volatile int32_t wr_off;  /* Position of next item to be written by either
                                 host (down-buffer) or target (up-buffer). Must
                                 be volatile since it may be modified by host
                                 (down-buffer) */
    volatile int32_t rd_off;  /* Position of next item to be read by target
                                 (down-buffer) or host (up-buffer). Must be
                                 volatile since it may be modified by host
                                 (up-buffer) */
    int32_t    flags;         /* Contains configuration flags */
} rtt_ringbuf_t;

/**
 * @brief RTT control block which describes the number of buffers available
 *        as well as the configuration for each buffer. The struct definition
 *        is fixed, as it is expected by SEGGER's software
 */
typedef struct {
    char        sentinel[16];      /* Initialized to "SEGGER RTT" */
    int32_t     max_up_buffers;    /* Initialized to 1 */
    int32_t     max_down_buffers;  /* Initialized to 1 */
    rtt_ringbuf_t up[1];           /* Up buffers, transferring information up
                                      from target via debug probe to host */
    rtt_ringbuf_t down[1];         /* Down buffers, transferring information
                                      down from host via debug probe to target */
} segger_rtt_cb_t;

/**
 * @brief The SEGGER Real-Time-Terminal control block (CB)
 */
static segger_rtt_cb_t rtt_cb = {
    "SEGGER RTT",
    1,
    1,
    {{ "Terminal", &up_buffer[0],   sizeof(up_buffer),   0, 0, 0 }},
    {{ "Terminal", &down_buffer[0], sizeof(down_buffer), 0, 0, 0 }},
};

static int rtt_read_bytes_avail(void)
{
    int16_t rd_off;
    int16_t wr_off;

    rd_off = rtt_cb.down[0].rd_off;
    wr_off = rtt_cb.down[0].wr_off;

    /* Read from current read position to wrap-around of buffer, first */
    if (rd_off > wr_off) {
        return rtt_cb.down[0].buf_size - rd_off;
    } else {
        return wr_off - rd_off;
    }
}

/**
 * @brief read bytes from the down buffer. This function does not block.
 *        The logic here is unmodified from SEGGER's reference, it is just
 *        refactored to match code style. The string is not null terminated.
 *
 * @return the number of bytes read
 */
static int rtt_read(uint8_t* buf_ptr, uint16_t buf_size) {
    int16_t num_bytes_rem;
    uint16_t num_bytes_read;
    int16_t rd_off;
    int16_t wr_off;

    rd_off = rtt_cb.down[0].rd_off;
    wr_off = rtt_cb.down[0].wr_off;
    num_bytes_read = 0;

    /* Read from current read position to wrap-around of buffer, first */
    if (rd_off > wr_off) {
        num_bytes_rem = rtt_cb.down[0].buf_size - rd_off;
        num_bytes_rem = MIN(num_bytes_rem, (int)buf_size);
        memcpy(buf_ptr, rtt_cb.down[0].buf_ptr + rd_off, num_bytes_rem);
        num_bytes_read += num_bytes_rem;
        buf_ptr  += num_bytes_rem;
        buf_size -= num_bytes_rem;
        rd_off   += num_bytes_rem;
        /* Handle wrap-around of buffer */
        if (rd_off == rtt_cb.down[0].buf_size) {
            rd_off = 0;
        }
    }

    /* Read remaining items of buffer */
    num_bytes_rem = wr_off - rd_off;
    num_bytes_rem = MIN(num_bytes_rem, (int)buf_size);
    if (num_bytes_rem > 0) {
        memcpy(buf_ptr, rtt_cb.down[0].buf_ptr + rd_off, num_bytes_rem);
        num_bytes_read += num_bytes_rem;
        rd_off   += num_bytes_rem;
    }
    if (num_bytes_read) {
        rtt_cb.down[0].rd_off = rd_off;
    }
    return num_bytes_read;
}

/**
 * @brief write bytes to the up buffer. This function does not block.
 *        The logic here is unmodified from SEGGER's reference, it is just
 *        refactored to match code style. The string does not need to be null
 *        terminated.
 *
 * @return the number of bytes read
 */
int rtt_write(const char* buf_ptr, unsigned num_bytes) {
    int num_bytes_to_write;
    unsigned num_bytes_written;
    int rd_off;

    rd_off = rtt_cb.up[0].rd_off;
    num_bytes_to_write =  rd_off - rtt_cb.up[0].wr_off - 1;
    if (num_bytes_to_write < 0) {
        num_bytes_to_write += rtt_cb.up[0].buf_size;
    }
    /* If the complete data does not fit in the buffer, trim the data */
    if ((int)num_bytes > num_bytes_to_write) {
        num_bytes = num_bytes_to_write;
    }

    /* Early out if there is nothing to do */
    if (num_bytes == 0) {
        return 0;
    }

    /* Write data to buffer and handle wrap-around if necessary */
    num_bytes_written = 0;
    do {
        /* May be changed by host (debug probe) in the meantime */
        rd_off = rtt_cb.up[0].rd_off;
        num_bytes_to_write = rd_off - rtt_cb.up[0].wr_off - 1;
        if (num_bytes_to_write < 0) {
            num_bytes_to_write += rtt_cb.up[0].buf_size;
        }
        /* Number of bytes that can be written until buffer wrap-around */
        num_bytes_to_write = MIN(num_bytes_to_write, (rtt_cb.up[0].buf_size -
          rtt_cb.up[0].wr_off));
        num_bytes_to_write = MIN(num_bytes_to_write, (int)num_bytes);
        memcpy(rtt_cb.up[0].buf_ptr + rtt_cb.up[0].wr_off, buf_ptr, num_bytes_to_write);
        num_bytes_written   += num_bytes_to_write;
        buf_ptr             += num_bytes_to_write;
        num_bytes           -= num_bytes_to_write;
        rtt_cb.up[0].wr_off += num_bytes_to_write;
        if (rtt_cb.up[0].wr_off == rtt_cb.up[0].buf_size) {
            rtt_cb.up[0].wr_off = 0;
        }
    } while (num_bytes);
    return num_bytes_written;
}

static bool _rtt_read_cb(void *arg)
{
    int bytes = rtt_read_bytes_avail();
    uint8_t buffer[STDIO_RX_BUFSIZE];

    if (bytes) {
        bytes = rtt_read(buffer, sizeof(buffer));
        isrpipe_write(arg, buffer, bytes);
    }

    return true;
}

static bool _init_done;
static void _init(void) {
    if (IS_USED(STDIO_RTT_DISABLE_STDIN)) {
        return;
    }
    if (!thread_getpid()) {
        /* we can't use ztimer in early init */
        return;
    }

    ztimer_periodic_init(ZTIMER_MSEC, &stdin_timer, _rtt_read_cb, &stdin_isrpipe,
                         STDIO_POLL_INTERVAL_MS);
    ztimer_periodic_start(&stdin_timer);
    _init_done = true;
}

static void _detach(void)
{
    if (!IS_USED(STDIO_RTT_DISABLE_STDIN)) {
        ztimer_periodic_stop(&stdin_timer);
    }
}

void rtt_stdio_enable_blocking_stdout(void) {
    blocking_stdout = 1;
}

static ssize_t _write(const void* in, size_t len) {
    const char *buffer = in;
    int written = rtt_write(buffer, len);

    /* we have to postpone ztimer init */
    if (!_init_done) {
        _init();
    }

    if (blocking_stdout) {
        uint32_t last_wakeup = ztimer_now(ZTIMER_MSEC);
        while ((size_t)written < len) {
            ztimer_periodic_wakeup(ZTIMER_MSEC, &last_wakeup, STDIO_POLL_INTERVAL_MS);
            written += rtt_write(&buffer[written], len-written);
        }
    }

    return (ssize_t)written;
}

STDIO_PROVIDER(STDIO_RTT, _init, _detach, _write)
