/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for pkg libcose
 *
 * @author     Koen Zandberg <koen@bergzand.net>
 */

/* Number of nodes allocated for cn-cbor */
#define MAX_NUMBER_BLOCKS 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cose.h"
#include "cose/sign.h"
#include "cose/crypto.h"
#include "embUnit.h"
#include "memarray.h"
#include "random.h"

/* Example payload */
static char payload[] = "Input string";
/* Key ID's */
static const char kid[] = "peter@riot-os.org";
static const char kid2[] = "schmerzl@riot-os.org";
/* Key pairs */
static unsigned char pk[COSE_CRYPTO_SIGN_ED25519_PUBLICKEYBYTES];
static unsigned char sk[COSE_CRYPTO_SIGN_ED25519_SECRETKEYBYTES];
static unsigned char pk2[COSE_CRYPTO_SIGN_ED25519_PUBLICKEYBYTES];
static unsigned char sk2[COSE_CRYPTO_SIGN_ED25519_SECRETKEYBYTES];
/* COSE structs */
static cose_sign_enc_t sign;
static cose_sign_dec_t verify;
static cose_signature_t signature1, signature2;
static cose_key_t signer1, signer2;

/* COSE sign buffer */
static uint8_t buf[2048];
/* Signature Verification buffer */
static uint8_t vbuf[2048];

static void setUp(void)
{
    /* Initialize */
    random_init(0);
    /* Clear buffer */
    memset(buf, 0, sizeof(buf));
    memset(vbuf, 0, sizeof(vbuf));
}

/* Untagged 1 signer test */
static void test_libcose_01(void)
{
    /* Set up first signer */
    cose_key_init(&signer1);
    cose_key_set_keys(&signer1, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA, pk,
                      NULL, sk);
    cose_crypto_keypair_ed25519(&signer1);
    cose_key_set_kid(&signer1, (uint8_t *)kid, sizeof(kid) - 1);

    /* Initialize struct */
    cose_sign_init(&sign, COSE_FLAGS_UNTAGGED);
    cose_signature_init(&signature1);

    /* Add payload */
    cose_sign_set_payload(&sign, payload, sizeof(payload));

    /* First signer */
    cose_sign_add_signer(&sign, &signature1, &signer1);

    /* Encode COSE sign object */
    uint8_t *out = NULL;
    ssize_t encode_size = cose_sign_encode(&sign, buf, sizeof(buf), &out);
    TEST_ASSERT(encode_size > 0);
    /* Decode again */
    TEST_ASSERT_EQUAL_INT(0, cose_sign_decode(&verify, out, encode_size));

    /* Verify with signature slot 0 */
    cose_signature_dec_t vsignature;
    cose_sign_signature_iter_init(&vsignature);
    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));
    TEST_ASSERT_EQUAL_INT(0,
                          cose_sign_verify(&verify, &vsignature, &signer1, vbuf,
                                           sizeof(vbuf)));
    /* Modify payload */
    ((uint8_t *)(verify.payload))[0]++;
    /* Should fail due to modified payload */
    TEST_ASSERT_EQUAL_INT(-2,
                          cose_sign_verify(&verify, &vsignature, &signer1, vbuf,
                                           sizeof(vbuf)));
}

/* 2 signer test */
static void test_libcose_02(void)
{
    /* Set up first signer */
    cose_key_init(&signer1);
    cose_key_set_keys(&signer1, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA, pk,
                      NULL, sk);
    cose_crypto_keypair_ed25519(&signer1);
    cose_key_set_kid(&signer1, (uint8_t *)kid, sizeof(kid) - 1);

    /* Second signer */
    cose_key_init(&signer2);
    cose_key_set_keys(&signer2, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA, pk2,
                      NULL, sk2);
    cose_crypto_keypair_ed25519(&signer2);
    cose_key_set_kid(&signer2, (uint8_t *)kid2, sizeof(kid2) - 1);

    /* Initialize structs */
    cose_sign_init(&sign, 0);
    cose_signature_init(&signature1);
    cose_signature_init(&signature2);

    /* Add payload */
    cose_sign_set_payload(&sign, payload, sizeof(payload));

    /* Signers */
    cose_sign_add_signer(&sign, &signature1, &signer1);
    cose_sign_add_signer(&sign, &signature2, &signer2);

    /* Encode COSE sign object */
    uint8_t *out = NULL;
    size_t len = cose_sign_encode(&sign, buf, sizeof(buf), &out);

    TEST_ASSERT(len > 0);
    TEST_ASSERT_EQUAL_INT(0, cose_sign_decode(&verify, out, len));

    /* Test correct signature with correct signer */
    cose_signature_dec_t vsignature;
    cose_sign_signature_iter_init(&vsignature);
    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));
    TEST_ASSERT_EQUAL_INT(-2,
                          cose_sign_verify(&verify, &vsignature, &signer1, vbuf,
                                           sizeof(vbuf)));
    TEST_ASSERT_EQUAL_INT(0,
                          cose_sign_verify(&verify, &vsignature, &signer2, vbuf,
                                           sizeof(vbuf)));
    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));
    TEST_ASSERT_EQUAL_INT(0,
                          cose_sign_verify(&verify, &vsignature, &signer1, vbuf,
                                           sizeof(vbuf)));
    TEST_ASSERT_EQUAL_INT(-2,
                          cose_sign_verify(&verify, &vsignature, &signer2, vbuf,
                                           sizeof(vbuf)));

    /* Modify payload */
    ((uint8_t *)(verify.payload))[0]++;
    cose_sign_signature_iter_init(&vsignature);
    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));
    TEST_ASSERT_EQUAL_INT(-2,
                          cose_sign_verify(&verify, &vsignature, &signer1, vbuf,
                                           sizeof(vbuf)));
    TEST_ASSERT_EQUAL_INT(-2,
                          cose_sign_verify(&verify, &vsignature, &signer2, vbuf,
                                           sizeof(vbuf)));
    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));
    TEST_ASSERT_EQUAL_INT(-2,
                          cose_sign_verify(&verify, &vsignature, &signer1, vbuf,
                                           sizeof(vbuf)));
    TEST_ASSERT_EQUAL_INT(-2,
                          cose_sign_verify(&verify, &vsignature, &signer2, vbuf,
                                           sizeof(vbuf)));
}

Test *tests_libcose(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_libcose_01),
        new_TestFixture(test_libcose_02),
    };

    EMB_UNIT_TESTCALLER(libcose_tests, setUp, NULL, fixtures);
    return (Test *)&libcose_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_libcose());
    TESTS_END();
    return 0;
}
