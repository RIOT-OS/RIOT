/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup native_cpu
 * @{
 * @file
 * @brief       native reboot() implementation
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <err.h>
#include <stdlib.h>

#include "native_internal.h"
#include "netdev2_tap.h"
#include "tty_uart.h"

void reboot(void)
{
    printf("\n\n\t\t!! REBOOT !!\n\n");

#ifdef MODULE_NETDEV2_TAP
    netdev2_tap_cleanup(&netdev2_tap);
#endif

    uart_cleanup();

    if (real_execve(_native_argv[0], _native_argv, NULL) == -1) {
        err(EXIT_FAILURE, "reboot: execve");
    }

    errx(EXIT_FAILURE, "reboot: this should not have been reached");
}
