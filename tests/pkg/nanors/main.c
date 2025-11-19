/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "architecture.h"
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
    /* nanors work buffer */
    static uint8_t rs_buf[reed_solomon_bufsize(DATA_SHARDS, RECOVERY_SHARDS)];

    /* pointer to shards */
    uint8_t *shards[DATA_SHARDS + RECOVERY_SHARDS];
    /* map of missing shards */
    uint8_t marks[DATA_SHARDS + RECOVERY_SHARDS];
    memset(marks, 0, sizeof(marks));

    /* generate random data */
    random_bytes(data, sizeof(data_cmp));
    memcpy(data_cmp, data, sizeof(data_cmp));

    /* set up shard pointers */
    for (unsigned i = 0; i < ARRAY_SIZE(shards); ++i) {
        shards[i] = &data[i * SHARD_SIZE];
    }

    puts("START");
    rs_t *rs = reed_solomon_new_static(rs_buf, sizeof(rs_buf), DATA_SHARDS, RECOVERY_SHARDS);
    if (!rs) {
        puts("failed to init codec");
        return -1;
    }

    printf("total: %" PRIuSIZE " shards (%" PRIuSIZE " bytes)\n",
           ARRAY_SIZE(marks), sizeof(data));

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

    if (memcmp(data, data_cmp, sizeof(data_cmp))) {
        puts("FAILED");
    } else {
        puts("SUCCESS");
    }

    return 0;
}
