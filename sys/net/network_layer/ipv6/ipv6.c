/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  net_ipv6
 * @{
 *
 * @file        ipv6.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "byteorder.h"
#include "kernel_types.h"
#include "net_help.h"

#include "ipv6.h"

kernel_pid_t ipv6_pid = KERNEL_PID_UNDEF;

uint16_t ipv6_pseudo_hdr_csum(const ipv6_hdr_t *ipv6_hdr, uint32_t ul_packet_len,
                              uint8_t next_header, uint16_t sum)
{
    network_uint32_t length, nh;
    length = byteorder_htonl(ul_packet_len);
    nh.u32 = 0;
    nh.u8[3] = next_header;

    sum = net_help_csum(sum, (uint8_t *)(&ipv6_hdr->srcaddr),
                        2 * sizeof(ipv6_addr_t));
    sum = net_help_csum(sum, (uint8_t *)(&length), sizeof(length));
    sum = net_help_csum(sum, (uint8_t *)(&nh), sizeof(nh));

    return sum;
}

/**
 * @}
 */
