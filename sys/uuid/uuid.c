/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_uuid
 * @{
 *
 * @file
 * @brief       UUID module implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "assert.h"
#include "periph/cpuid.h"

#include "uuid.h"

static uint8_t lastused = 1;

void uuid_get(void *buf, size_t len)
{
    uuid_base(buf, len);

    ((uint8_t *)buf)[0] ^= lastused++;
}

void uuid_custom(void *buf, size_t len, int gen)
{
    uuid_base(buf, len);

    for (size_t i = 0; i < sizeof(gen); i++) {
        ((uint8_t *)buf)[i % len] ^= ((gen >> (i * 8)) & 0xff);
    }
}

void uuid_base(void *buf, size_t len)
{
    assert(buf && (len > 0));

    memset(buf, UUID_BACKUP_SEED, len);

#if CPUID_LEN
    uint8_t *out = (uint8_t *)buf;
    uint8_t cid[CPUID_LEN];

    cpuid_get(cid);
    for (size_t i = 0; i < CPUID_LEN; i++) {
        out[i % len] ^= cid[i];
    }
#endif
}
