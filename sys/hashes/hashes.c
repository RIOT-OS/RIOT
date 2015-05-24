/**
 * This file contains some simple hash function
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author      Jason Linehan <patientulysses@gmail.com>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#include "hashes.h"

uint32_t djb2_hash(const uint8_t *buf, size_t len)
{
    uint32_t hash = 5381;

    for (size_t i = 0; i < len; i++) {
        hash = hash * 33 + buf[i];
    }

    return hash;
}

uint32_t sdbm_hash(const uint8_t *buf, size_t len)
{
    uint32_t hash = 0;

    for (size_t i = 0; i < len; i++) {
        hash = buf[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

uint32_t kr_hash(const uint8_t *buf, size_t len)
{
    uint32_t hash = 0;

    for (size_t i = 0; i < len; i++) {
        hash += buf[i];
    }

    return hash;
}

uint32_t sax_hash(const uint8_t *buf, size_t len)
{
    uint32_t hash = 0;

    for (size_t i = 0; i < len; i++) {
        hash ^= (hash << 5) + (hash >> 2) + buf[i];
    }

    return hash;
}

uint32_t dek_hash(const uint8_t *buf, size_t len)
{
    uint32_t hash = 7919; /* prime */

    for (size_t i = 0; i < len; i++) {
        hash = (hash << 5) ^ (hash >> 27) ^ buf[i];
    }

    return hash;
}

uint32_t fnv_hash(const uint8_t *buf, size_t len)
{
    uint32_t FNV_PRIME = 0x811C9DC5;
    uint32_t hash = 0;

    for (size_t i = 0; i < len; i++) {
        hash *= FNV_PRIME;
        hash ^= buf[i];
    }

    return hash;
}

uint32_t rotating_hash(const uint8_t *buf, size_t len)
{
    uint32_t hash = 0;

    for (size_t i = 0; i < len; i++) {
        hash = (hash << 4) ^ (hash >> 28) ^ buf[i];
    }

    return hash;
}

uint32_t one_at_a_time_hash(const uint8_t *buf, size_t len)
{
    uint32_t hash = 786431; /* prime */

    for (size_t i = 0; i < len; i++) {
        hash += buf[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

uint16_t fletcher16(const uint8_t *data, size_t bytes)
{
    uint16_t sum1 = 0xff, sum2 = 0xff;

    while (bytes) {
        size_t tlen = bytes > 20 ? 20 : bytes;
        bytes -= tlen;
        do {
            sum2 += sum1 += *data++;
        } while (--tlen);
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);
    }
    /* Second reduction step to reduce sums to 8 bits */
    sum1 = (sum1 & 0xff) + (sum1 >> 8);
    sum2 = (sum2 & 0xff) + (sum2 >> 8);
    return (sum2 << 8) | sum1;
}

uint32_t fletcher32(const uint16_t *data, size_t words)
{
    uint32_t sum1 = 0xffff, sum2 = 0xffff;

    while (words) {
        unsigned tlen = words > 359 ? 359 : words;
        words -= tlen;
        do {
            sum2 += sum1 += *data++;
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return (sum2 << 16) | sum1;
}
