/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Anthony Merlino  <anthony@vergeaero.com>
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
 * This file implements ETHOS stdio read/write functions.
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Anthony Merlino <anthony@vergeaero.com>
 *
 * @}
 */

#include <stdio.h>

#include "riot_stdio.h"

#include "board.h"
#include "isrpipe.h"

#include "ethos.h"
extern ethos_t ethos;

#define ENABLE_DEBUG 0
#include "debug.h"

static char _rx_buf_mem[ETHOS_RX_BUFSIZE];
isrpipe_t ethos_stdio_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

void stdio_init(void)
{
    uart_init(ETHOS_UART, ETHOS_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &ethos_stdio_isrpipe);
}

int stdio_read(char* buffer, int count)
{
    return isrpipe_read(&ethos_stdio_isrpipe, buffer, count);
}

int stdio_write(const char* buffer, int len)
{
    ethos_send_frame(&ethos, (uint8_t*)buffer, len, ETHOS_FRAME_TYPE_TEXT);
    return len;
}
