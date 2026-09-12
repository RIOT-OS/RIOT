/*
 * SPDX-FileCopyrightText: 2014 René Kijewski  <rene.kijewski@fu-berlin.de>
 * SPDX-License-Identifier: NONE
 */

/**
 * @ingroup     sys_pipe
 * @{
 * @file
 * @brief       Implementation for dynamically allocated pipes.
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <stdlib.h>

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
        return &m_pipe->pipe;
    }
    return NULL;
}

void pipe_free(pipe_t *rp)
{
    if (rp && rp->free) {
        rp->free(rp);
    }
}
