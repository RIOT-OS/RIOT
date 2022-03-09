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

#include <errno.h>

#include "stdio_uart.h"

#include "board.h"
#include "periph/uart.h"
#include "isrpipe.h"

#if MODULE_VFS
#include "vfs.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_STDIO_UART_RX
static uint8_t _rx_buf_mem[STDIO_UART_RX_BUFSIZE];
isrpipe_t stdio_uart_isrpipe = ISRPIPE_INIT(_rx_buf_mem);
#endif

void stdio_init(void)
{
    uart_rx_cb_t cb;
    void *arg;

#ifdef MODULE_STDIO_UART_RX
    cb = (uart_rx_cb_t) isrpipe_write_one;
    arg = &stdio_uart_isrpipe;
#else
    cb = NULL;
    arg = NULL;
#endif

    uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, cb, arg);

#if MODULE_VFS
    vfs_bind_stdio();
#endif
}

#if IS_USED(MODULE_STDIO_AVAILABLE)
int stdio_available(void)
{
    return tsrb_avail(&stdio_uart_isrpipe.tsrb);
}
#endif

ssize_t stdio_read(void* buffer, size_t count)
{
#ifdef MODULE_STDIO_UART_RX
    return (ssize_t)isrpipe_read(&stdio_uart_isrpipe, buffer, count);
#else
    (void)buffer;
    (void)count;
    return -ENOTSUP;
#endif
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    uart_write(STDIO_UART_DEV, (const uint8_t *)buffer, len);
    return len;
}
