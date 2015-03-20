/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "embUnit/embUnit.h"

#include "crypto/edsign.h"
#include "tests-crypto.h"
#include "tests-crypto-edsign.h"

static int test_sign(const char *secret, const char *public, size_t mlen, const char *message,
                     const char *signature)
{
    uint8_t pub[EDSIGN_PUBLIC_KEY_SIZE];
    uint8_t msg[MAX_MSG_SIZE];
    uint8_t sig[EDSIGN_SIGNATURE_SIZE];

    edsign_sec_to_pub((uint8_t *)pub, (const uint8_t *)secret);

    if (memcmp(public, pub, sizeof(pub))) {
        return 0;
    }

    memcpy(msg, message, mlen);

    edsign_sign(sig, (const uint8_t *)pub, (const uint8_t *)secret, (const uint8_t *)msg, mlen);

    if (memcmp(signature, sig, sizeof(sig))) {
        return 0;
    }

    if (!edsign_verify(sig, pub, msg, mlen)) {
        return 0;
    }

    msg[0] ^= 1;

    if (edsign_verify(sig, pub, msg, mlen)) {
        return 0;
    }

    msg[0] ^= 1;

    sig[0] ^= 1;

    if (edsign_verify(sig, pub, msg, mlen)) {
        return 0;
    }

    sig[0] ^= 1;

    sig[32] ^= 1;

    if (edsign_verify(sig, pub, msg, mlen)) {
        return 0;
    }

    sig[32] ^= 1;

    return 1;
}

static void test_crypto_edsign_sign(void)
{
    TEST_ASSERT(test_sign(TV1_SECRET, TV1_PUBLIC, TV1_MLEN, TV1_MESSAGE, TV1_SIGNATURE));
    TEST_ASSERT(test_sign(TV2_SECRET, TV2_PUBLIC, TV2_MLEN, TV2_MESSAGE, TV2_SIGNATURE));
    TEST_ASSERT(test_sign(TV3_SECRET, TV3_PUBLIC, TV3_MLEN, TV3_MESSAGE, TV3_SIGNATURE));
    TEST_ASSERT(test_sign(TV4_SECRET, TV4_PUBLIC, TV4_MLEN, TV4_MESSAGE, TV4_SIGNATURE));
}

Test *tests_crypto_edsign_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_edsign_sign),
    };

    EMB_UNIT_TESTCALLER(crypto_edsign_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_edsign_tests;
}
