/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_luid
 * @{
 *
 * @file
 * @brief       LUID module implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "assert.h"
#include "periph/cpuid.h"

#include "luid.h"

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

void __attribute__((weak)) luid_base(void *buf, size_t len)
{
    memset(buf, LUID_BACKUP_SEED, len);

#if CPUID_LEN
    uint8_t *out = (uint8_t *)buf;
    uint8_t cid[CPUID_LEN];

    cpuid_get(cid);
    for (size_t i = 0; i < MAX(len, CPUID_LEN); i++) {
        out[i % len] ^= cid[i % CPUID_LEN];
    }
#endif
}

static uint8_t lastused;

void luid_get(void *buf, size_t len)
{
    luid_base(buf, len);

    ((uint8_t *)buf)[0] ^= lastused++;
}

void luid_custom(void *buf, size_t len, int gen)
{
    luid_base(buf, len);

    for (size_t i = 0; i < sizeof(gen); i++) {
        ((uint8_t *)buf)[i % len] ^= ((gen >> (i * 8)) & 0xff);
    }
}

void luid_get_short(network_uint16_t *addr)
{
    luid_base(addr, sizeof(*addr));
    addr->u8[1] ^= lastused++;

    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    addr->u8[0] &= 0x7F;
}

void luid_get_eui48(eui48_t *addr)
{
    luid_base(addr, sizeof(*addr));
    addr->uint8[5] ^= lastused++;

    eui48_set_local(addr);
    eui48_clear_group(addr);
}

void luid_get_eui64(eui64_t *addr)
{
    luid_base(addr, sizeof(*addr));
    addr->uint8[7] ^= lastused++;

    eui64_set_local(addr);
    eui64_clear_group(addr);
}
