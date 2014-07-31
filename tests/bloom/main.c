/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Bloom filter test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "hashes.h"
#include "bloom.h"

#include "sets.h"

int main(void)
{
    struct bloom_t *bloom = bloom_new(1 << 7, 6, fnv_hash, sax_hash, sdbm_hash,
                                      djb2_hash, kr_hash, dek_hash, rotating_hash, one_at_a_time_hash);

    printf("Testing Bloom filter.\n\n");
    printf("m: %zd\nk: %zd\n\n", bloom->m, bloom->k);

    for (int i = 0; i < lenB; i++) {
        bloom_add(bloom, (const uint8_t *) B[i], strlen(B[i]));
        printf("Added \"%s\"\n", B[i]);
    }

    int in = 0;
    int not_in = 0;

    for (int i = 0; i < lenA; i++) {
        if (bloom_check(bloom, (const uint8_t *) A[i], strlen(A[i]))) {
            in++;
        }
        else {
            not_in++;
        }
    }

    printf("\n");
    printf("%d elements probably in the filter.\n", in);
    printf("%d elements not in the filter.\n", not_in);
    double false_positive_rate = (double) in / (double) lenA;
    printf("%f false positive rate.\n", false_positive_rate);

    bloom_del(bloom);
    printf("\nAll done!\n");
    return 0;
}
