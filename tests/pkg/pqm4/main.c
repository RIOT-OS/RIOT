/*
 * SPDX-FileCopyrightText: 2026 Mert Cakir <mert-cakir@outlook.com>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: CC0-1.0
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   pqm4 ML-DSA-44 self-test: keygen, sign, verify round-trip.
 *
 * @author  Mert Cakir <mert-cakir@outlook.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "api.h"
#include "fmt.h"

static uint8_t pk[CRYPTO_PUBLICKEYBYTES];
static uint8_t sk[CRYPTO_SECRETKEYBYTES];

static const uint8_t msg[] = "pqm4 self-test message";
#define MSG_LEN (sizeof(msg) - 1)

static uint8_t sm[MSG_LEN + CRYPTO_BYTES];
static uint8_t m_out[MSG_LEN + CRYPTO_BYTES];

int main(void)
{
    puts("pqm4 ML-DSA-44 self-test");
    puts("========================");

    printf("  pk size: %u B\n", CRYPTO_PUBLICKEYBYTES);
    printf("  sk size: %u B\n", CRYPTO_SECRETKEYBYTES);
    printf("  sig size: %u B\n", CRYPTO_BYTES);

    /* keygen */
    puts("[1/3] crypto_sign_keypair ...");
    if (crypto_sign_keypair(pk, sk) != 0) {
        puts("  FAIL: keygen returned non-zero");
        return 1;
    }
    puts("  OK");

    /* sign */
    puts("[2/3] crypto_sign ...");
    size_t smlen = 0;
    if (crypto_sign(sm, &smlen, msg, MSG_LEN, sk) != 0) {
        puts("  FAIL: sign returned non-zero");
        return 1;
    }
    printf("  OK (sm = %u B)\n", (unsigned)smlen);

    /* verify */
    puts("[3/3] crypto_sign_open ...");
    size_t mlen = 0;
    if (crypto_sign_open(m_out, &mlen, sm, smlen, pk) != 0) {
        puts("  FAIL: verify returned non-zero");
        return 1;
    }
    if (mlen != MSG_LEN || memcmp(m_out, msg, MSG_LEN) != 0) {
        puts("  FAIL: recovered message mismatch");
        return 1;
    }
    puts("  OK (message recovered and verified)");

    puts("");
    puts("ALL TESTS PASSED");
    return 0;
}
