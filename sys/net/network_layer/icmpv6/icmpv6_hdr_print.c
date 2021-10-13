/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author smlng <s@mlng.net>
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/icmpv6.h"

void icmpv6_hdr_print(icmpv6_hdr_t *hdr)
{
    printf("   type: %3" PRIu16 "  code: %3" PRIu16 "\n",
           (uint16_t)hdr->type, (uint16_t)hdr->code);
    printf("   cksum: 0x%04" PRIx16 "\n", byteorder_ntohs(hdr->csum));
}
/** @} */
