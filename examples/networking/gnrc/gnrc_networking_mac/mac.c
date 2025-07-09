/*
 * SPDX-FileCopyrightText: 2018 Shuguo Zhuo
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       MAC shell commands
 *
 * @author      Shuguo Zhuo <zhuosgzju@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/gnrc.h"
#include "net/gnrc/mac/types.h"
#include "shell.h"

static int _mac_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s duty\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "duty") == 0) {
#if (GNRC_MAC_ENABLE_DUTYCYCLE_RECORD == 1)
        gnrc_netif_t *netif = NULL;
        netif = gnrc_netif_iter(netif);

        if (netif) {
            msg_t msg;
            msg.type = GNRC_MAC_TYPE_GET_DUTYCYCLE;
            msg_send(&msg, netif->pid);
        }
#else
        puts("MAC: radio duty-cycle unavailable.");
#endif
    }
    else {
        puts("error: invalid command");
    }
    return 0;
}

SHELL_COMMAND(mac, "get MAC protocol's internal information", _mac_cmd);
