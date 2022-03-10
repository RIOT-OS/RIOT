/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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
 * @brief       STDIO over Telnet implementation
 *
 * This file implements STDIO via a Telnet server with fallback UART output
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>

#include "board.h"
#include "kernel_defines.h"
#include "net/telnet.h"
#if IS_USED(MODULE_PERIPH_UART)
#include "stdio_uart.h"
#include "periph/uart.h"
#endif
#if IS_USED(MODULE_VFS)
#include "vfs.h"
#endif
#ifdef CPU_NATIVE
#include "native_internal.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static inline void _init_fallback(void)
{
#if defined(STDIO_UART_DEV) && IS_USED(MODULE_PERIPH_UART)
    uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, NULL, NULL);
#endif
}

static inline int _write_fallback(const void* buffer, size_t len)
{
#if defined(CPU_NATIVE)
    real_write(STDOUT_FILENO, buffer, len);
#elif defined(STDIO_UART_DEV) && IS_USED(MODULE_PERIPH_UART)
    uart_write(STDIO_UART_DEV, buffer, len);
#else
    (void)buffer;
#endif

    return len;
}

void stdio_init(void)
{
    _init_fallback();

#if IS_USED(MODULE_VFS)
    vfs_bind_stdio();
#endif
}

ssize_t stdio_read(void* buffer, size_t count)
{
    return telnet_server_read(buffer, count);
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    int res = telnet_server_write(buffer, len);

    /* write to UART if no client is connected */
    if (res == -ENOTCONN) {
        return _write_fallback(buffer, len);
    }

    return res;
}
