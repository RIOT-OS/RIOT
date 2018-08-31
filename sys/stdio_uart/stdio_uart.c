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

void stdio_init(void)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &stdio_uart_isrpipe);
#else
    uart_init(ETHOS_UART, ETHOS_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &stdio_uart_isrpipe);
#endif
#if MODULE_VFS
    vfs_bind_stdio();
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
