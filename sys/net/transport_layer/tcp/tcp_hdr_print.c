/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     net_tcp
 * @{
 *
 * @file
 *
 * @author smlng <s@mlng.net>
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/tcp.h"

void tcp_hdr_print(tcp_hdr_t *hdr)
{
    printf("   src-port: %5" PRIu16 "  dst-port: %5" PRIu16 "\n",
           byteorder_ntohs(hdr->src_port), byteorder_ntohs(hdr->dst_port));
    printf("   seq-num: %10" PRIu32 "  ack-num: %10" PRIu32 "\n",
           byteorder_ntohl(hdr->seq_num), byteorder_ntohl(hdr->ack_num));
    printf("   off_ctl: %5" PRIu16 "  window: %5" PRIu16 "\n",
           byteorder_ntohs(hdr->off_ctl), byteorder_ntohs(hdr->window));
    printf("   cksum: 0x%04" PRIx16 "  urgent_ptr: %5" PRIu16 "\n",
           byteorder_ntohs(hdr->checksum), byteorder_ntohs(hdr->urgent_ptr));
}
/** @} */
