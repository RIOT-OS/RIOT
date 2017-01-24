/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 *
 * @}
 */

#include <stdio.h>

#include "uart_stdio.h"

#include "board.h"
#include "periph/uart.h"
#include "isrpipe.h"

#ifdef USE_ETHOS_FOR_STDIO
#include "ethos.h"
extern ethos_t ethos;
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static char _rx_buf_mem[UART_STDIO_RX_BUFSIZE];
isrpipe_t uart_stdio_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

void uart_stdio_init(void)
{
#ifndef USE_ETHOS_FOR_STDIO
    uart_init(UART_STDIO_DEV, UART_STDIO_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_stdio_isrpipe);
#else
    uart_init(ETHOS_UART, ETHOS_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_stdio_isrpipe);
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
