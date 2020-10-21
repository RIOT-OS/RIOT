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

#include "periph/cpuid.h"

#include "luid.h"

#if CPUID_LEN
/* based on DJB2 Hash */
static void _luid_round(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len)
{
    uint32_t hash = 5381;

    for (size_t i = 0; i < in_len; i++) {
        hash = hash * 33 + in[i];
        out[i % out_len] ^= hash;
    }
}
#endif

void __attribute__((weak)) luid_base(void *buf, size_t len)
{
    memset(buf, LUID_BACKUP_SEED, len);

#if CPUID_LEN
    uint8_t cid[CPUID_LEN];
    cpuid_get(cid);

    _luid_round(cid, sizeof(cid), buf, len);
    _luid_round(buf, len, buf, len);
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
