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
#include "timex.h"

#include "benchmark.h"

void benchmark_print_time(uint32_t time, unsigned long runs, const char *name)
{
    uint32_t full = (time / runs);
    uint32_t div  = (time - (full * runs)) / (runs / 1000);

    uint32_t per_sec = (uint32_t)(((uint64_t)US_PER_SEC * runs) / time);

    printf("%25s: %9" PRIu32 "us"
           "  ---  %2" PRIu32 ".%03" PRIu32 "us per call"
           "  ---  %9" PRIu32 " calls per sec\n",
           name, time, full, div, per_sec);
}
