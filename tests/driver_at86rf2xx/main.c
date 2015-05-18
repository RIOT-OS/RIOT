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
#include "posix_io.h"
#include "board_uart0.h"
#include "net/ng_pktdump.h"
#include "net/ng_netbase.h"

/**
 * @brief   Buffer size used by the shell
 */
#define SHELL_BUFSIZE           (64U)

/**
 * @brief   Read chars from STDIO
 */
int shell_read(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

/**
 * @brief   Write chars to STDIO
 */
void shell_put(int c)
{
    putchar((char)c);
}

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    shell_t shell;
    ng_netreg_entry_t dump;

    puts("AT86RF2xx device driver test");

    /* register the pktdump thread */
    puts("Register the packet dump thread for NG_NETTYPE_UNDEF packets");
    dump.pid = ng_pktdump_getpid();
    dump.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    ng_netreg_register(NG_NETTYPE_UNDEF, &dump);

    /* start the shell */
    puts("Initialization successful - starting the shell now");
    (void) posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, SHELL_BUFSIZE, shell_read, shell_put);
    shell_run(&shell);

    return 0;
}
