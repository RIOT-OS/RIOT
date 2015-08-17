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

#include "kernel_types.h"

#include "net/gnrc/netif.h"
#include "net/gnrc/sixlowpan/netif.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static gnrc_sixlowpan_netif_t sixlow_ifs[GNRC_NETIF_NUMOF];

void gnrc_sixlowpan_netif_init(void)
{
    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        sixlow_ifs[i].pid = KERNEL_PID_UNDEF;
        sixlow_ifs[i].max_frag_size = 0;
    }
}

void gnrc_sixlowpan_netif_add(kernel_pid_t pid, uint16_t max_frag_size)
{
    gnrc_sixlowpan_netif_t *free_entry = NULL;

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (sixlow_ifs[i].pid == pid) {
            return;
        }

        if ((sixlow_ifs[i].pid == KERNEL_PID_UNDEF) && !free_entry) {
            /* found the first free entry */
            free_entry = &sixlow_ifs[i];
        }
    }

    if (!free_entry) {
        DEBUG("gnrc_sixlowpan_netif_add: couldn't add interface with PID %d: No space left.\n", pid);
        return;
    }

    free_entry->pid = pid;
    free_entry->max_frag_size = max_frag_size;
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
    free_entry->iphc_enabled = true;
#endif
    return;
}

void gnrc_sixlowpan_netif_remove(kernel_pid_t pid)
{
    gnrc_sixlowpan_netif_t *entry = gnrc_sixlowpan_netif_get(pid);

    entry->pid = KERNEL_PID_UNDEF;
}

gnrc_sixlowpan_netif_t *gnrc_sixlowpan_netif_get(kernel_pid_t pid)
{
    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (sixlow_ifs[i].pid == pid) {
            return sixlow_ifs + i;
        }
    }

    return NULL;
}

/** @} */
