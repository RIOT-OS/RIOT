/**
 * Native uart0 implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the LGPLv2. See
 * the file LICENSE in the top level directory for more details.
 *
 * @ingroup native_board
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <sys/select.h>

#include "cpu.h"
#include "debug.h"
#include "board_uart0.h"

#include "native_internal.h"

static int _native_uart_in;

fd_set _native_uart_rfds;

int uart0_puts(char *astring, int length)
{
    int nwritten, offset;

    nwritten = 0;
    offset = 0;

    while (
            (length - offset > 0) && (
                (nwritten = write(
                               STDOUT_FILENO,
                               astring+offset,
                               length-offset)
                ) > 0)
          ) {
        offset += nwritten;
    }
    if (nwritten == -1) {
        err(EXIT_FAILURE, "uart0_puts: write");
    }
    else if ((length > 0) && (nwritten == 0)) {
        /* XXX: handle properly */
        errx(EXIT_FAILURE, "uart0_puts: Could not write to stdout. I don't know what to do now.");
    }

    return length;
}

void _native_handle_uart0_input()
{
    char buf[42];
    int nread;

    if (!FD_ISSET(_native_uart_in, &_native_rfds)) {
        DEBUG("_native_handle_uart0_input - nothing to do\n");
        return;
    }
    DEBUG("_native_handle_uart0_input\n");

    nread = read(_native_uart_in, buf, sizeof(buf));
    if (nread == -1) {
        err(1, "_native_handle_uart0_input(): read()");
    }
    else if (nread == 0) {
        /* XXX:
         * preliminary resolution for this situation, will be coped
         * with properly in #161 */
        close(_native_uart_in);
        _native_uart_in = -1;
        printf("stdin closed");
    }
    for(int pos = 0; pos < nread; pos++) {
        uart0_handle_incoming(buf[pos]);
    }
    uart0_notify_thread();

    thread_yield();
}

int _native_set_uart_fds(void)
{
    DEBUG("_native_set_uart_fds");
    if (_native_uart_in != -1) {
        FD_SET(_native_uart_in, &_native_rfds);
    }
    return _native_uart_in;
}

void _native_init_uart0()
{
    _native_uart_in = STDIN_FILENO;

    puts("RIOT native uart0 initialized.");
}
