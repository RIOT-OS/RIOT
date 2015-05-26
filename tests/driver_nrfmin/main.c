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
#include "posix_io.h"
#include "board_uart0.h"
#include "nrfmin.h"
#include "net/ng_netbase.h"
#include "net/ng_nomac.h"
#include "net/ng_pktdump.h"

#define SHELL_BUFSIZE       (UART0_BUFSIZE)

static char nomac_stack[KERNEL_CONF_STACKSIZE_DEFAULT];

int shell_read(void)
{
    char c;
    int result = posix_read(uart0_handler_pid, &c, 1);
    if (result != 1) {
        return -1;
    }
    return (unsigned char) c;
}

void shell_put(int c)
{
    putchar(c);
}

int main(void)
{
    shell_t shell;
    ng_netdev_t dev;
    ng_netreg_entry_t netobj;

    puts("\nManual test for the minimal NRF51822 radio driver\n");
    puts("Use the 'ifconfig' and 'txtsnd' shell commands to verify the driver");

    /* initialize network device */
    nrfmin_init(&dev);
    ng_nomac_init(nomac_stack, sizeof(nomac_stack), 5, "nomac", &dev);

    /* initialize packet dumper */
    netobj.pid = ng_pktdump_getpid();
    netobj.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    ng_netreg_register(NG_NETTYPE_UNDEF, &netobj);

    /* initialize and run the shell */
    board_uart0_init();
    posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, SHELL_BUFSIZE, shell_read, shell_put);
    shell_run(&shell);

    return 0;
}
