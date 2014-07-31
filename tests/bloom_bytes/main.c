/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include <inttypes.h>

#include "hwtimer.h"

#include "hashes.h"
#include "bloom.h"
#include "random.h"

#define lenB 512
#define lenA (10 * 1000)

#define MAGIC_A 0xafafafaf
#define MAGIC_B 0x0c0c0c0c

#define myseed 0x83d385c0 /* random number */

#define BUF_SIZE 50
static uint32_t buf[BUF_SIZE];

static void buf_fill(uint32_t *buf, int len)
{
    for (int k = 0; k < len; k++) {
        buf[k] = genrand_uint32();
    }
}

int main(void)
{
    hwtimer_init();

    struct bloom_t *bloom = bloom_new(1 << 12, 8, fnv_hash, sax_hash, sdbm_hash,
                                      djb2_hash, kr_hash, dek_hash, rotating_hash, one_at_a_time_hash);

    printf("Testing Bloom filter.\n\n");
    printf("m: %" PRIu32 " k: %" PRIu32 "\n\n", (uint32_t) bloom->m,
           (uint32_t) bloom->k);

    genrand_init(myseed);

    unsigned long t1 = hwtimer_now();

    for (int i = 0; i < lenB; i++) {
        buf_fill(buf, BUF_SIZE);
        buf[0] = MAGIC_B;
        bloom_add(bloom,
                  (uint8_t *) buf,
                  BUF_SIZE * sizeof(uint32_t) / sizeof(uint8_t));
    }

    unsigned long t2 = hwtimer_now();
    printf("adding %d elements took %" PRIu32 "ms\n", lenB,
           (uint32_t) HWTIMER_TICKS_TO_US(t2 - t1) / 1000);

    int in = 0;
    int not_in = 0;

    unsigned long t3 = hwtimer_now();

    for (int i = 0; i < lenA; i++) {
        buf_fill(buf, BUF_SIZE);
        buf[0] = MAGIC_A;

        if (bloom_check(bloom,
                        (uint8_t *) buf,
                        BUF_SIZE * sizeof(uint32_t) / sizeof(uint8_t))) {
            in++;
        }
        else {
            not_in++;
        }
    }

    unsigned long t4 = hwtimer_now();
    printf("checking %d elements took %" PRIu32 "ms\n", lenA,
           (uint32_t) HWTIMER_TICKS_TO_US(t4 - t3) / 1000);

    printf("\n");
    printf("%d elements probably in the filter.\n", in);
    printf("%d elements not in the filter.\n", not_in);
    double false_positive_rate = (double) in / (double) lenA;
    printf("%f false positive rate.\n", false_positive_rate);

    bloom_del(bloom);
    printf("\nAll done!\n");
    return 0;
}
