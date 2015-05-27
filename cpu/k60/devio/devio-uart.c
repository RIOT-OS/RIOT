/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Device I/O helpers for UARTs on K60, implementation.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <string.h>
#include "devio-uart.h"
#include "periph/uart.h"
#include "cpu.h"

static inline long uart_write_r(uart_t uart_num, struct _reent *r, int fd, const char *ptr,
                                int len)
{
    (void) r;
    (void) fd;
    int i = 0;

    while (i < len) {
        uart_write_blocking(uart_num, ptr[i]);
        ++i;
    }

    return i;
}

static long uart_read_r(uart_t uart_num, struct _reent *r, int fd, char *ptr, int len)
{
    /* not yet implemented */
    (void) uart_num;
    (void) r;
    (void) fd;
    (void) ptr;
    (void) len;
    return 0;
}

#if UART_0_EN
long uart0_write_r(struct _reent *r, int fd, const char *ptr, int len)
{
    return uart_write_r(UART_0, r, fd, ptr, len);
}
#endif

#if UART_1_EN
long uart1_write_r(struct _reent *r, int fd, const char *ptr, int len)
{
    return uart_write_r(UART_1, r, fd, ptr, len);
}
#endif

#if UART_2_EN
long uart2_write_r(struct _reent *r, int fd, const char *ptr, int len)
{
    return uart_write_r(UART_2, r, fd, ptr, len);
}
#endif

#if UART_3_EN
long uart3_write_r(struct _reent *r, int fd, const char *ptr, int len)
{
    return uart_write_r(UART_3, r, fd, ptr, len);
}
#endif

#if UART_4_EN
long uart4_write_r(struct _reent *r, int fd, const char *ptr, int len)
{
    return uart_write_r(UART_4, r, fd, ptr, len);
}
#endif

#if UART_0_EN
long uart0_read_r(struct _reent *r, int fd, char *ptr, int len)
{
    return uart_read_r(UART_0, r, fd, ptr, len);
}
#endif
