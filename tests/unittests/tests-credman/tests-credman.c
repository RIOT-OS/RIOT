/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include "embUnit.h"
#include "tests-credman.h"
#include "credentials.h"

#include "net/credman.h"

#define CREDMAN_TEST_TAG (1)

/* $ openssl ecparam -name secp256r1 -genkey -outform der -out key.der */
static const uint8_t key_pair_der[] = {
    0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0x04,
    0x43, 0x81, 0x68, 0xa8, 0x63, 0x9e, 0xf7, 0xe3,
    0x0d, 0x40, 0x5c, 0xf1, 0xea, 0xc9, 0x90, 0xf6,
    0x5f, 0x63, 0x86, 0x29, 0xca, 0x20, 0x4f, 0x9f,
    0x34, 0xb1, 0x9d, 0xa5, 0x17, 0x7b, 0x92, 0xa0,
    0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x03, 0x01, 0x07, 0xa1, 0x44, 0x03, 0x42, 0x00,
    0x04, 0x95, 0x38, 0x6c, 0x0e, 0x45, 0xd3, 0x15,
    0x22, 0xad, 0x29, 0xd5, 0x24, 0xd8, 0x6b, 0x29,
    0xf7, 0x5c, 0x7b, 0x71, 0x89, 0xac, 0xa2, 0x86,
    0xb5, 0x9b, 0xdc, 0x00, 0x28, 0xa4, 0xae, 0xd8,
    0x9f, 0x8b, 0xe8, 0x63, 0xab, 0xd2, 0x30, 0x3c,
    0x06, 0xcd, 0xec, 0x8a, 0xf7, 0xb9, 0xf8, 0xb4,
    0x46, 0xca, 0x01, 0x55, 0x03, 0xe1, 0xbe, 0x99,
    0xf7, 0x13, 0x13, 0x78, 0xbd, 0x40, 0xf9, 0xd4,
    0x9c
};

/* $ openssl pkcs8 -topk8 -inform DER -outform DER -in key.der -out key_pkcs8.der -nocrypt */
static const uint8_t key_pair_pkcs8_der[] = {
    0x30, 0x81, 0x87, 0x02, 0x01, 0x00, 0x30, 0x13,
    0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02,
    0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x03, 0x01, 0x07, 0x04, 0x6d, 0x30, 0x6b, 0x02,
    0x01, 0x01, 0x04, 0x20, 0x04, 0x43, 0x81, 0x68,
    0xa8, 0x63, 0x9e, 0xf7, 0xe3, 0x0d, 0x40, 0x5c,
    0xf1, 0xea, 0xc9, 0x90, 0xf6, 0x5f, 0x63, 0x86,
    0x29, 0xca, 0x20, 0x4f, 0x9f, 0x34, 0xb1, 0x9d,
    0xa5, 0x17, 0x7b, 0x92, 0xa1, 0x44, 0x03, 0x42,
    0x00, 0x04, 0x95, 0x38, 0x6c, 0x0e, 0x45, 0xd3,
    0x15, 0x22, 0xad, 0x29, 0xd5, 0x24, 0xd8, 0x6b,
    0x29, 0xf7, 0x5c, 0x7b, 0x71, 0x89, 0xac, 0xa2,
    0x86, 0xb5, 0x9b, 0xdc, 0x00, 0x28, 0xa4, 0xae,
    0xd8, 0x9f, 0x8b, 0xe8, 0x63, 0xab, 0xd2, 0x30,
    0x3c, 0x06, 0xcd, 0xec, 0x8a, 0xf7, 0xb9, 0xf8,
    0xb4, 0x46, 0xca, 0x01, 0x55, 0x03, 0xe1, 0xbe,
    0x99, 0xf7, 0x13, 0x13, 0x78, 0xbd, 0x40, 0xf9,
    0xd4, 0x9c
};

/* $ openssl ec -in key.der -inform DER -pubout -outform DER -out pub.der */
static const uint8_t public_key_der[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86,
    0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
    0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00, 0x04, 0x95, 0x38, 0x6c, 0x0e, 0x45,
    0xd3, 0x15, 0x22, 0xad, 0x29, 0xd5, 0x24, 0xd8,
    0x6b, 0x29, 0xf7, 0x5c, 0x7b, 0x71, 0x89, 0xac,
    0xa2, 0x86, 0xb5, 0x9b, 0xdc, 0x00, 0x28, 0xa4,
    0xae, 0xd8, 0x9f, 0x8b, 0xe8, 0x63, 0xab, 0xd2,
    0x30, 0x3c, 0x06, 0xcd, 0xec, 0x8a, 0xf7, 0xb9,
    0xf8, 0xb4, 0x46, 0xca, 0x01, 0x55, 0x03, 0xe1,
    0xbe, 0x99, 0xf7, 0x13, 0x13, 0x78, 0xbd, 0x40,
    0xf9, 0xd4, 0x9c
};

