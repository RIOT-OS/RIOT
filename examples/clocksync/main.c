/*
 * Copyright (C) 2014 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       clocksync test application
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"

static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

int main(void)
{
    shell_t shell;
    (void) posix_open(uart0_handler_pid, 0);

    (void) puts("Welcome to RIOT!");
    shell_init(&shell, NULL, UART0_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&shell);
    return 0;
}
