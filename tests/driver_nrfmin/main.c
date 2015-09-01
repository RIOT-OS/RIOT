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
 * @brief       Test application for the NRF51822 minimal radio driver (nrfmin)
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#ifdef MODULE_NEWLIB
#   include "uart_stdio.h"
#else
#   include "posix_io.h"
#   include "board_uart0.h"
#endif
#include "nrfmin.h"
#include "net/gnrc.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc/pktdump.h"

#define SHELL_BUFSIZE       (UART0_BUFSIZE)

static char nomac_stack[THREAD_STACKSIZE_DEFAULT];

int main(void)
{
    shell_t shell;
    gnrc_netdev_t dev;
    gnrc_netreg_entry_t netobj;

    puts("\nManual test for the minimal NRF51822 radio driver\n");
    puts("Use the 'ifconfig' and 'txtsnd' shell commands to verify the driver");

    /* initialize network device */
    nrfmin_init(&dev);
    gnrc_nomac_init(nomac_stack, sizeof(nomac_stack), 5, "nomac", &dev);

    /* initialize packet dumper */
    netobj.pid = gnrc_pktdump_getpid();
    netobj.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &netobj);

    /* initialize and run the shell */
#ifndef MODULE_NEWLIB
    board_uart0_init();
    (void) posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, SHELL_BUFSIZE, uart0_readc, uart0_putc);
#else
    shell_init(&shell, NULL, SHELL_BUFSIZE, getchar, putchar);
#endif
    shell_run(&shell);

    return 0;
}
