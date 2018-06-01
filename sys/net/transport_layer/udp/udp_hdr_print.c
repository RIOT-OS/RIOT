/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_udp
 * @{
 *
 * @file
 * @brief       Generic module to dump packages received via netapi to STDOUT
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/udp.h"

void udp_hdr_print(udp_hdr_t *hdr)
{
    printf("   src-port: %5" PRIu16 "  dst-port: %5" PRIu16 "\n",
           byteorder_ntohs(hdr->src_port), byteorder_ntohs(hdr->dst_port));
    printf("   length: %" PRIu16 "  cksum: 0x%04" PRIx16 "\n",
           byteorder_ntohs(hdr->length), byteorder_ntohs(hdr->checksum));
}
