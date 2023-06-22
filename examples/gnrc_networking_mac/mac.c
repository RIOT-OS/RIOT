/*
 * Copyright (C) 2018 Shuguo Zhuo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

int mac_cmd(int argc, char **argv)
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