/* $ openssl ec -in key.der -inform DER -text */
static const uint8_t private_key[] = {
    0x04, 0x43, 0x81, 0x68, 0xa8, 0x63, 0x9e, 0xf7,
    0xe3, 0x0d, 0x40, 0x5c, 0xf1, 0xea, 0xc9, 0x90,
    0xf6, 0x5f, 0x63, 0x86, 0x29, 0xca, 0x20, 0x4f,
    0x9f, 0x34, 0xb1, 0x9d, 0xa5, 0x17, 0x7b, 0x92
};

static const uint8_t public_key_x[] = {
    0x95, 0x38, 0x6c, 0x0e, 0x45, 0xd3, 0x15, 0x22,
    0xad, 0x29, 0xd5, 0x24, 0xd8, 0x6b, 0x29, 0xf7,
    0x5c, 0x7b, 0x71, 0x89, 0xac, 0xa2, 0x86, 0xb5,
    0x9b, 0xdc, 0x00, 0x28, 0xa4, 0xae, 0xd8, 0x9f
};

static const uint8_t public_key_y[] = {
    0x8b, 0xe8, 0x63, 0xab, 0xd2, 0x30, 0x3c, 0x06,
    0xcd, 0xec, 0x8a, 0xf7, 0xb9, 0xf8, 0xb4, 0x46,
    0xca, 0x01, 0x55, 0x03, 0xe1, 0xbe, 0x99, 0xf7,
    0x13, 0x13, 0x78, 0xbd, 0x40, 0xf9, 0xd4, 0x9c
};

static int _compare_credentials(const credman_credential_t *a,
                                const credman_credential_t *b)
{
    if ((a->tag == b->tag) && (a->type == b->type)) {
        return 0;
    }
    return -1;
}

static void set_up(void)
{
    /* reset credential pool before every test */
    credman_reset();
}

static void test_credman_add(void)
{
    int ret;
    unsigned exp_count = 0;

    psk_params_t exp_psk_params = {
        .id =  {
            .s = (void *)"RIOTer",
            .len = sizeof("RIOTer") - 1,
        },
        .key = {
            .s = (void *)"LGPLisyourfriend",
            .len = sizeof("LGPLisyourfriend") - 1,
        },
    };

    credman_credential_t credential = {
        .tag = CREDMAN_TEST_TAG,
        .type = CREDMAN_TYPE_PSK,
        .params = {
            .psk = exp_psk_params,
        },
    };

    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* add one credential */
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&credential));
    TEST_ASSERT_EQUAL_INT(++exp_count, credman_get_used_count());

    /* add duplicate credential */
    ret = credman_add(&credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_EXIST, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* add invalid credential params */
    memset(&credential.params.psk, 0, sizeof(psk_params_t));
    ret = credman_add(&credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_INVALID, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* fill the pool */
    memcpy(&credential.params.psk, &exp_psk_params, sizeof(psk_params_t));
    while (credman_get_used_count() < CONFIG_CREDMAN_MAX_CREDENTIALS) {
        /* increase tag number so that it is not recognized as duplicate */
        credential.tag++;
        TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&credential));
        TEST_ASSERT_EQUAL_INT(++exp_count, credman_get_used_count());
    }

    /* add to full pool */
    credential.tag++;
    ret = credman_add(&credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_NO_SPACE, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());
}

static void test_credman_get(void)
{
    int ret;
    credman_credential_t out_credential;
    credman_credential_t in_credential = {
        .tag = CREDMAN_TEST_TAG,
        .type = CREDMAN_TYPE_ECDSA,
        .params = {
            .ecdsa = {
                .private_key = ecdsa_priv_key,
                .public_key = { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
                .client_keys = NULL,
                .client_keys_size = 0,
            },
        },
    };

    /* get non-existing credential */
    ret = credman_get(&out_credential, in_credential.tag, in_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_NOT_FOUND, ret);

    ret = credman_add(&in_credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, ret);

    ret = credman_get(&out_credential, in_credential.tag, in_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, ret);
    TEST_ASSERT(!_compare_credentials(&in_credential, &out_credential));
}

static void test_credman_delete(void)
{
    int ret;
    unsigned exp_count = 0;
    credman_credential_t out_credential;
    credman_credential_t in_credential = {
        .tag = CREDMAN_TEST_TAG,
        .type = CREDMAN_TYPE_ECDSA,
        .params = {
            .ecdsa = {
                .private_key = ecdsa_priv_key,
                .public_key = { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
                .client_keys = NULL,
                .client_keys_size = 0,
            },
        },
    };

    /* delete non-existing credential */
    credman_delete(in_credential.tag, in_credential.type);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* add a credential */
    ret = credman_add(&in_credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, ret);
    TEST_ASSERT_EQUAL_INT(++exp_count, credman_get_used_count());

    /* delete a credential from credential pool */
    credman_delete(in_credential.tag, in_credential.type);
    TEST_ASSERT_EQUAL_INT(--exp_count, credman_get_used_count());

    /* get the deleted credential */
    ret = credman_get(&out_credential, in_credential.tag, in_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_NOT_FOUND, ret);

    /* delete a deleted credential */
    credman_delete(in_credential.tag, in_credential.type);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());
}

static void test_credman_delete_random_order(void)
{
    credman_tag_t tag1 = CREDMAN_TEST_TAG;
    credman_tag_t tag2 = CREDMAN_TEST_TAG + 1;

    credman_credential_t out_credential;
    credman_credential_t in_credential = {
        .tag = tag1,
        .type = CREDMAN_TYPE_ECDSA,
        .params = {
            .ecdsa = {
                .private_key = ecdsa_priv_key,
                .public_key = { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
                .client_keys = NULL,
                .client_keys_size = 0,
            },
        },
    };
    TEST_ASSERT_EQUAL_INT(0, credman_get_used_count());

    /* fill the credential pool, assume CONFIG_CREDMAN_MAX_CREDENTIALS is 2 */
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&in_credential));
    in_credential.tag = tag2;
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&in_credential));
    TEST_ASSERT_EQUAL_INT(2, credman_get_used_count());

    /* delete the first credential */
    credman_delete(tag1, in_credential.type);
    TEST_ASSERT_EQUAL_INT(1, credman_get_used_count());

    /* get the second credential */
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_get(&out_credential, tag2, in_credential.type));
    TEST_ASSERT(!_compare_credentials(&in_credential, &out_credential));
}

