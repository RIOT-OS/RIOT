/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @brief       Test application for ng_tapnet network device driver
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "kernel.h"
#include "shell.h"
#include "shell_commands.h"
#include "ng_tapnet.h"
#include "net/ng_netbase.h"
#include "net/ng_nomac.h"
#include "net/ng_pktdump.h"

/**
 * @brief   Buffer size used by the shell
 */
#define SHELL_BUFSIZE           (64U)

/**
 * @brief   Stack for the nomac thread
 */
static char nomac_stack[KERNEL_CONF_STACKSIZE_DEFAULT];

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

    puts("tapnet device driver test");
    printf("Initializing tapnet... \n");

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

    /* ng_tapnet is initialized on native start-up */
    /* start MAC layer */
    res = ng_nomac_init(nomac_stack, sizeof(nomac_stack), PRIORITY_MAIN - 3,
                        "tapnet_l2", (ng_netdev_t *)&ng_tapnet);

    if (res < 0) {
        puts("Error starting nomac thread");
        return -1;
    }

    /* start the shell */
    shell_init(&shell, NULL, SHELL_BUFSIZE, shell_read, shell_put);
    shell_run(&shell);

    return 0;
}
