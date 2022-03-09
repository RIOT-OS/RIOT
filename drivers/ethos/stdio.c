/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
 *               2018-21 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include "ethos.h"
#include "isrpipe.h"
#include "stdio_uart.h"

extern ethos_t ethos;

static uint8_t _rx_buf_mem[STDIO_UART_RX_BUFSIZE];
isrpipe_t ethos_stdio_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

static void _isrpipe_write(void *arg, uint8_t data)
{
    isrpipe_write_one(arg, (char)data);
}

void stdio_init(void)
{
    uart_init(ETHOS_UART, ETHOS_BAUDRATE, _isrpipe_write, &ethos_stdio_isrpipe);

#if MODULE_VFS
    vfs_bind_stdio();
#endif
}

extern unsigned ethos_unstuff_readbyte(uint8_t *buf, uint8_t byte,
                                       bool *escaped, uint8_t *frametype);

ssize_t stdio_read(void* buffer, size_t len)
{
    uint8_t *ptr = buffer;
    bool escaped = false;
    uint8_t frametype = ETHOS_FRAME_TYPE_TEXT;
    uint8_t byte;

    do {
        int read = isrpipe_read(&ethos_stdio_isrpipe, &byte, 1);
        int tmp;

        if (read == 0) {
            continue;
        }
        else if (len == 0) {
            return -ENOBUFS;
        }
        tmp = ethos_unstuff_readbyte(ptr, byte, &escaped, &frametype);
        ptr += tmp;
        if ((unsigned)(ptr - (uint8_t *)buffer) > len) {
            while (byte != ETHOS_FRAME_DELIMITER) {
                /* clear out unreceived frame */
                isrpipe_read(&ethos_stdio_isrpipe, &byte, 1);
            }
            return -ENOBUFS;
        }
    } while (byte != ETHOS_FRAME_DELIMITER);

    return ptr - (uint8_t *)buffer;
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    ethos_send_frame(&ethos, (const uint8_t *)buffer, len, ETHOS_FRAME_TYPE_TEXT);
    return len;
}

/** @} */
