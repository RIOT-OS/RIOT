/*
 * Copyright (C) Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for displaying neighbor statistics
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/gnrc/netif.h"
#include "net/netstats.h"
#include "net/netstats/neighbor.h"

static void _print_neighbors(netif_t *dev)
{
    netstats_nb_t *stats = &dev->neighbors.pstats[0];
    unsigned header_len = 0;
    char l2addr_str[3 * L2UTIL_ADDR_MAX_LEN];
    puts("Neighbor link layer stats:");
    header_len += printf("L2 address               fresh");
    if (IS_USED(MODULE_NETSTATS_NEIGHBOR_ETX)) {
        header_len += printf("  etx");
    }
    if (IS_USED(MODULE_NETSTATS_NEIGHBOR_COUNT)) {
        header_len += printf(" sent received");
    }
    if (IS_USED(MODULE_NETSTATS_NEIGHBOR_RSSI)) {
     header_len += printf("   rssi ");
    }
    if (IS_USED(MODULE_NETSTATS_NEIGHBOR_LQI)) {
        header_len += printf(" lqi");
    }
    if (IS_USED(MODULE_NETSTATS_NEIGHBOR_TX_TIME)) {
        header_len += printf(" avg tx time");
    }
    printf("\n");

    while (header_len--) {
        printf("-");
    }
    printf("\n");

    for (unsigned i = 0; i < NETSTATS_NB_SIZE; ++i) {
        netstats_nb_t *entry = &stats[i];

        if (entry->l2_addr_len == 0) {
            continue;
        }

        printf("%-24s ",
               gnrc_netif_addr_to_str(entry->l2_addr, entry->l2_addr_len, l2addr_str));
        if (netstats_nb_isfresh(dev, entry)) {
            printf("%5u", (unsigned)entry->freshness);
        } else {
            printf("STALE");
        }

#if IS_USED(MODULE_NETSTATS_NEIGHBOR_ETX)
        printf(" %3u%%", (100 * entry->etx) / NETSTATS_NB_ETX_DIVISOR);
#endif
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_COUNT)
        printf(" %4"PRIu16" %8"PRIu16, entry->tx_count, entry->rx_count);
#endif
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_RSSI)
        printf(" %4i dBm", (int8_t) entry->rssi);
#endif
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_LQI)
        printf(" %u", entry->lqi);
#endif
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_TX_TIME)
        printf(" %7"PRIu32" µs", entry->time_tx_avg);
#endif
        printf("\n");
    }
}

int _netstats_nb(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    gnrc_netif_t *netif = NULL;
    while ((netif = gnrc_netif_iter(netif))) {
        _print_neighbors(&netif->netif);
    }

    return 0;
}
