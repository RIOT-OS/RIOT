/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */

#include <inttypes.h>
#include <stdio.h>

#include "net/ipv4/hdr.h"

void ipv4_hdr_print(ipv4_hdr_t *hdr)
{
    char addr_str[IPV4_ADDR_MAX_STR_LEN];

    printf("version: %u  IHL: %u  type of service: 0x%02x\n",
           (unsigned)ipv4_hdr_get_version(hdr), (unsigned)ipv4_hdr_get_ihl(hdr),
           (unsigned)hdr->ts);
    printf("total length: %" PRIu16 "  identification: 0x%04" PRIx16 "\n",
           byteorder_ntohs(hdr->tl), byteorder_ntohs(hdr->id));
    printf("flags: 0x%x  fragment offset: %" PRIu16 "\n",
           (unsigned)ipv4_hdr_get_flags(hdr), ipv4_hdr_get_fo(hdr));
    printf("TTL: %u  protocol: %u  checksum: 0x%04" PRIx16 "\n",
           (unsigned)hdr->ttl, (unsigned)hdr->protocol,
           byteorder_ntohs(hdr->csum));
    printf("source address: %s\n", ipv4_addr_to_str(addr_str, &hdr->src,
                                                    sizeof(addr_str)));
    printf("destination address: %s\n", ipv4_addr_to_str(addr_str, &hdr->dst,
                                                         sizeof(addr_str)));
}

/** @} */
