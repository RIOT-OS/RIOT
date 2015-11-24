/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 INRIA
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
#include "kernel_types.h"
#include "net/gnrc/netif.h"

#ifdef MODULE_GNRC_IPV6_NETIF
#include "net/gnrc/ipv6/netif.h"
#endif

static gnrc_netif_handler_t if_handler[] = {
#ifdef MODULE_GNRC_IPV6_NETIF
    { gnrc_ipv6_netif_add, gnrc_ipv6_netif_remove },
#endif
    /* #ifdef MODULE_GNRC_IPV4_NETIF
     *  { ipv4_netif_add, ipv4_netif_remove },
     * #endif ... you get the idea
     */
    { NULL, NULL }
};

static kernel_pid_t ifs[GNRC_NETIF_NUMOF];

void gnrc_netif_init(void)
{
    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        ifs[i] = KERNEL_PID_UNDEF;
    }
}

int gnrc_netif_add(kernel_pid_t pid)
{
    kernel_pid_t *free_entry = NULL;

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (ifs[i] == pid) {
            return 0;
        }
        else if (ifs[i] == KERNEL_PID_UNDEF && !free_entry) {
            free_entry = &ifs[i];
        }
    }

    if (!free_entry) {
        return -ENOMEM;
    }

    *free_entry = pid;

    for (int j = 0; if_handler[j].add != NULL; j++) {
        if_handler[j].add(pid);
    }

    return 0;
}

void gnrc_netif_remove(kernel_pid_t pid)
{
    int i;

    for (i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (ifs[i] == pid) {
            ifs[i] = KERNEL_PID_UNDEF;

            for (int j = 0; if_handler[j].remove != NULL; j++) {
                if_handler[j].remove(pid);
            }

            return;
        }
    }
}

size_t gnrc_netif_get(kernel_pid_t *netifs)
{
    size_t size = 0;

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (ifs[i] != KERNEL_PID_UNDEF) {
            netifs[size++] = ifs[i];
        }
    }

    return size;
}

bool gnrc_netif_exist(kernel_pid_t pid)
{
    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (ifs[i] == pid) {
            return true;
        }
    }
    return false;
}

/** @} */
