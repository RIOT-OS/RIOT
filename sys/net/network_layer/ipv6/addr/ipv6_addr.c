/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fmt.h"
#include "bitarithm.h"
#include "kernel_defines.h"
#include "net/ipv6/addr.h"

const ipv6_addr_t ipv6_addr_unspecified = IPV6_ADDR_UNSPECIFIED;
const ipv6_addr_t ipv6_addr_loopback = IPV6_ADDR_LOOPBACK;
const ipv6_addr_t ipv6_addr_link_local_prefix = IPV6_ADDR_LINK_LOCAL_PREFIX;
const ipv6_addr_t ipv6_addr_solicited_node_prefix = IPV6_ADDR_SOLICITED_NODE_PREFIX;
const ipv6_addr_t ipv6_addr_all_nodes_if_local = IPV6_ADDR_ALL_NODES_IF_LOCAL;
const ipv6_addr_t ipv6_addr_all_nodes_link_local = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
const ipv6_addr_t ipv6_addr_all_routers_if_local = IPV6_ADDR_ALL_ROUTERS_IF_LOCAL;
const ipv6_addr_t ipv6_addr_all_routers_link_local = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
const ipv6_addr_t ipv6_addr_all_routers_site_local = IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL;

bool ipv6_addr_equal(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    return (a->u64[0].u64 == b->u64[0].u64) &&
           (a->u64[1].u64 == b->u64[1].u64);
}

uint8_t ipv6_addr_match_prefix(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    uint8_t prefix_len = 0;

    if ((a == NULL) || (b == NULL)) {
        return 0;
    }

    if (a == b) {
        return 128;
    }

    for (int i = 0; i < 16; i++) {
        uint8_t xor = a->u8[i] ^ b->u8[i];
        if (xor) {
            /* if bytes aren't equal count matching leading bits */
            prefix_len += bitarithm_clzb(xor);
            break;
        }
        else {
            /* if bytes are equal add 8 */
            prefix_len += 8;
        }
    }

    return prefix_len;
}

bool ipv6_addr_match_mask(const ipv6_addr_t *a, const ipv6_addr_t *b,
                          const ipv6_addr_t *mask)
{
    for (int i = 0; i < 16; i++) {
        if ((a->u8[i] ^ b->u8[i]) & mask->u8[i]) {
            return false;
        }
    }

    return true;
}

void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix,
                           uint8_t bits)
{
    uint8_t bytes;

    if (bits > 128) {
        bits = 128;
    }

    bytes = bits / 8;
    bits  = bits % 8;

    memcpy(out, prefix, bytes);

    if (bits) {
        uint8_t mask = 0xff << (8 - bits);

        out->u8[bytes] &= ~mask;
        out->u8[bytes] |= (prefix->u8[bytes] & mask);
    }
}

void ipv6_addr_init_iid(ipv6_addr_t *out, const uint8_t *iid, uint8_t bits)
{
    uint8_t unaligned_bits, bytes, pos;

    if (bits > 128) {
        bits = 128;
    }

    unaligned_bits = bits % 8;
    bytes = bits / 8;
    pos = (IPV6_ADDR_BIT_LEN / 8) - bytes;

    if (unaligned_bits) {
        uint8_t mask = 0xff << unaligned_bits;
        out->u8[pos - 1] &= mask;
        out->u8[pos - 1] |= (*iid & ~mask);
        iid++;
    }

    memcpy(&(out->u8[pos]), iid, bytes);
}

char *ipv6_addr_split_str(char *addr_str, char separator)
{
    char *sep = addr_str;
    while (*(++sep)) {
        if (*sep == separator) {
            *sep++ = '\0';
            break;
        }
    }

    return *sep ? sep : NULL;
}

int ipv6_addr_split_int(char *addr_str, char separator, int _default)
{
    char *val = ipv6_addr_split_str(addr_str, separator);
    return val ? atoi(val) : _default;
}

void ipv6_addr_print(const ipv6_addr_t *addr)
{
    assert(addr);
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    ipv6_addr_to_str(addr_str, addr, sizeof(addr_str));

    if (IS_USED(MODULE_FMT)) {
        print_str(addr_str);
    }
    else {
        printf("%s", addr_str);
    }
}

void ipv6_addrs_print(const ipv6_addr_t *addrs, size_t num,
                      const char *separator)
{
    if (num == 0) {
        return;
    }

    num--;
    char buf[IPV6_ADDR_MAX_STR_LEN];
    for (size_t idx = 0; idx < (size_t)num; idx++) {
        ipv6_addr_to_str(buf, &addrs[idx], sizeof(buf));
        if (IS_USED(MODULE_FMT)) {
            print_str(buf);
            print_str(separator);
        }
        else {
            printf("%s%s", buf, separator);
        }
    }

    ipv6_addr_to_str(buf, &addrs[num], sizeof(buf));
    if (IS_USED(MODULE_FMT)) {
        print_str(buf);
    }
    else {
        printf("%s", buf);
    }
}

/**
 * @}
 */
