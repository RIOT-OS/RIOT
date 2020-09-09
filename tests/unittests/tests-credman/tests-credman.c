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

Test *tests_credman_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_credman_add),
        new_TestFixture(test_credman_get),
        new_TestFixture(test_credman_delete),
        new_TestFixture(test_credman_delete_random_order),
        new_TestFixture(test_credman_add_delete_all),
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
