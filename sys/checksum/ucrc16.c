/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <inttypes.h>

#include "checksum/ucrc16.h"

#define UINT8_BIT_SIZE              (8U)
#define UINT16_BIT_SIZE             (16U)
#define LEFTMOST_BIT_SET(value)     ((value) & 0x8000U)
#define RIGHTMOST_BIT_SET(value)    ((value) & 0x0001U)

uint16_t ucrc16_calc_be(const uint8_t *buf, size_t len, uint16_t poly,
                        uint16_t seed)
{
    assert(buf != NULL);
    for (unsigned c = 0; c < len; c++, buf++) {
        uint32_t tmp = seed ^ (*buf << (UINT16_BIT_SIZE - UINT8_BIT_SIZE));
        for (unsigned i = 0; i < UINT8_BIT_SIZE; i++) {
            tmp = LEFTMOST_BIT_SET(tmp) ? ((tmp << 1) ^ poly) : (tmp << 1);
        }
        seed = tmp & UINT16_MAX;
    }
    return seed;
}

uint16_t ucrc16_calc_le(const uint8_t *buf, size_t len, uint16_t poly,
                        uint16_t seed)
{
    assert(buf != NULL);
    for (unsigned c = 0; c < len; c++, buf++) {
        seed ^= (*buf);
        for (unsigned i = 0; i < UINT8_BIT_SIZE; i++) {
            seed = RIGHTMOST_BIT_SET(seed) ? ((seed >> 1) ^ poly) : (seed >> 1);
        }
    }
    return seed;
}

/** @} */
