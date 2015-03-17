/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/netif.h"
#include "net/ng_nomac.h"
#include "net/ng_zep.h"
#include "thread.h"

static ng_zep_t zep;
static char zep_stack[THREAD_STACKSIZE_DEFAULT];

int _zep_init(int argc, char **argv)
{
    uint16_t src_port = NG_ZEP_DEFAULT_PORT;
    uint16_t dst_port = NG_ZEP_DEFAULT_PORT;
    ng_ipv6_addr_t dst_addr;
    int res;

    if (argc < 2) {
        printf("usage: %s dst_addr [src_port [dst_port]]\n", argv[0]);
        return 1;
    }

    if (argc > 2) {
        src_port = (uint16_t)atoi(argv[2]);
    }

    if (argc > 3) {
        dst_port = (uint16_t)atoi(argv[3]);
    }

    ng_ipv6_addr_from_str(&dst_addr, argv[1]);

    if ((res = ng_zep_init(&zep, src_port, &dst_addr, dst_port)) < 0) {
        switch (res) {
            case -EADDRINUSE:
                printf("error: Source port %" PRIu16 " already in use\n", src_port);
                break;

            case -EEXIST:
                puts("error: ZEP already intialized");
                break;

            case -ENOTSUP:
                printf("error: dst_addr (%s) invalid\n", argv[1]);
                break;

            case -EOVERFLOW:
                puts("error: too many threads running");
                break;

            default:
                puts("unexpected error");
                break;
        }

        return 1;
    }

    if ((res = ng_nomac_init(zep_stack, sizeof(zep_stack), THREAD_PRIORITY_MAIN - 3,
                             "zep_l2", (ng_netdev_t *)&zep)) < 0) {
        switch (res) {
            case -EOVERFLOW:
                puts("error: too many threads running");
                break;

            default:
                puts("unexpected error");
                break;
        }

        return 1;
    }

#ifdef MODULE_NG_IPV6_NETIF
    ng_ipv6_netif_init_by_dev();
#endif

    return 0;
}

/** @} */
