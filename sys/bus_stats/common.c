/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Bus statistic common functions
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdio.h>
#include <string.h>

#include "bitarithm.h"
#include "bus_stats.h"
#include "ztimer.h"

void bus_stats_histogram_add(uint32_t dest[BUS_STATS_HISTOGRAM_SIZE], uint16_t value)
{
    if (value == 0) {
        /* bitarith_msb() may not be defined for value == 0*/
        dest[0]++;
    }
    else if (value >= (1 << (BUS_STATS_HISTOGRAM_SIZE - 1))) {
        /* put values exceeding range in highest bucket */
        dest[BUS_STATS_HISTOGRAM_SIZE - 1]++;
    }
    else {
        /* normal case */
        dest[bitarithm_msb(value)]++;
    }
}


static void print_histogram(uint32_t data[BUS_STATS_HISTOGRAM_SIZE], const char *unit)
{
    unsigned lower = 0;
    unsigned upper = 1;
    printf("    %04u %s - %04u %s: %" PRIu32 "\n", lower, unit, upper, unit, data[0]);

    for (unsigned idx = 1; idx < BUS_STATS_HISTOGRAM_SIZE - 1; idx++) {
        lower = 1 << idx;
        upper = (lower << 1) - 1;
        printf("    %04u %s - %04u %s: %" PRIu32 "\n",
               lower, unit, upper, unit, data[idx]);

    }
    printf("    %04u %s -    ∞ %s: %" PRIu32 "\n",
           1 << (BUS_STATS_HISTOGRAM_SIZE - 1), unit, unit,
           data[BUS_STATS_HISTOGRAM_SIZE - 1]);
}

void bus_stats_print(bus_stats_t *stats)
{
    uint32_t now_us = ztimer_now(ZTIMER_USEC);
    uint32_t duration_us = now_us - stats->last_clear_us;
    uint64_t usage_ppm = 1000000LLU * stats->time_of_use_us / duration_us;
    uint32_t usage_int = usage_ppm / 10000U;
    uint32_t usage_frac = usage_ppm % 10000U;
    printf("  Access time: %" PRIu32 " µs\n", stats->time_of_use_us);
    printf("  Usage: %" PRIu32 ".%04" PRIu32 " %%\n",
           usage_int, usage_frac);
    printf("  Last access: %" PRIu32 " µs ago\n", now_us - stats->last_access_us);
    printf("  Transfers:\n");
    print_histogram(stats->transfer_size_histogram, "B");
    printf("  Transactions:\n");
    print_histogram(stats->transaction_size_histogram, "B");
    printf("  Time of transactions:\n");
    print_histogram(stats->time_us_of_transactions_histogram, "µs");
    printf("  Time between transactions:\n");
    print_histogram(stats->time_ms_between_transactions_histogram, "ms");
}
