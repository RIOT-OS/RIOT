/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys
 * @{
 *
 * @file
 * @brief UART implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board_uart0.h"
#include "pipe.h"

static char uart0_buffer[UART0_BUFSIZE];
static ringbuffer_t uart0_ringbuffer = RINGBUFFER_INIT(uart0_buffer);
static pipe_t uart0_pipe = PIPE_INIT(uart0_ringbuffer);

size_t uart0_handle_incoming(const char *buf, size_t n)
{
    return pipe_write(&uart0_pipe, buf, n);
}

ssize_t uart0_read(char *buf, size_t n)
{
    return pipe_read(&uart0_pipe, buf, n);
}

int uart0_readc(void)
{
    char c;
    int result = pipe_read(&uart0_pipe, &c, 1);
    if (result != 1) {
        return -1;
    }
    return (unsigned char) c;
}

void uart0_putc(int c)
{
    putchar(c);
}

#ifdef MODULE_UART0_GETCHAR
int getchar(void) __attribute__((alias("uart0_readc")));
#endif
