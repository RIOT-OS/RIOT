/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
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
 * @brief UART stdio implementation
 *
 * This file implements a UART callback and read/write functions.
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#if MODULE_VFS
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif
#include "uart_stdio.h"

#include "board.h"
#include "periph/uart.h"
#include "isrpipe.h"

#ifdef USE_ETHOS_FOR_STDIO
#include "ethos.h"
extern ethos_t ethos;
#endif

#if MODULE_VFS
#include "vfs.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static char _rx_buf_mem[UART_STDIO_RX_BUFSIZE];
isrpipe_t uart_stdio_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

#if MODULE_VFS
static ssize_t uart_stdio_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);
static ssize_t uart_stdio_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes);

/**
 * @brief VFS file operation table for stdin/stdout/stderr
 */
static vfs_file_ops_t uart_stdio_vfs_ops = {
    .read = uart_stdio_vfs_read,
    .write = uart_stdio_vfs_write,
};

static ssize_t uart_stdio_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    int fd = filp->private_data.value;
    if (fd != STDIN_FILENO) {
        return -EBADF;
    }
    return uart_stdio_read(dest, nbytes);
}

static ssize_t uart_stdio_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    int fd = filp->private_data.value;
    if (fd == STDIN_FILENO) {
        return -EBADF;
    }
    return uart_stdio_write(src, nbytes);
}
#endif

void uart_stdio_init(void)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_init(UART_STDIO_DEV, UART_STDIO_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_stdio_isrpipe);
#else
    uart_init(ETHOS_UART, ETHOS_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_stdio_isrpipe);
#endif
#if MODULE_VFS
    int fd;
    fd = vfs_bind(STDIN_FILENO, O_RDONLY, &uart_stdio_vfs_ops, (void *)STDIN_FILENO);
    if (fd < 0) {
        /* How to handle errors on init? */
    }
    fd = vfs_bind(STDOUT_FILENO, O_WRONLY, &uart_stdio_vfs_ops, (void *)STDOUT_FILENO);
    if (fd < 0) {
        /* How to handle errors on init? */
    }
    fd = vfs_bind(STDERR_FILENO, O_WRONLY, &uart_stdio_vfs_ops, (void *)STDERR_FILENO);
    if (fd < 0) {
        /* How to handle errors on init? */
    }
#endif
}

int uart_stdio_read(char* buffer, int count)
{
    return isrpipe_read(&uart_stdio_isrpipe, buffer, count);
}

int uart_stdio_write(const char* buffer, int len)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_write(UART_STDIO_DEV, (uint8_t *)buffer, (size_t)len);
#else
    ethos_send_frame(&ethos, (uint8_t*)buffer, len, ETHOS_FRAME_TYPE_TEXT);
#endif
    return len;
}
