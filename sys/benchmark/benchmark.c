/*
 * Copyright (C) 2017,2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_benchmark
 * @{
 *
 * @file
 * @brief       Utility functions for the benchmark module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "benchmark.h"
#include "test_utils/result_output.h"

void benchmark_print_time(uint32_t time, unsigned long runs, const char *name)
{
    uint64_t per_sec = ((uint64_t)1000000UL * runs) / time;

    turo_t ctx;
    turo_init(&ctx);
    turo_container_open(&ctx);
    turo_dict_open(&ctx);

    turo_dict_key(&ctx, name);
    turo_dict_open(&ctx);

    turo_dict_key(&ctx, "us");
    turo_u32(&ctx, time);

    turo_dict_key(&ctx, "us/call");
    turo_float(&ctx, (float)time/(float)runs);

    turo_dict_key(&ctx, "calls/s");
    turo_u64(&ctx, per_sec);

    turo_dict_close(&ctx);
    turo_container_close(&ctx, 0);
}
