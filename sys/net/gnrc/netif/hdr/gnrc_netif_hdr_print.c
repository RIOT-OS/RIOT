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
    char addr_str[GNRC_NETIF_HDR_L2ADDR_MAX_LEN * 3];

    printf("if_pid: %" PRIkernel_pid "  ", hdr->if_pid);
    printf("rssi: %" PRIu8 "  ", hdr->rssi);
    printf("lqi: %" PRIu8 "\n", hdr->lqi);

    if (hdr->src_l2addr_len > 0) {
        printf("src_l2addr: %s\n",
               gnrc_netif_addr_to_str(addr_str, sizeof(addr_str),
                                      gnrc_netif_hdr_get_src_addr(hdr),
                                      (size_t)hdr->src_l2addr_len));
    }
    else {
        puts("src_l2addr: (nil)");
    }

    if (hdr->dst_l2addr_len > 0) {
        printf("dst_l2addr: %s\n",
               gnrc_netif_addr_to_str(addr_str, sizeof(addr_str),
                                      gnrc_netif_hdr_get_dst_addr(hdr),
                                      (size_t)hdr->dst_l2addr_len));
    }
    else {
        puts("dst_l2addr: (nil)");
    }
}

/** @} */
