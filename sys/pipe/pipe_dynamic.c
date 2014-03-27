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
 * @ingroup     sys
 * @{
 * @file
 * @brief       Implementation for dynamically allocated pipes.
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <malloc.h>

#include "pipe.h"

struct mallocd_pipe
{
    pipe_t pipe;
    ringbuffer_t rb;
    char buffer[1];
};

pipe_t *pipe_malloc(unsigned size)
{
    struct mallocd_pipe *m_pipe = malloc(sizeof (*m_pipe) + size);
    if (m_pipe) {
        ringbuffer_init(&m_pipe->rb, m_pipe->buffer, size);
        pipe_init(&m_pipe->pipe, &m_pipe->rb, free);
    }
    return &m_pipe->pipe;
}

void pipe_free(pipe_t *rp)
{
    if (rp && rp->free) {
        rp->free(rp);
    }
}
