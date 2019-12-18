/*
 * Copyright (C) 2019 William MARTIN <william.martin@power-lan.com>
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

#include "assert.h"
#include "net/ipv4/addr.h"

ipv4_addr_t ipv4_mask_to_addr(const uint8_t mask)
{
    ipv4_addr_t addr;

    assert(mask <= 32);
    uint32_t m = ((uint32_t)0xFFFFFFFF << (32 - mask));

    addr.u32 = byteorder_htonl(m);
    return addr;
}


/** @} */
