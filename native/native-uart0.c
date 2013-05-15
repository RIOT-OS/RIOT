/*
 * TODO: 
 * make stdin/stdout customizable.
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

#include "cpu.h"
#include "debug.h"
#include "board_uart0.h"

fd_set _native_uart_rfds;

static inline int uart0_puts(char *astring, int length)
{
    return puts(astring);
}

void _native_handle_uart0_input()
{
    char buf[42];
    int nread;

    _native_in_syscall = 0;
    _native_in_isr = 1;

    nread = read(0, buf, sizeof(buf));
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

void _native_init_uart0()
{
    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&_native_uart_rfds);
    FD_SET(0, &_native_uart_rfds);
    puts("RIOT native uart0 initialized.");
}

int putchar(int c) {
    write(1, &c, 1);
    return 0;
}
