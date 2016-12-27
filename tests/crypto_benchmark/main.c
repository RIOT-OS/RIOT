/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Crypto benchmark for comparing software and hardware
 *              implementations.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdbool.h>

#include "crypto/3des.h"
#include "crypto/aes.h"
#include "crypto/twofish.h"

#include "hashes/md5.h"
#include "hashes/sha1.h"
#include "hashes/sha256.h"

#include "periph/hwcrypto.h"

#include "xtimer.h"

/**
 * @brief   Dummy data buffer (don't care about data).
 */
static uint8_t data[32] __attribute__((aligned));

/**
 * @brief   Dummy key (don't care about actual key).
 */
static uint8_t key[32] __attribute__((aligned));

/**
 * @brief   Benchmark timer
 */
static xtimer_t timer;

/**
 * @brief   Global flag to stop a running benchmark. Is set when alarm is
 *          triggered.
 */
static volatile bool stop;

/**
 * @brief   Alarm callback to stop benchmark.
 */
static void benchmark_cb(void *arg)
{
    (void) arg;

    xtimer_remove(&timer);

    stop = true;
}

/**
 * @brief   Prepare benchmark timer.
 */
static void benchmark_start(uint32_t duration)
{
    stop = false;

    timer.callback = benchmark_cb;
    timer.arg = NULL;

    xtimer_set(&timer, duration * SEC_IN_USEC);
}

static uint32_t benchmark_crypto_3des(uint32_t duration)
{
    cipher_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    tripledes_init(&context, key, 16);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        tripledes_encrypt(&context, data, data);
        ops++;
    }

    return ops;
}

static uint32_t benchmark_crypto_aes128(uint32_t duration)
{
    cipher_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    aes_init(&context, key, 16);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        aes_encrypt(&context, data, data);
        ops++;
    }

    return ops;
}

static uint32_t benchmark_crypto_twofish(uint32_t duration)
{
    cipher_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    twofish_init(&context, key, 16);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        twofish_encrypt(&context, data, data);
        ops++;
    }

    return ops;
}

static uint32_t benchmark_crypto_md5(uint32_t duration)
{
    md5_ctx_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    md5_init(&context);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        md5_update(&context, data, 16);
        ops++;
    }

    return ops;
}

static uint32_t benchmark_crypto_sha1(uint32_t duration)
{
    sha1_context context;
    uint32_t ops = 0;

    /* prepare cipher */
    sha1_init(&context);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        sha1_update(&context, data, 20);
        ops++;
    }

    return ops;
}

static uint32_t benchmark_crypto_sha256(uint32_t duration)
{
    sha256_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    sha256_init(&context);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        sha256_update(&context, data, 32);
        ops++;
    }

    return ops;
}

#ifdef HAVE_HWCRYPTO_AES128
static uint32_t benchmark_hwcrypto_aes128(uint32_t duration)
{
    hwcrypto_cipher_aes128_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_cipher_init((hwcrypto_cipher_context_t *) &context, HWCRYPTO_AES128, HWCRYPTO_MODE_ECB);
    hwcrypto_cipher_set((hwcrypto_cipher_context_t *) &context, HWCRYPTO_OPT_KEY, key, 16);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_cipher_encrypt((hwcrypto_cipher_context_t *) &context, data, data, 16);
        ops++;
    }

    return ops;
}
#endif

#ifdef HAVE_HWCRYPTO_AES256
static uint32_t benchmark_hwcrypto_aes256(uint32_t duration)
{
    hwcrypto_cipher_aes256_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_cipher_init((hwcrypto_cipher_context_t *) &context, HWCRYPTO_AES256, HWCRYPTO_MODE_ECB);
    hwcrypto_cipher_set((hwcrypto_cipher_context_t *) &context, HWCRYPTO_OPT_KEY, key, 32);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_cipher_encrypt((hwcrypto_cipher_context_t *) &context, data, data, 16);
        ops++;
    }

    return ops;
}
#endif

#ifdef HAVE_HWCRYPTO_SHA1
static uint32_t benchmark_hwcrypto_sha1(uint32_t duration)
{
    hwcrypto_hash_sha1_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_hash_init((hwcrypto_hash_context_t *) &context, HWCRYPTO_SHA1);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_hash_update((hwcrypto_hash_context_t *) &context, data, 20);
        ops++;
    }

    return ops;
}
#endif

#ifdef HAVE_HWCRYPTO_SHA256
static uint32_t benchmark_hwcrypto_sha256(uint32_t duration)
{
    hwcrypto_hash_sha256_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_hash_init((hwcrypto_hash_context_t *) &context, HWCRYPTO_SHA256);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_hash_update((hwcrypto_hash_context_t *) &context, data, 32);
        ops++;
    }

    return ops;
}
#endif

int main(void)
{
    uint32_t duration = 10;
    uint32_t ops;

    /* run tests */
    puts("Starting benchmarks.");

    ops = benchmark_crypto_3des(duration);
    printf("CRYPTO 3DES: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec.\n", ops, duration, ops / duration);

    ops = benchmark_crypto_aes128(duration);
    printf("CRYPTO AES128: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);

    ops = benchmark_crypto_twofish(duration);
    printf("CRYPTO TWOFISH: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);

    ops = benchmark_crypto_md5(duration);
    printf("CRYPTO MD5: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);

    ops = benchmark_crypto_sha1(duration);
    printf("CRYPTO SHA1: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);

    ops = benchmark_crypto_sha256(duration);
    printf("CRYPTO SHA256: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);

#ifdef HAVE_HWCRYPTO_AES128
    ops = benchmark_hwcrypto_aes128(duration);
    printf("HWCRYPTO AES128: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);
#endif

#ifdef HAVE_HWCRYPTO_AES256
    ops = benchmark_hwcrypto_aes256(duration);
    printf("HWCRYPTO AES256: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);
#endif

#ifdef HAVE_HWCRYPTO_SHA1
    ops = benchmark_hwcrypto_sha1(duration);
    printf("HWCRYPTO SHA1: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);
#endif

#ifdef HAVE_HWCRYPTO_SHA256
    ops = benchmark_hwcrypto_sha256(duration);
    printf("HWCRYPTO SHA256: completed %" PRIu32 " ops in %" PRIu32 " seconds, %" PRIu32 " ops/sec\n", ops, duration, ops / duration);
#endif

    puts("Benchmarks done!");
}
