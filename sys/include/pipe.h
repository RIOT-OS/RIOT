/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @defgroup    sys_pipe Pipe IPC
 * @ingroup     sys
 * @{
 * @file
 *
 * @brief       Generic pipe implementation.
 * @details     This pipe implementation is a tight wrapper around a ringbuffer.
 *              It sends the calling thread to sleep if the ringbuffer is full
 *              or empty, respectively. It can be used in ISRs, too.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef __PIPE__H
#define __PIPE__H

#include <sys/types.h>

#include "mutex.h"
#include "ringbuffer.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PIPE_BUF
#   define PIPE_BUF (128) /**< Size of a dynamically malloc'd pipe. */
#endif

/**
 * A generic pipe.
 */
typedef struct riot_pipe
{
    ringbuffer_t *rb;     /**< Wrapped ringbuffer. */
    tcb_t *read_blocked;  /**< A thread that wants to write to this full pipe. */
    tcb_t *write_blocked; /**< A thread that wants to read from this empty pipe. */
    void (*free)(void *); /**< Function to call by pipe_free(). Used like `pipe->free(pipe)`. */
} pipe_t;

/**
 * @brief        Initialize a pipe.
 * @param[out]   pipe   Datum to initialize.
 * @param        rb     Ringbuffer to use. Needs to be initialized!
 * @param        free   Function to call by pipe_free(). Used like `pipe->free(pipe)`.
 *                      Should be `NULL` for statically allocated pipes.
 */
void pipe_init(pipe_t *pipe, ringbuffer_t *rb, void (*free)(void *));

/**
 * @brief        Read from a pipe.
 * @details      Only one thread may access the pipe readingly at once.
 *               If the pipe is empty, then the current thread is send sleeping.
 *               It gets woken up once there is data ready in the pipe.
 *               In an ISR (inISR()) 0 will returned if the pipe is empty.
 * @param[in]    pipe   Pipe to read from.
 * @param[out]   buf    Buffer to write into
 * @param        n      Size of buffer.
 * @returns      `> 0` if data could be read.
 *               `== 0` if the pipe is empty and isISR().
 */
ssize_t pipe_read(pipe_t *pipe, void *buf, size_t n);

/**
 * @brief        Write to a pipe.
 * @details      Only one thread may access the pipe writingly at once.
 *               If the pipe is full, then the current thread is send sleeping.
 *               It gets woken up once there is room again in the pipe.
 *               In an ISR (inISR()) 0 will returned if the pipe is full.
 * @param[in]    pipe   Pipe to write to.
 * @param[out]   buf    Buffer to read from.
 * @param        n      Size of buffer.
 * @returns      `> 0` if data could be written.
 *               `== 0` if the pipe is full and isISR().
 */
ssize_t pipe_write(pipe_t *pipe, const void *buf, size_t n);

/**
 * @brief      Dynamically allocate a pipe with room for `size` bytes.
 * @details    This function uses `malloc()` and may break real-time behaviors.
 *             Try not to use this function.
 * @param      size   Size of the underlying ringbuffer to allocate.
 * @returns    Newly allocated pipe. NULL if the memory is exhausted.
 */
pipe_t *pipe_malloc(unsigned size);

/**
 * @brief     Free a pipe.
 * @details   On statically allocated pipes you do not have to call this function.
 *            Most likely you will only need this function in junction with
 *            pipe_malloc().
 * @param     rp   Pipe to free.
 */
void pipe_free(pipe_t *rp);

#ifdef __cplusplus
}
#endif

#endif
/**
 * @}
 */