static void test_credman_add_delete_all(void)
{
    credman_tag_t tag1 = CREDMAN_TEST_TAG;
    credman_tag_t tag2 = CREDMAN_TEST_TAG + 1;

    credman_credential_t in_credential = {
        .tag = tag1,
        .type = CREDMAN_TYPE_ECDSA,
        .params = {
            .ecdsa = {
                .private_key = ecdsa_priv_key,
                .public_key = { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
                .client_keys = NULL,
                .client_keys_size = 0,
            },
        },
    };

    /* add credentials */
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&in_credential));
    in_credential.tag = tag2;
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&in_credential));
    TEST_ASSERT_EQUAL_INT(2, credman_get_used_count());

    /* delete starting from first added credential */
    credman_delete(tag1, in_credential.type);
    credman_delete(tag2, in_credential.type);
    TEST_ASSERT_EQUAL_INT(0, credman_get_used_count());

    /* re-add the credentials after deletion */
    in_credential.tag = tag1;
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&in_credential));
    in_credential.tag = tag2;
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add(&in_credential));
    TEST_ASSERT_EQUAL_INT(2, credman_get_used_count());
}

static void test_credman_load_public_key_from_buffer(void)
{
    ecdsa_public_key_t pub;

    int res = credman_load_public_key(public_key_der, sizeof(public_key_der), &pub);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, res);
    TEST_ASSERT_EQUAL_INT(memcmp(public_key_x, pub.x, sizeof(public_key_x)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(public_key_y, pub.y, sizeof(public_key_y)), 0);
}

static void test_credman_load_private_key_from_buffer(void)
{
    credman_credential_t cred;

    int res = credman_load_private_key(key_pair_pkcs8_der, sizeof(key_pair_pkcs8_der), &cred);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, res);
    TEST_ASSERT_EQUAL_INT(memcmp(private_key, cred.params.ecdsa.private_key, sizeof(private_key)),
                          0);
    TEST_ASSERT_EQUAL_INT(memcmp(public_key_x, cred.params.ecdsa.public_key.x,
                                 sizeof(public_key_x)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(public_key_y, cred.params.ecdsa.public_key.y,
                                 sizeof(public_key_y)), 0);
    TEST_ASSERT_EQUAL_INT(cred.type, CREDMAN_TYPE_ECDSA);
}

static void test_credman_load_private_ecc_key_from_buffer(void)
{
    credman_credential_t cred;

    int res = credman_load_private_ecc_key(key_pair_der, sizeof(key_pair_der), &cred);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, res);
    TEST_ASSERT_EQUAL_INT(memcmp(private_key, cred.params.ecdsa.private_key, sizeof(private_key)),
                          0);
    TEST_ASSERT_EQUAL_INT(memcmp(public_key_x, cred.params.ecdsa.public_key.x,
                                 sizeof(public_key_x)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(public_key_y, cred.params.ecdsa.public_key.y,
                                 sizeof(public_key_y)), 0);
    TEST_ASSERT_EQUAL_INT(cred.type, CREDMAN_TYPE_ECDSA);
}

Test *tests_credman_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_credman_add),
        new_TestFixture(test_credman_get),
        new_TestFixture(test_credman_delete),
        new_TestFixture(test_credman_delete_random_order),
        new_TestFixture(test_credman_add_delete_all),
        new_TestFixture(test_credman_load_public_key_from_buffer),
        new_TestFixture(test_credman_load_private_key_from_buffer),
        new_TestFixture(test_credman_load_private_ecc_key_from_buffer),
    };

    EMB_UNIT_TESTCALLER(credman_tests,
                        set_up,
                        NULL, fixtures);

    return (Test *)&credman_tests;
}

void tests_credman(void)
{
    TESTS_RUN(tests_credman_tests());
}
