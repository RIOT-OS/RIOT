/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu> 
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       This program tests the input stream in stdin for randomness, 
 *              using the tests defined by FIPS 140-1/140-2 2001-10-10. 
 *	        The Monobit, Poker, Runs, and Long Runs tests are implemented below.
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Henrique de Moraes Holschuh <hmh@debian.org> 
 * @}
 */

#include "rngtest.h"
#include "vtimer.h"
#include "periph/random.h"

#ifdef CPU_NATIVE
#include "native_internal.h"
#endif

#ifdef CPU_NRF51822
#include "nrf51.h"
#endif

static void fips_test_store(fips_ctx_t *ctx, unsigned int rng_data)
{
    ctx->poker[rng_data >> 4]++;
    ctx->poker[rng_data & 15]++;

    /* Note in the loop below rlength is always one less than the actual
       run length. This makes things easier. */
    for (int j = 7; j >= 0; j--) {
        ctx->ones += ctx->current_bit = ((rng_data >> j) & 1);

        if (ctx->current_bit != ctx->last_bit) {
            /* If runlength is 1-6 count it in correct bucket. 0's go in
               runs[0-5] 1's go in runs[6-11] hence the 6*current_bit below */
            if (ctx->rlength < 5) {
                ctx->runs[ctx->rlength + (6 * ctx->current_bit)]++;
            }
            else {
                ctx->runs[5 + (6 * ctx->current_bit)]++;
            }

            /* Check if we just failed longrun test */
            if (ctx->rlength >= 25) {
                ctx->longrun = 1;
            }

            ctx->rlength = 0;
            /* flip the current run type */
            ctx->last_bit = ctx->current_bit;
        }
        else {
            ctx->rlength++;
        }
    }
}

int fips_run_rng_test(fips_ctx_t *ctx, const void *buf)
{
    int i, j;
    int rng_test = 0;
    const unsigned char *rngdatabuf;

    if (!ctx) {
        return -1;
    }

    if (!buf) {
        return -1;
    }

    rngdatabuf = (const unsigned char *)buf;

    /* Prepare random buffer */
    for (i = 0; i < FIPS_RNG_BUFFER_SIZE; i += 4) {
        unsigned int new32 = rngdatabuf[i] | (rngdatabuf[i + 1] << 8) | (rngdatabuf[i + 2] << 16) |
                    (rngdatabuf[i + 3] << 24);

        if (new32 == ctx->last32) {
            rng_test |= FIPS_RNG_CONTINUOUS_RUN;
        }

        ctx->last32 = new32;
        fips_test_store(ctx, rngdatabuf[i]);
        fips_test_store(ctx, rngdatabuf[i + 1]);
        fips_test_store(ctx, rngdatabuf[i + 2]);
        fips_test_store(ctx, rngdatabuf[i + 3]);
    }

    /* Add in the last (possibly incomplete) run */
    if (ctx->rlength < 5) {
        ctx->runs[ctx->rlength + (6 * ctx->current_bit)]++;
    }
    else {
        ctx->runs[5 + (6 * ctx->current_bit)]++;

        if (ctx->rlength >= 25) {
            rng_test |= FIPS_RNG_LONGRUN;
        }
    }

    if (ctx->longrun) {
        rng_test |= FIPS_RNG_LONGRUN;
        ctx->longrun = 0;
    }

    /* Ones test */
    if ((ctx->ones >= 10275) || (ctx->ones <= 9725)) {
        rng_test |= FIPS_RNG_MONOBIT;
    }

    /* Poker calcs */
    for (i = 0, j = 0; i < 16; i++) {
        j += ctx->poker[i] * ctx->poker[i];
    }

    /* 16/5000*1563176-5000 = 2.1632  */
    /* 16/5000*1576928-5000 = 46.1696 */
    if ((j > 1576928) || (j < 1563176)) {
        rng_test |= FIPS_RNG_POKER;
    }

    if ((ctx->runs[0] < 2315) || (ctx->runs[0] > 2685) ||
        (ctx->runs[1] < 1114) || (ctx->runs[1] > 1386) ||
        (ctx->runs[2] < 527) || (ctx->runs[2] > 723) ||
        (ctx->runs[3] < 240) || (ctx->runs[3] > 384) ||
        (ctx->runs[4] < 103) || (ctx->runs[4] > 209) ||
        (ctx->runs[5] < 103) || (ctx->runs[5] > 209) ||
        (ctx->runs[6] < 2315) || (ctx->runs[6] > 2685) ||
        (ctx->runs[7] < 1114) || (ctx->runs[7] > 1386) ||
        (ctx->runs[8] < 527) || (ctx->runs[8] > 723) ||
        (ctx->runs[9] < 240) || (ctx->runs[9] > 384) ||
        (ctx->runs[10] < 103) || (ctx->runs[10] > 209) ||
        (ctx->runs[11] < 103) || (ctx->runs[11] > 209)) {
        rng_test |= FIPS_RNG_RUNS;
    }

    /* finally, clear out FIPS variables for start of next run */
    memset(ctx->poker, 0, sizeof(ctx->poker));
    memset(ctx->runs, 0, sizeof(ctx->runs));
    ctx->ones = 0;
    ctx->rlength = -1;
    ctx->current_bit = 0;

    return rng_test;
}


