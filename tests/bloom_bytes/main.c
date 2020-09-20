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

#include "xtimer.h"
#include "fmt.h"

#include "hashes.h"
#include "bloom.h"
#include "random.h"
#include "bitfield.h"

#define BLOOM_BITS (1UL << 12)
#define BLOOM_HASHF (8)
#define lenB 512
#define lenA (10 * 1000)

#define MAGIC_A 0xafafafaf
#define MAGIC_B 0x0c0c0c0c

#define myseed 0x83d385c0 /* random number */

#define BUF_SIZE 50
static uint32_t buf[BUF_SIZE];
static bloom_t bloom;
BITFIELD(bf, BLOOM_BITS);
hashfp_t hashes[BLOOM_HASHF] = {
    (hashfp_t) fnv_hash, (hashfp_t) sax_hash, (hashfp_t) sdbm_hash,
    (hashfp_t) djb2_hash, (hashfp_t) kr_hash, (hashfp_t) dek_hash,
    (hashfp_t) rotating_hash, (hashfp_t) one_at_a_time_hash,
};

static void buf_fill(uint32_t *buf, int len)
{
    for (int k = 0; k < len; k++) {
        buf[k] = random_uint32();
    }
}

int main(void)
{
    bloom_init(&bloom, BLOOM_BITS, bf, hashes, BLOOM_HASHF);

    printf("Testing Bloom filter.\n\n");
    printf("m: %" PRIu32 " k: %" PRIu32 "\n\n", (uint32_t) bloom.m,
           (uint32_t) bloom.k);

    random_init(myseed);

    unsigned long t1 = xtimer_now_usec();

    for (int i = 0; i < lenB; i++) {
        buf_fill(buf, BUF_SIZE);
        buf[0] = MAGIC_B;
        bloom_add(&bloom,
                  (uint8_t *) buf,
                  BUF_SIZE * sizeof(uint32_t) / sizeof(uint8_t));
    }

    unsigned long t2 = xtimer_now_usec();
    printf("adding %d elements took %" PRIu32 "ms\n", lenB,
           (uint32_t) (t2 - t1) / 1000);

    int in = 0;
    int not_in = 0;

    unsigned long t3 = xtimer_now_usec();

    for (int i = 0; i < lenA; i++) {
        buf_fill(buf, BUF_SIZE);
        buf[0] = MAGIC_A;

        if (bloom_check(&bloom,
                        (uint8_t *) buf,
                        BUF_SIZE * sizeof(uint32_t) / sizeof(uint8_t))) {
            in++;
        }
        else {
            not_in++;
        }
    }

    unsigned long t4 = xtimer_now_usec();
    printf("checking %d elements took %" PRIu32 "ms\n", lenA,
           (uint32_t) (t4 - t3) / 1000);

    printf("\n");
    printf("%d elements probably in the filter.\n", in);
    printf("%d elements not in the filter.\n", not_in);
    double false_positive_rate = (double) in / (double) lenA;
    /* Use 'fmt/print_float' to work on all platforms (atmega)
     * Stdout should be flushed before to prevent garbled output. */
#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    fflush(stdout);
#endif
    print_float(false_positive_rate, 6);
    puts(" false positive rate.");

    bloom_del(&bloom);
    printf("\nAll done!\n");
    return 0;
}
