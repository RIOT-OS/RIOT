/**
 * Bloom filter implementation
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @file
 * @author Jason Linehan <patientulysses@gmail.com>
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 */

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>

#include "bloom.h"
#include "bitfield.h"
#include "string.h"

#define ROUND(size) ((size + CHAR_BIT - 1) / CHAR_BIT)

void bloom_init(bloom_t *bloom, size_t size, uint8_t *bitfield, hashfp_t *hashes, int hashes_numof)
{
    bloom->m = size;
    bloom->a = bitfield;
    bloom->hash = hashes;
    bloom->k = hashes_numof;
}

void bloom_del(bloom_t *bloom)
{
    if (bloom->a) {
        memset(bloom->a, 0, ROUND(bloom->m));
    }
    bloom->a = NULL;
    bloom->m = 0;
    bloom->hash = NULL;
    bloom->k = 0;
}

void bloom_add(bloom_t *bloom, const uint8_t *buf, size_t len)
{
    for (size_t n = 0; n < bloom->k; n++) {
        uint32_t hash = bloom->hash[n](buf, len);
        bf_set(bloom->a, (hash % bloom->m));
    }
}

bool bloom_check(bloom_t *bloom, const uint8_t *buf, size_t len)
{
    for (size_t n = 0; n < bloom->k; n++) {
        uint32_t hash = bloom->hash[n](buf, len);

        if (!(bf_isset(bloom->a, (hash % bloom->m)))) {
            return false;
        }
    }

    return true; /* ? */
}
