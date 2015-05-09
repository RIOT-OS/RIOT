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
 * @brief       Test application for the NRF51822 BLE minimal radio driver (blemin)
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "posix_io.h"
#include "board_uart0.h"
#include "net/ng_ble.h"
#include "net/gnrc.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc/pktdump.h"

#define SHELL_BUFSIZE       (UART0_BUFSIZE)

static char nomac_stack[THREAD_STACKSIZE_DEFAULT];

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
    gnrc_netdev_t dev;
    gnrc_netreg_entry_t netobj;

    puts("\nManual test for the minimal NRF51822 BLE radio driver\n");
    puts("Use the 'ifconfig' and 'blesnd' shell commands to verify the driver");

    /* initialize network device */
    blemin_init(&dev);
    gnrc_nomac_init(nomac_stack, sizeof(nomac_stack), 5, "nomac", &dev);

    /* initialize packet dumper */
    netobj.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    //netobj.pid = gnrc_pktdump_init(dump_stack, sizeof(dump_stack), 6, "dump");
    netobj.pid = gnrc_pktdump_init();
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &netobj);

    /* initialize and run the shell */
    board_uart0_init();
    posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, SHELL_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}
