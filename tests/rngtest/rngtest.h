/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu> 
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RNGTEST_H
#define RNGTEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>


#define BLOCKCOUNT 1000
#define UNIT(BLOCKS, STAT) ((1000000.0 * BLOCKS) / STAT) / (1024.0 * 1024.0);

#define TS(NAME) gettimeofday(&NAME, 0)

#define TS_DIFF(START, STOP)\
   ((STOP.tv_sec - START.tv_sec) * 1000000UL + STOP.tv_usec - START.tv_usec)


/* Size of a FIPS test buffer, do not change this */
#define FIPS_RNG_BUFFER_SIZE 2500 
#define BLOCKSIZE FIPS_RNG_BUFFER_SIZE*8

/* Min-Max stat */
struct rng_stat {
    uint32_t max;               /* Highest value seen */
    uint32_t min;               /* Lowest value seen */
    uint32_t num_samples;       /* Number of samples */
    uint32_t sum;               /* Sum of all samples */
};

/* RNG Buffer */
unsigned char rng_buffer[FIPS_RNG_BUFFER_SIZE];

/* Context for running FIPS tests */
typedef struct fips_ctx {
        int poker[16], runs[12];
        int ones, rlength, current_bit, last_bit, longrun;
        unsigned int last32;
} fips_ctx_t;

/* Return values for fips_run_rng_test */
#define FIPS_RNG_MONOBIT        0x0001 /* FIPS 140-2 2001-10-10 monobit */
#define FIPS_RNG_POKER          0x0002 /* FIPS 140-2 2001-10-10 poker */
#define FIPS_RNG_RUNS           0x0004 /* FIPS 140-2 2001-10-10 runs */
#define FIPS_RNG_LONGRUN        0x0008 /* FIPS 140-2 2001-10-10 long run */
#define FIPS_RNG_CONTINUOUS_RUN 0x0010 /* FIPS 140-2 continuous run */

/* Names for the FIPS tests, and bitmask */
#define N_FIPS_TESTS 5

/* Logic and contexts */
static fips_ctx_t fipsctx;                      /* Context for the FIPS tests */

const char *fips_test_names[N_FIPS_TESTS] = {
    "FIPS 140-2(2001-10-10) Monobit",
    "FIPS 140-2(2001-10-10) Poker",
    "FIPS 140-2(2001-10-10) Runs",
    "FIPS 140-2(2001-10-10) Long run",
    "FIPS 140-2(2001-10-10) Continuous run"
};

const unsigned int fips_test_mask[N_FIPS_TESTS] = {
    FIPS_RNG_MONOBIT, FIPS_RNG_POKER, FIPS_RNG_RUNS,
    FIPS_RNG_LONGRUN, FIPS_RNG_CONTINUOUS_RUN
};

int fips_run_rng_test(fips_ctx_t *ctx, const void *buf);

/* Statistics */
struct {
    /* simple counters */
    uint32_t bad_fips_blocks;                   /* Blocks reproved by FIPS 140-2 */
    uint32_t good_fips_blocks;                  /* Blocks approved by FIPS 140-2 */
    uint32_t fips_failures[N_FIPS_TESTS];       /* Breakdown of block failures per FIPS test */
    uint32_t bytes_received;                    /* Bytes read from input */
    uint32_t bytes_sent;                        /* Bytes sent to output */

    /* performance timers */
    struct rng_stat source_blockfill;           /* Block-receive time */
    struct rng_stat fips_blockfill;             /* FIPS run time */
    struct rng_stat sink_blockfill;             /* Block-send time */
    struct timeval progstart;                   /* Program start time */
} rng_stats;

#ifdef __cplusplus
}
#endif

#endif
