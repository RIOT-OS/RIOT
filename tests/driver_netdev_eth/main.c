/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *                    Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Test application for netdev ethernet device driver
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *              Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "kernel.h"
#include "shell.h"
#include "shell_commands.h"
#include "net/gnrc.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/netdev_eth.h"
#include "net/dev_eth.h"
#include "dev_eth_tap.h"

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    gnrc_netreg_entry_t dump;

    puts("netdev ethernet device driver test");

    /* initialize and register pktdump */
    dump.pid = gnrc_pktdump_init();
    dump.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;

    if (dump.pid <= KERNEL_PID_UNDEF) {
        puts("Error starting pktdump thread");
        return -1;
    }

    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
