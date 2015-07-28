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
#include "log.h"

#include "net/ng_netif.h"
#include "net/ng_sixlowpan/netif.h"

static ng_sixlowpan_netif_t sixlow_ifs[NG_NETIF_NUMOF];

void ng_sixlowpan_netif_init(void)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        sixlow_ifs[i].pid = KERNEL_PID_UNDEF;
        sixlow_ifs[i].max_frag_size = 0;
    }
}

void ng_sixlowpan_netif_add(kernel_pid_t pid, uint16_t max_frag_size)
{
    ng_sixlowpan_netif_t *free_entry = NULL;

    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        if (sixlow_ifs[i].pid == pid) {
            return;
        }

        if ((sixlow_ifs[i].pid == KERNEL_PID_UNDEF) && !free_entry) {
            /* found the first free entry */
            free_entry = &sixlow_ifs[i];
        }
    }

    if (!free_entry) {
        LOG_ERROR("ng_sixlowpan_netif_add: couldn't add interface with PID %d: No space left.\n", pid);
        return;
    }

    free_entry->pid = pid;
    free_entry->max_frag_size = max_frag_size;
#ifdef MODULE_NG_SIXLOWPAN_IPHC
    free_entry->iphc_enabled = true;
#endif
    return;
}

void ng_sixlowpan_netif_remove(kernel_pid_t pid)
{
    ng_sixlowpan_netif_t *entry = ng_sixlowpan_netif_get(pid);

    entry->pid = KERNEL_PID_UNDEF;
}

ng_sixlowpan_netif_t *ng_sixlowpan_netif_get(kernel_pid_t pid)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        if (sixlow_ifs[i].pid == pid) {
            return sixlow_ifs + i;
        }
    }

    return NULL;
}

/** @} */
