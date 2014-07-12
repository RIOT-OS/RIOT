/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup sys
 * @{
 *
 * @file
 * @brief UART implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu-conf.h"
#include "chardev_thread.h"
#include "ringbuffer.h"
#include "thread.h"
#include "msg.h"
#include "posix_io.h"
#include "irq.h"

#include "board_uart0.h"

#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE       (128)
#endif

/* increase when ENABLE_DEBUG in chardev_thread is set to 1! */
#define UART0_STACKSIZE 	(KERNEL_CONF_STACKSIZE_DEFAULT)

ringbuffer_t uart0_ringbuffer;
int uart0_handler_pid;

static char buffer[UART0_BUFSIZE];

static char uart0_thread_stack[UART0_STACKSIZE];

void board_uart0_init(void)
{
    ringbuffer_init(&uart0_ringbuffer, buffer, UART0_BUFSIZE);
    int pid = thread_create(
                  uart0_thread_stack,
                  sizeof(uart0_thread_stack),
                  PRIORITY_MAIN - 1,
                  CREATE_STACKTEST | CREATE_SLEEPING,
                  chardev_thread_entry,
                  &uart0_ringbuffer,
                  "uart0"
              );
    uart0_handler_pid = pid;
    thread_wakeup(pid);
    puts("uart0_init() [OK]");
}

void uart0_handle_incoming(int c)
{
    ringbuffer_add_one(&uart0_ringbuffer, c);
}

void uart0_notify_thread(void)
{
    msg_t m;
    m.type = 0;
    msg_send_int(&m, uart0_handler_pid);
}

int uart0_readc(void)
{
    char c = 0;
    posix_read(uart0_handler_pid, &c, 1);
    return c;
}

void uart0_putc(int c)
{
    putchar(c);
}
