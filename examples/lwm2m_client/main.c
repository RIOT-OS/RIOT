/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lwm2mclient
 * @{
 *
 * @file
 * @brief       LWM2M example implementation
 *
 * @author      Alexandru Razvan Caciulescu <alex.darredevil@gmail.com>
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdio.h>

#include "board_uart0.h"
#include "shell.h"

int main(void)
{
    shell_t shell;

    /* run the shell */
    shell_init(&shell, NULL, UART0_BUFSIZE, getchar, putchar);
    shell_run(&shell);

    return 0;
}
