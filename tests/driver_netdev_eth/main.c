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
 * @brief   Stack for the nomac thread
 */
static char nomac_stack[THREAD_STACKSIZE_DEFAULT];

/**
 * @brief   Read chars from STDIO
 */
int shell_read(void)
{
    return (int)getchar();
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
    int res;
    shell_t shell;
    ng_netreg_entry_t dump;

    puts("netdev ethernet device driver test");

    /* initialize network module(s) */
    ng_netif_init();

    /* initialize and register pktdump */
    dump.pid = ng_pktdump_init();
    dump.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;

    if (dump.pid <= KERNEL_PID_UNDEF) {
        puts("Error starting pktdump thread");
        return -1;
    }

    ng_netreg_register(NG_NETTYPE_UNDEF, &dump);

    /* initialize netdev_eth layer */
    ng_netdev_eth_init(&ng_netdev_eth, (dev_eth_t*)&dev_eth_tap);

    /* start MAC layer */
    res = ng_nomac_init(nomac_stack, sizeof(nomac_stack), THREAD_PRIORITY_MAIN - 3,
                        "tapnet_l2", (ng_netdev_t *)&ng_netdev_eth);

    if (res < 0) {
        printf("Error starting nomac thread. res=%i\n", res);
        return -1;
    }

    /* start the shell */
    shell_init(&shell, NULL, SHELL_BUFSIZE, shell_read, shell_put);
    shell_run(&shell);

    return 0;
}
