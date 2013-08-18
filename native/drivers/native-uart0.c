/*
 * native uart0 implementation
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/select.h>

#include "cpu.h"
#include "debug.h"
#include "board_uart0.h"

int _native_uart_in;
int _native_uart_out;

fd_set _native_uart_rfds;

inline int uart0_puts(char *astring, int length)
{
    int nwritten, offset;

    nwritten = 0;
    offset = 0;

    _native_in_syscall = 1;

    while ((length > 0) &&(nwritten = write(_native_uart_out, astring+offset, length-offset)) > 0) {
        offset += nwritten;
    }
    if (nwritten == -1) {
        err(EXIT_FAILURE, "uart0_puts: write");
    }
    else if ((length > 0) && (nwritten == 0)) {
        /* XXX: handle properly */
        errx(EXIT_FAILURE, "uart0_puts: Could not write to stdout. I don't know what to do now.");
    }

    _native_in_syscall = 0;

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
    _native_in_syscall = 0;
    _native_in_isr = 1;

    nread = read(_native_uart_in, buf, sizeof(buf));
    if (nread == -1) {
        err(1, "_native_handle_uart0_input(): read()");
    }
    for(int pos = 0; pos < nread; pos++) {
        uart0_handle_incoming(buf[pos]);
    }
    uart0_notify_thread();

    _native_in_isr = 0;
    cpu_switch_context_exit();
}

int _native_set_uart_fds(void)
{
    DEBUG("_native_set_uart_fds");
    FD_SET(_native_uart_in, &_native_rfds);
    return _native_uart_in;
}

void _native_init_uart0()
{
    _native_uart_out = STDOUT_FILENO;
    _native_uart_in = STDIN_FILENO;

    puts("RIOT native uart0 initialized.");
}

int putchar(int c) {
    write(_native_uart_out, &c, 1);
    return 0;
}
