/*
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief IRQ test application
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board_uart0.h"
#include "posix_io.h"
#include "hwtimer.h"
#include "thread.h"

char busy_stack[KERNEL_CONF_STACKSIZE_PRINTF];
int busy, i, k;

void busy_thread(void)
{
    int j;
    printf("busy_thread starting\n");

    i = 0;

    while (busy) {
        k = j = ++i;
    }

    printf("i: %i\n", i);
    printf("j: %i\n", j);
    printf("k: %i\n", k);

    printf("success\n");
}


int main(void)
{
    posix_open(uart0_handler_pid, 0);

    busy = 1;
    k = 23;
    thread_create(busy_stack, KERNEL_CONF_STACKSIZE_PRINTF,
                  PRIORITY_MAIN + 1, CREATE_WOUT_YIELD, busy_thread,
                  "busy_thread");
    printf("busy_thread created\n");

    printf("hwtimer_wait()\n");
    hwtimer_wait(HWTIMER_TICKS(100000));
    busy = 0;

    printf("main: return\n");
}
