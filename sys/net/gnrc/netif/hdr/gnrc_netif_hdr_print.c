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
#include <inttypes.h>

#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"

void gnrc_netif_hdr_print(gnrc_netif_hdr_t *hdr)
{
    char addr_str[GNRC_NETIF_HDR_L2ADDR_PRINT_LEN];

    printf("if_pid: %u  ", (unsigned) hdr->if_pid);
    printf("rssi: %d  ", (signed) hdr->rssi);
    printf("lqi: %u\n", (unsigned) hdr->lqi);
    printf("flags: ");

    if (hdr->flags) {
        if (hdr->flags & GNRC_NETIF_HDR_FLAGS_BROADCAST) {
            printf("BROADCAST ");
        }

        if (hdr->flags & GNRC_NETIF_HDR_FLAGS_MULTICAST) {
            printf("MULTICAST ");
        }
        puts("");
    }
    else {
        puts("0x0");
    }

    if (hdr->src_l2addr_len > 0) {
        printf("src_l2addr: %s\n",
               gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(hdr),
                                      hdr->src_l2addr_len, addr_str));
    }
    else {
        puts("src_l2addr: (nil)");
    }

    if (hdr->dst_l2addr_len > 0) {
        printf("dst_l2addr: %s\n",
               gnrc_netif_addr_to_str(gnrc_netif_hdr_get_dst_addr(hdr),
                                      hdr->dst_l2addr_len, addr_str));
    }
    else {
        puts("dst_l2addr: (nil)");
    }
}

/** @} */
