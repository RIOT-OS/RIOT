/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython HAL implementation for RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "py/mpconfig.h"

int mp_hal_stdin_rx_chr(void)
{
    /* Read through RIOT's stdio (getchar()) rather than a raw read() on
     * STDIN_FILENO. With MODULE_STDIN enabled (see Makefile.dep), the native
     * port installs an asynchronous (SIGIO) handler that drains stdin into an
     * isrpipe and switches the file descriptor to non-blocking mode. A direct
     * read() would race that handler for incoming bytes (losing, among others,
     * the line-terminating newline) and busy-loop on EAGAIN. getchar() routes
     * through RIOT's stdio and blocks until a byte is available on every
     * supported platform. */
    int c = getchar();

    if (c == EOF) {
        return 0;
    }

    /* The REPL line editor treats '\r' as "enter"; translate incoming '\n'
     * accordingly (matches MicroPython's unix port). */
    if (c == '\n') {
        c = '\r';
    }

    return c;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len)
{
    int r = write(STDOUT_FILENO, str, len);

    (void)r;
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len)
{
    while (len--) {
        if (*str == '\n') {
            mp_hal_stdout_tx_strn("\r", 1);
        }
        mp_hal_stdout_tx_strn(str++, 1);
    }
}

void mp_hal_stdout_tx_str(const char *str)
{
    mp_hal_stdout_tx_strn(str, strlen(str));
}
