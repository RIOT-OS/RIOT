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

#include "net/ng_netif.h"
#include "net/ng_netif/hdr.h"

void ng_netif_hdr_print(ng_netif_hdr_t *hdr)
{
    char addr_str[NG_NETIF_HDR_L2ADDR_MAX_LEN * 3];

    printf("if_pid: %" PRIkernel_pid "  ", hdr->if_pid);
    printf("rssi: %" PRIu8 "  ", hdr->rssi);
    printf("lqi: %" PRIu8 "\n", hdr->lqi);
    printf("src_l2addr: %s\n",
           ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                ng_netif_hdr_get_src_addr(hdr),
                                (size_t)hdr->src_l2addr_len));
    printf("dst_l2addr: %s\n",
           ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                ng_netif_hdr_get_dst_addr(hdr),
                                (size_t)hdr->dst_l2addr_len));
}

/** @} */
