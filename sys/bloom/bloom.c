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

#define SETBIT(a,n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a,n) (a[n/CHAR_BIT] &  (1<<(n%CHAR_BIT)))
#define ROUND(size) ((size + CHAR_BIT - 1) / CHAR_BIT)

struct bloom_t *bloom_new(size_t size, size_t num_hashes, ...)
{
    struct bloom_t *bloom;
    va_list hashes;
    size_t n;

    /* Allocate Bloom filter container */
    if (!(bloom = malloc(sizeof(struct bloom_t)))) {
        return NULL;
    }

    /* Allocate Bloom array */
    if (!(bloom->a = calloc(ROUND(size), sizeof(char)))) {
        free(bloom);
        return NULL;
    }

    /* Allocate Bloom filter hash function pointers */
    if (!(bloom->hash = (hashfp_t *)malloc(num_hashes * sizeof(hashfp_t)))) {
        free(bloom->a);
        free(bloom);
        return NULL;
    }

    /* Assign hash functions to pointers in the Bloom filter */
    va_start(hashes, num_hashes);

    for (n = 0; n < num_hashes; n++) {
        bloom->hash[n] = va_arg(hashes, hashfp_t);
    }

    va_end(hashes);

    /*
     * Record the number of hash functions (k) and the number of bytes
     * in the Bloom array (m).
     */
    bloom->k = num_hashes;
    bloom->m = size;

    return bloom;
}

void bloom_del(struct bloom_t *bloom)
{
    free(bloom->a);
    free(bloom->hash);
    free(bloom);
}

void bloom_add(struct bloom_t *bloom, const uint8_t *buf, size_t len)
{
    for (size_t n = 0; n < bloom->k; n++) {
        uint32_t hash = bloom->hash[n](buf, len);
        SETBIT(bloom->a, (hash % bloom->m));
    }
}

bool bloom_check(struct bloom_t *bloom, const uint8_t *buf, size_t len)
{
    for (size_t n = 0; n < bloom->k; n++) {
        uint32_t hash = bloom->hash[n](buf, len);

        if (!(GETBIT(bloom->a, (hash % bloom->m)))) {
            return false;
        }
    }

    return true; /* ? */
}
