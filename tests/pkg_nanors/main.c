/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for Reed-Solomon codec
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "kernel_defines.h"
#include "random.h"
#include "rs.h"

#define DATA_SHARDS     20
#define RECOVERY_SHARDS 10
#define SHARD_SIZE      64

typedef reed_solomon rs_t;

int main(void)
{
    /* data + recovery */
    static uint8_t data[(DATA_SHARDS + RECOVERY_SHARDS) * SHARD_SIZE];
    /* copy of data for comparison */
    static uint8_t data_cmp[DATA_SHARDS * SHARD_SIZE];
    /* pointer to shards */
    static uint8_t *shards[DATA_SHARDS + RECOVERY_SHARDS];
    /* map of missing shards */
    static uint8_t marks[DATA_SHARDS + RECOVERY_SHARDS];

    /* generate random data */
    random_bytes(data, sizeof(data_cmp));
    memcpy(data_cmp, data, sizeof(data_cmp));

    /* set up shard pointers */
    for (unsigned i = 0; i < ARRAY_SIZE(shards); ++i) {
        shards[i] = &data[i * SHARD_SIZE];
    }

    puts("START");
    reed_solomon_init();
    rs_t *rs = reed_solomon_new(DATA_SHARDS, RECOVERY_SHARDS);
    if (!rs) {
        puts("failed to init codec");
        return -1;
    }

    printf("total: %u shards (%u bytes)\n",
           (unsigned)ARRAY_SIZE(marks), (unsigned)sizeof(data));

    /* generate parity shards */
    reed_solomon_encode(rs, shards, ARRAY_SIZE(shards), SHARD_SIZE);

    /* delete up to N shards */
    for (unsigned i = 0; i < RECOVERY_SHARDS; i++) {
        unsigned at = random_uint32_range(0, ARRAY_SIZE(shards));
        printf("clear shard %u (%u byte)\n", at, SHARD_SIZE);
        memset(shards[at], 0, SHARD_SIZE);
        marks[at] = 1;
    }

    puts("reconstruct…");
    if (reed_solomon_reconstruct(rs, shards, marks,
                                 ARRAY_SIZE(shards), SHARD_SIZE)) {
        puts("failed.");
    } else {
        puts("done.");
    }
    reed_solomon_release(rs);

    if (memcmp(data, data_cmp, sizeof(data_cmp))) {
        puts("FAILED");
    } else {
        puts("SUCCESS");
    }

    return 0;
}
