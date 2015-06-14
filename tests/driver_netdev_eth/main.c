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
#include "net/ng_netbase.h"
#include "net/ng_nomac.h"
#include "net/ng_pktdump.h"
#include "net/ng_netdev_eth.h"
#include "net/dev_eth.h"
#include "dev_eth_tap.h"

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
    ng_netreg_entry_t dump;

    puts("netdev ethernet device driver test");

    /* initialize and register pktdump */
    dump.pid = ng_pktdump_init();
    dump.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;

    if (dump.pid <= KERNEL_PID_UNDEF) {
        puts("Error starting pktdump thread");
        return -1;
    }

    ng_netreg_register(NG_NETTYPE_UNDEF, &dump);

    /* start the shell */
    shell_init(&shell, NULL, SHELL_BUFSIZE, getchar, putchar);
    shell_run(&shell);

    return 0;
}
