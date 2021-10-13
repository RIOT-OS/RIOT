/*
 * Copyright (C) 2019 Kaleb J. Himes, Daniele Lacamera
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       wolfSSL cryptographic library test
 *
 * @author      Kaleb J. Himes <kaleb@wolfssl.com>
 *              Daniele Lacamera <daniele@wolfssl.com>
 *
 * @}
 */

#include <stdio.h>
#include "xtimer.h"
#include "log.h"

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfcrypt/test/test.h>
#ifdef MODULE_WOLFCRYPT_BENCHMARK
#include <wolfcrypt/benchmark/benchmark.h>
#endif

int main(void)
{
    LOG_INFO("wolfSSL Crypto Test!\n");
    /* Wait to work around a failing tests
     * on platforms that don't have RTC synchronized
     */
    xtimer_sleep(1);
    wolfcrypt_test(NULL);
#ifdef MODULE_WOLFCRYPT_BENCHMARK
    LOG_INFO("wolfSSL Benchmark!\n");
    benchmark_test(NULL);
#else
    LOG_INFO("wolfSSL Benchmark disabled\n");
#endif
    return 0;
}
