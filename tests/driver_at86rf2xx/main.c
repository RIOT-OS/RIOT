/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for AT86RF2xx network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#ifdef MODULE_NEWLIB
#   include "uart_stdio.h"
#else
#   include "posix_io.h"
#   include "board_uart0.h"
#endif
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

/**
 * @brief   Buffer size used by the shell
 */
#define SHELL_BUFSIZE           (64U)

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    shell_t shell;
    gnrc_netreg_entry_t dump;

    puts("AT86RF2xx device driver test");

    /* register the pktdump thread */
    puts("Register the packet dump thread for GNRC_NETTYPE_UNDEF packets");
    dump.pid = gnrc_pktdump_getpid();
    dump.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    /* start the shell */
    puts("Initialization successful - starting the shell now");
#ifndef MODULE_NEWLIB
    (void) posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, SHELL_BUFSIZE, uart0_readc, uart0_putc);
#else
    shell_init(&shell, NULL, SHELL_BUFSIZE, getchar, putchar);
#endif
    shell_run(&shell);

    return 0;
}