static void do_rng_fips_test_loop(void)
{
    int j;
    int fips_result;
    struct timeval start, stop, statdump;
    unsigned long int statruns, runs;
    uint32_t overflow = 0U;
    uint32_t value = 0U;

    struct rng_stat *stat;

    runs = statruns = 0;
    TS(statdump);

    for (;;) {
        TS(start);

	random_read(rng_buffer, sizeof(rng_buffer));

        /* Update timer stats (source_blockfill) */
        TS(stop);
        stat = (struct rng_stat *)&rng_stats.source_blockfill;
        overflow = stat->num_samples;
        value = TS_DIFF(start, stop);

        if ((stat->min == 0) || (value < stat->min)) {
            stat->min = value;
        }

        if (value > stat->max) {
            stat->max = value;
        }

        if (++stat->num_samples > overflow) {
            stat->sum += value;
        }
        else {
            stat->sum = value;
            stat->num_samples = 1;
        }

        /* Run FIPS rng tests */
        TS(start);
        fips_result = fips_run_rng_test(&fipsctx, &rng_buffer);

        /* Update timer stats (source_blockfill) */
        TS(stop);

        stat = (struct rng_stat *)&rng_stats.fips_blockfill;
        overflow = stat->num_samples;
        value = TS_DIFF(start, stop);

        if ((stat->min == 0) || (value < stat->min)) {
            stat->min = value;
        }

        if (value > stat->max) {
            stat->max = value;
        }

        if (++stat->num_samples > overflow) {
            stat->sum += value;
        }
        else {
            stat->sum = value;
            stat->num_samples = 1;
        }

        if (fips_result) {
            rng_stats.bad_fips_blocks++;

            for (j = 0; j < N_FIPS_TESTS; j++)
                if (fips_result & fips_test_mask[j]) {
                    rng_stats.fips_failures[j]++;
                }
        }
        else {
            rng_stats.good_fips_blocks++;
        }

        if (BLOCKCOUNT && (++runs >= BLOCKCOUNT)) {
            break;
        }
    }
}

int main(void)
{
    int j;
    unsigned char tempbuf[4];
    double bw_avg, bw_min, bw_max;
    struct timeval now;
    struct rng_stat *stat;

    /* wait a bit */
    vtimer_usleep(1000 * 1000);

    printf("Initializing Random Number Generator driver.\n");

#ifdef CPU_NATIVE
    _native_rng_mode = 1;
#endif

#ifdef CPU_NRF51822
    NRF_RNG->CONFIG = 1;
#endif

    random_init();

    /* Init data structures */
    memset(&rng_stats, 0, sizeof(rng_stats));
    TS(rng_stats.progstart);

    printf("Starting FIPS tests...\n");

    /* Bootstrap FIPS tests */
    fips_ctx_t *ctx = (fips_ctx_t *)&fipsctx;
    random_read(tempbuf, sizeof(tempbuf));


    /* Init statistic structures and bootstrap data */
    memset(ctx->poker, 0, sizeof(ctx->poker));
    memset(ctx->runs, 0, sizeof(ctx->runs));
    ctx->longrun = 0;
    ctx->ones = 0;
    ctx->rlength = -1;
    ctx->current_bit = 0;
    ctx->last_bit = 0;
    ctx->last32 = tempbuf[0] | (tempbuf[1] << 8) | (tempbuf[2] << 16) | (tempbuf[3] << 24);

    /* Main FIPS test loop is repeated $BLOCKCOUNT times */
    do_rng_fips_test_loop();

    /* Output FIPS 140-2 rng stats */
    printf("Bits received from input: %" PRIu32 "\n", rng_stats.bytes_received * 8);
    printf("FIPS 140-2 successes: %" PRIu32 "\n",rng_stats.good_fips_blocks);
    printf("FIPS 140-2 failures: %" PRIu32 "\n", rng_stats.bad_fips_blocks);

    for (j = 0; j < N_FIPS_TESTS; j++) {
        printf("%s: %" PRIu32 "\n", fips_test_names[j], rng_stats.fips_failures[j]);
    }


    /* Output input channel speed */
    bw_avg = bw_min = bw_max = 0.0;
    stat = &rng_stats.source_blockfill;

    if (stat->max > 0) {
        bw_min = UNIT(BLOCKSIZE, stat->max);
    }

    if (stat->min > 0) {
        bw_max = UNIT(BLOCKSIZE, stat->min);
    }

    if (stat->num_samples > 0) {
        bw_avg = UNIT(BLOCKSIZE * stat->num_samples, stat->sum);
    }

    printf("input channel speed min/avg/max: %.3lf/%.3lf/%.3lf\n", bw_min, bw_avg, bw_max);


    /* Output FIPS test speed */
    bw_avg = bw_min = bw_max = 0.0;
    stat = &rng_stats.fips_blockfill;

    if (stat->max > 0) {
        bw_min = UNIT(BLOCKSIZE, stat->max);
    }

    if (stat->min > 0) {
        bw_max = UNIT(BLOCKSIZE, stat->min);
    }

    if (stat->num_samples > 0) {
        bw_avg = UNIT(BLOCKSIZE * stat->num_samples, stat->sum);
    }

    printf("FIPS test speed min/avg/max: %.3lf/%.3lf/%.3lf\n", bw_min, bw_avg, bw_max);


    /* Output Programm run time */
    TS(now);
    printf("Program run time: %lu ms\n", TS_DIFF(rng_stats.progstart, now));

    return 0;
}
