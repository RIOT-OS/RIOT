/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_help Net help
 * @ingroup     net
 * @brief       Helper functions for networking as byte order conversions and checksum calculations
 * @{
 *
 * @file        net_help.h
 *
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef __NET_HELP_H
#define __NET_HELP_H

#include <string.h>
#include <stdint.h>

#if defined(__MACH__)
#include "clang_compat.h"
#endif

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BITSET(var,pos) ((var) & (1<<(pos)))

static inline uint16_t HTONS(uint16_t a)
{
    return byteorder_htons(a).u16;
}

static inline uint32_t HTONL(uint32_t a)
{
    return byteorder_htonl(a).u32;
}

static inline uint64_t HTONLL(uint64_t a)
{
    return byteorder_htonll(a).u64;
}

static inline uint16_t NTOHS(uint16_t a)
{
    return byteorder_ntohs(*(network_uint16_t *) &a);
}

static inline uint32_t NTOHL(uint32_t a)
{
    return byteorder_ntohl(*(network_uint32_t *) &a);
}

static inline uint64_t NTOHLL(uint64_t a)
{
    return byteorder_ntohll(*(network_uint64_t *) &a);
}

uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __NET_HELP_H */
