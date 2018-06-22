/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Unit tests for pkg libcose
 *
 * @author     Koen Zandberg <koen@bergzand.net>
 */

/* Number of nodes allocated for cn-cbor */
#define MAX_NUMBER_BLOCKS 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cn-cbor/cn-cbor.h"
#include "cose.h"
#include "cose/crypto.h"
#include "embUnit.h"
#include "memarray.h"
#include "random.h"

#include "tests-libcose.h"

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
static unsigned char symmkey[COSE_CRYPTO_AEAD_CHACHA20POLY1305_KEYBYTES];
static uint8_t nonce[COSE_CRYPTO_AEAD_CHACHA20POLY1305_NONCEBYTES] = { 0 };
/* COSE structs */
static cose_sign_t sign, verify;
static cose_key_t signer, signer2, symm;
static cose_encrypt_t test_encrypt, test_decrypt;
/* COSE sign buffer */
static uint8_t buf[1024];
/*Signature Verification buffer */
static uint8_t vbuf[1024];

static cn_cbor block_storage_data[MAX_NUMBER_BLOCKS];
static memarray_t storage;

/* CN_CBOR calloc/free functions */
static void *cose_calloc(size_t count, size_t size, void *memblock);
static void cose_free(void *ptr, void *memblock);

/* CN_CBOR block allocator context struct*/
static cn_cbor_context ct =
{
    .calloc_func = cose_calloc,
    .free_func = cose_free,
    .context = &storage,
};

static void *cose_calloc(size_t count, size_t size, void *memblock)
{
    (void)count;
    void *block = memarray_alloc(memblock);
    if (block) {
        memset(block, 0, size);
    }
    return block;

}

static void cose_free(void *ptr, void *memblock)
{
    memarray_free(memblock, ptr);
}

static void setUp(void)
{
    /* Initialize */
    random_init(0);
    memarray_init(&storage, block_storage_data, sizeof(cn_cbor),
                  MAX_NUMBER_BLOCKS);
    /* Clear buffer */
    memset(buf, 0, sizeof(buf));
    memset(vbuf, 0, sizeof(vbuf));
}

static void test_libcose_01(void)
{
    /* Set up first signer */
    cose_key_init(&signer);
    cose_key_set_keys(&signer, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA,
                      pk, NULL, sk);
    cose_crypto_keypair_ed25519(&signer);
    cose_key_set_kid(&signer, (uint8_t *)kid, sizeof(kid) - 1);

    /* Initialize struct */
    cose_sign_init(&sign, COSE_FLAGS_UNTAGGED);
    cose_sign_init(&verify, 0);

    /* Add payload */
    cose_sign_set_payload(&sign, payload, sizeof(payload));

    /* First signer */
    cose_sign_add_signer(&sign, &signer);

    /* Encode COSE sign object */
    uint8_t *out = NULL;
    ssize_t encode_size = cose_sign_encode(&sign, buf, sizeof(buf), &out, &ct);
    TEST_ASSERT(encode_size > 0);
    /* Decode again */
    TEST_ASSERT_EQUAL_INT(cose_sign_decode(&verify, out, encode_size, &ct), 0);
    /* Verify with signature slot 0 */
    TEST_ASSERT_EQUAL_INT(cose_sign_verify(&verify, &signer,
                                           0, vbuf, sizeof(vbuf), &ct), 0);
}

static void test_libcose_02(void)
{
    /* Set up first signer */
    cose_key_init(&signer);
    cose_key_set_keys(&signer, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA,
                      pk, NULL, sk);
    cose_crypto_keypair_ed25519(&signer);
    cose_key_set_kid(&signer, (uint8_t *)kid, sizeof(kid) - 1);

    /* Second signer */
    cose_key_init(&signer2);
    cose_key_set_keys(&signer2, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA,
                      pk2, NULL, sk2);
    cose_crypto_keypair_ed25519(&signer2);
    cose_key_set_kid(&signer2, (uint8_t *)kid2, sizeof(kid2) - 1);

    /* Initialize struct */
    cose_sign_init(&sign, 0);
    cose_sign_init(&verify, 0);

    /* Add payload */
    cose_sign_set_payload(&sign, payload, sizeof(payload));

    /* Signers */
    cose_sign_add_signer(&sign, &signer);
    cose_sign_add_signer(&sign, &signer2);

    uint8_t *out = NULL;
    size_t len = cose_sign_encode(&sign, buf, sizeof(buf), &out, &ct);

    TEST_ASSERT(len > 0);
    TEST_ASSERT_EQUAL_INT(cose_sign_decode(&verify, out, len, &ct), 0);

    /* Test correct signature with correct signer */
    TEST_ASSERT_EQUAL_INT(cose_sign_verify(&verify, &signer, 0, vbuf,
                                           sizeof(vbuf), &ct), 0);
    TEST_ASSERT(cose_sign_verify(&verify, &signer, 1, vbuf,
                                 sizeof(vbuf), &ct) != 0);
    TEST_ASSERT(cose_sign_verify(&verify, &signer2, 0, vbuf,
                                 sizeof(vbuf), &ct) != 0);
    TEST_ASSERT_EQUAL_INT(cose_sign_verify(&verify, &signer2, 1, vbuf,
                                           sizeof(vbuf), &ct), 0);
}

static void test_libcose_03(void)
{
    cose_key_init(&symm);
    cose_encrypt_init(&test_encrypt);
    cose_encrypt_init(&test_decrypt);

    cose_crypto_keygen(symmkey, sizeof(symmkey), COSE_ALGO_CHACHA20POLY1305);
    cose_key_set_kid(&symm, (uint8_t *)kid, sizeof(kid) - 1);
    cose_key_set_keys(&symm, 0, COSE_ALGO_CHACHA20POLY1305,
                      NULL, NULL, symmkey);
    cose_encrypt_add_recipient(&test_encrypt, &symm);
    cose_encrypt_set_algo(&test_encrypt, COSE_ALGO_DIRECT);

    cose_encrypt_set_payload(&test_encrypt, payload, sizeof(payload) - 1);

    uint8_t *out = NULL;
    ssize_t len = cose_encrypt_encode(&test_encrypt, buf, sizeof(buf), nonce, &out, &ct);
    TEST_ASSERT(len > 0);
    TEST_ASSERT_EQUAL_INT(cose_encrypt_decode(&test_decrypt, out, len, &ct), 0);
    size_t plaintext_len = 0;
    int res = cose_encrypt_decrypt(&test_decrypt, &symm, 0, buf, sizeof(buf), vbuf,
                                   &plaintext_len, &ct);
    TEST_ASSERT_EQUAL_INT(res, 0);
    TEST_ASSERT_EQUAL_INT(plaintext_len, sizeof(payload) - 1);
}

Test *tests_libcose_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_libcose_01),
        new_TestFixture(test_libcose_02),
        new_TestFixture(test_libcose_03),
    };

    EMB_UNIT_TESTCALLER(libcose_tests, setUp, NULL, fixtures);
    return (Test *)&libcose_tests;
}

void tests_libcose(void)
{
    printf("Starting libcose test, performing multiple signature operations.\n");
    printf("This can take a while (up to 2 minutes on the samr21-xpro)\n");
    TESTS_RUN(tests_libcose_all());
}
