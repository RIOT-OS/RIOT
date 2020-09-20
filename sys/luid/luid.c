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
    uint8_t *out = buf;
    uint8_t cid[CPUID_LEN];
    uint8_t sum = 0;

    cpuid_get(cid);

    /* CPU ID is XORed with LUID_BACKUP_SEED.
     * Then Fisher–Yates shuffle is performed to mix
     * the bytes, as often large continuous chunks of
     * CPU ID are equal on machines with the same CPU.
     */
    for (size_t i = 0; i < MAX(len, CPUID_LEN); i++) {
        uint8_t tmp, j, k;
        j = i % len;
        k = i % CPUID_LEN;

        /* xor with CPU ID */
        out[j] ^= cid[k];

        /* get 'random' position */
        sum += out[j];
        k = sum % (j + 1);

        /* shuffle bytes */
        tmp    = out[j];
        out[j] = out[k];
        out[k] = tmp;
    }
#endif
}

static uint8_t lastused;

void luid_get(void *buf, size_t len)
{
    luid_base(buf, len);

    ((uint8_t *)buf)[0] ^= lastused++;
}

void luid_get_lb(void *buf, size_t len)
{
    luid_base(buf, len);

    ((uint8_t *)buf)[len - 1] ^= lastused++;
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

void luid_netdev_get_eui48(const netdev_t *netdev, eui48_t *addr)
{
    luid_base(addr, sizeof(*addr));
#ifdef MODULE_NETDEV_REGISTER
    addr->uint8[4] ^= netdev->type;
    addr->uint8[5] ^= netdev->index;
#else
    /* we should only get here with gnrc_netif_single */
    (void)netdev;
#endif

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

void luid_netdev_get_eui64(const netdev_t *netdev, eui64_t *addr)
{
    luid_base(addr, sizeof(*addr));
#ifdef MODULE_NETDEV_REGISTER
    addr->uint8[6] ^= netdev->type;
    addr->uint8[7] ^= netdev->index;
#else
    /* we should only get here with gnrc_netif_single */
    (void)netdev;
#endif

    eui64_set_local(addr);
    eui64_clear_group(addr);
}
