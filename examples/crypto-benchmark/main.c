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

#include "crypto/3des.h"
#include "crypto/aes.h"
#include "crypto/twofish.h"

#include "hashes/sha256.h"

#include "periph/rtt.h"
#include "periph/hwcrypto.h"

/**
 * @brief   Length of the benchmark in seconds.
 */
#ifndef CRYPTO_BENCHMARK_DURATION
#define CRYPTO_BENCHMARK_DURATION (10U)
#endif

/**
 * @brief   Dummy data buffer (don't care about contents). Must be aligned for
 *          performance reasons.
 */
static uint8_t __attribute__ ((aligned)) data[32];

/**
 * @brief   Dummy key (don't care about actual key). Must be aligned for
 *          performance reasons.
 */
static uint8_t __attribute__ ((aligned)) key[32];

/**
 * @brief   Global flag to stop benchmark that is set when alarm is triggered.
 */
static volatile bool stop;

/**
 * @brief   Alarm callback to stop benchmark.
 */
static void benchmark_cb(void *arg)
{
    stop = true;
}

/**
 * @brief   Prepare RTT benchmark.
 */
static void benchmark_start(uint32_t duration)
{
    uint32_t timeout;

    /* clear stop flag */
    stop = false;

    /* schedule alarm */
    timeout = rtt_get_counter() + (duration * RTT_FREQUENCY);
    rtt_set_alarm(timeout, benchmark_cb, 0);
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
    hwcrypto_cipher_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_cipher_init(&context, HWCRYPTO_AES128, key, 16);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_cipher_encrypt(&context, data, data, 16);
        ops++;
    }

    return ops;
}
#endif

#ifdef HAVE_HWCRYPTO_AES256
static uint32_t benchmark_hwcrypto_aes256(uint32_t duration)
{
    hwcrypto_cipher_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_cipher_init(&context, HWCRYPTO_AES256, key, 32);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_cipher_encrypt(&context, data, data, 16);
        ops++;
    }

    return ops;
}
#endif

#ifdef HAVE_HWCRYPTO_SHA1
static uint32_t benchmark_hwcrypto_sha1(uint32_t duration)
{
    hwcrypto_hash_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_hash_init(&context, HWCRYPTO_SHA1);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_hash_update(&context, data, 20);
        ops++;
    }

    return ops;
}
#endif

#ifdef HAVE_HWCRYPTO_SHA256
static uint32_t benchmark_hwcrypto_sha256(uint32_t duration)
{
    hwcrypto_hash_context_t context;
    uint32_t ops = 0;

    /* prepare cipher */
    hwcrypto_init();
    hwcrypto_hash_init(&context, HWCRYPTO_SHA256);

    /* prepare alarm */
    benchmark_start(duration);

    /* run the benchmark until alarm triggers */
    while (!stop) {
        hwcrypto_hash_update(&context, data, 32);
        ops++;
    }

    return ops;
}
#endif

int main(void)
{
    uint32_t duration = CRYPTO_BENCHMARK_DURATION;
    uint32_t ops;

    /* prepare ticker */
    rtt_init();

    /* run tests */
    puts("Starting benchmarks.\n");

    ops = benchmark_crypto_3des(duration);
    printf("CRYPTO 3DES: %lu ops in %lu seconds.\n", ops, duration);

    ops = benchmark_crypto_aes128(duration);
    printf("CRYPTO AES128: %lu ops in %lu seconds.\n", ops, duration);

    ops = benchmark_crypto_twofish(duration);
    printf("CRYPTO TWOFISH: %lu ops in %lu seconds.\n", ops, duration);

    ops = benchmark_crypto_sha256(duration);
    printf("CRYPTO SHA256: %lu ops in %lu seconds.\n", ops, duration);

#ifdef HAVE_HWCRYPTO_AES128
    ops = benchmark_hwcrypto_aes128(duration);
    printf("HWCRYPTO AES128: %lu ops in %lu seconds.\n", ops, duration);
#endif

#ifdef HAVE_HWCRYPTO_AES256
    ops = benchmark_hwcrypto_aes256(duration);
    printf("HWCRYPTO AES256: %lu ops in %lu seconds.\n", ops, duration);
#endif

#ifdef HAVE_HWCRYPTO_SHA1
    ops = benchmark_hwcrypto_sha1(duration);
    printf("HWCRYPTO SHA1: %lu ops in %lu seconds.\n", ops, duration);
#endif

#ifdef HAVE_HWCRYPTO_SHA256
    ops = benchmark_hwcrypto_sha256(duration);
    printf("HWCRYPTO SHA256: %lu ops in %lu seconds.\n", ops, duration);
#endif

    puts("Benchmarks done!\n");
}
