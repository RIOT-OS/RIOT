/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       STDIO over UART implementation
 *
 * This file implements a UART callback and the STDIO read/write functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#if MODULE_VFS
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif
#include "stdio_uart.h"

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

static char _rx_buf_mem[STDIO_UART_RX_BUFSIZE];
isrpipe_t stdio_uart_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

#if MODULE_VFS
static ssize_t stdio_uart_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);
static ssize_t stdio_uart_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes);

/**
 * @brief VFS file operation table for stdin/stdout/stderr
 */
static vfs_file_ops_t stdio_uart_vfs_ops = {
    .read = stdio_uart_vfs_read,
    .write = stdio_uart_vfs_write,
};

static ssize_t stdio_uart_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    int fd = filp->private_data.value;
    if (fd != STDIN_FILENO) {
        return -EBADF;
    }
    return stdio_read(dest, nbytes);
}

static ssize_t stdio_uart_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    int fd = filp->private_data.value;
    if (fd == STDIN_FILENO) {
        return -EBADF;
    }
    return stdio_write(src, nbytes);
}
#endif

void stdio_init(void)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &stdio_uart_isrpipe);
#else
    uart_init(ETHOS_UART, ETHOS_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &stdio_uart_isrpipe);
#endif
#if MODULE_VFS
    int fd;
    fd = vfs_bind(STDIN_FILENO, O_RDONLY, &stdio_uart_vfs_ops, (void *)STDIN_FILENO);
    if (fd < 0) {
        /* How to handle errors on init? */
    }
    fd = vfs_bind(STDOUT_FILENO, O_WRONLY, &stdio_uart_vfs_ops, (void *)STDOUT_FILENO);
    if (fd < 0) {
        /* How to handle errors on init? */
    }
    fd = vfs_bind(STDERR_FILENO, O_WRONLY, &stdio_uart_vfs_ops, (void *)STDERR_FILENO);
    if (fd < 0) {
        /* How to handle errors on init? */
    }
#endif
}

ssize_t stdio_read(void* buffer, size_t count)
{
    return (ssize_t)isrpipe_read(&stdio_uart_isrpipe, (char *)buffer, count);
}

ssize_t stdio_write(const void* buffer, size_t len)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_write(STDIO_UART_DEV, (const uint8_t *)buffer, len);
#else
    ethos_send_frame(&ethos, (const uint8_t *)buffer, len, ETHOS_FRAME_TYPE_TEXT);
#endif
    return len;
}
