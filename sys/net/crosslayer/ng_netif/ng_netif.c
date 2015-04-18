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

#include <stdio.h>

#include <errno.h>
#include "kernel_types.h"
#include "net/ng_netif.h"

#ifdef MODULE_NG_IPV6_NETIF
#include "net/ng_ipv6/netif.h"
#endif

static ng_netif_handler_t if_handler[] = {
#ifdef MODULE_NG_IPV6_NETIF
    { ng_ipv6_netif_add, ng_ipv6_netif_remove },
#endif
    /* #ifdef MODULE_NG_IPV4_NETIF
     *  { ipv4_netif_add, ipv4_netif_remove },
     * #endif ... you get the idea
     */
    { NULL, NULL }
};

static kernel_pid_t ifs[NG_NETIF_NUMOF];

void ng_netif_init(void)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        ifs[i] = KERNEL_PID_UNDEF;
    }
}

int ng_netif_add(kernel_pid_t pid)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        if (ifs[i] == pid) {    /* avoid duplicates */
            return 0;
        }
        else if (ifs[i] == KERNEL_PID_UNDEF) {
            ifs[i] = pid;

            for (int j = 0; if_handler[j].add != NULL; j++) {
                if_handler[j].add(pid);
            }

            return 0;
        }
    }

    return -ENOMEM;
}

void ng_netif_remove(kernel_pid_t pid)
{
    int i;

    for (i = 0; i < NG_NETIF_NUMOF; i++) {
        if (ifs[i] == pid) {
            ifs[i] = KERNEL_PID_UNDEF;

            for (int j = 0; if_handler[j].remove != NULL; j++) {
                if_handler[j].remove(pid);
            }

            break;
        }
    }

    for (; (i < (NG_NETIF_NUMOF - 1)) && (ifs[i + 1] != KERNEL_PID_UNDEF); i++) {
        ifs[i] = ifs[i + 1];
    }

    ifs[i] = KERNEL_PID_UNDEF;  /* set in case of i == (NG_NETIF_NUMOF - 1) */
}

kernel_pid_t *ng_netif_get(size_t *size)
{
    for (*size = 0;
         (*size < NG_NETIF_NUMOF) && (ifs[*size] != KERNEL_PID_UNDEF);
         (*size)++);

    return ifs;
}

/** @} */
