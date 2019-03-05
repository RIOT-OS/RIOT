/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <string.h>
#include <stdio.h>
#include "embUnit.h"

#include "tests-suit.h"
#include "suit/cbor.h"

/**
 * A nice example CBOR-encoded manifest:
 *
 * [
 *  2,
 *  None,
 *  b'\xc4>92\xee.\x15\x10',
 *  1527518505,
 *  [[1, b'T}\rtm:Z\x92\x96bH\x81\xaf\xd9@{'],
 *   [2, b'\xbc\xc9\t\x84\xfe}V+\xb4\xc9\xa2O&\xa3\xa9\xcd'],
 *   [3, b'T\x8a\xb6\xc8\xdfXS-\xbe\x0e\n\x1d\xc9\r\xcf\xeb']],
 *  None,
 *  None,
 *  None,
 *  None,
 *  [
 *   [1],
 *   71480,
 *   b'\x00\01',
 *   [[1, 'coap://[ff02::1]/fw/test']],
 *   [1],
 *   {1: b'\x0e\x06=\xb2\xf3T\xe5\xabG\xcf\xd6\x9a\xbbe\x17e\xcd\x9d\xf3\\6S\\\xf5'
 *       b'2\xde\xfe\x9c\xe36\x8an'},
 *   None
 *  ]
 * ]
 */
static unsigned char test_suit_cbor_full[] = {
    0x8a, 0x02, 0xf6, 0x48, 0xc4, 0x3e, 0x39, 0x32, 0xee, 0x2e, 0x15, 0x10,
    0x1a, 0x5b, 0x0c, 0x15, 0x29, 0x83, 0x82, 0x01, 0x50, 0x54, 0x7d, 0x0d,
    0x74, 0x6d, 0x3a, 0x5a, 0x92, 0x96, 0x62, 0x48, 0x81, 0xaf, 0xd9, 0x40,
    0x7b, 0x82, 0x02, 0x50, 0xbc, 0xc9, 0x09, 0x84, 0xfe, 0x7d, 0x56, 0x2b,
    0xb4, 0xc9, 0xa2, 0x4f, 0x26, 0xa3, 0xa9, 0xcd, 0x82, 0x03, 0x50, 0x54,
    0x8a, 0xb6, 0xc8, 0xdf, 0x58, 0x53, 0x2d, 0xbe, 0x0e, 0x0a, 0x1d, 0xc9,
    0x0d, 0xcf, 0xeb, 0xf6, 0xf6, 0xf6, 0xf6, 0x87, 0x81, 0x01, 0x1a, 0x00,
    0x01, 0x17, 0x38, 0x42, 0x00, 0x01, 0x81, 0x82, 0x01, 0x78, 0x18, 0x63,
    0x6f, 0x61, 0x70, 0x3a, 0x2f, 0x2f, 0x5b, 0x66, 0x66, 0x30, 0x32, 0x3a,
    0x3a, 0x31, 0x5d, 0x2f, 0x66, 0x77, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x81,
    0x01, 0xa1, 0x01, 0x58, 0x20, 0x0e, 0x06, 0x3d, 0xb2, 0xf3, 0x54, 0xe5,
    0xab, 0x47, 0xcf, 0xd6, 0x9a, 0xbb, 0x65, 0x17, 0x65, 0xcd, 0x9d, 0xf3,
    0x5c, 0x36, 0x53, 0x5c, 0xf5, 0x32, 0xde, 0xfe, 0x9c, 0xe3, 0x36, 0x8a,
    0x6e, 0xf6
};

/* Parameters from the conditions belonging to the manifest above */
static const uint8_t cond[][16] = {
    { 0x54, 0x7d, 0x0d, 0x74, 0x6d, 0x3a, 0x5a, 0x92, 0x96, 0x62, 0x48, 0x81,
      0xaf, 0xd9, 0x40, 0x7b },
    { 0xbc, 0xc9, 0x09, 0x84, 0xfe, 0x7d, 0x56, 0x2b, 0xb4, 0xc9, 0xa2, 0x4f,
      0x26, 0xa3, 0xa9, 0xcd },
    { 0x54, 0x8a, 0xb6, 0xc8, 0xdf, 0x58, 0x53, 0x2d, 0xbe, 0x0e, 0x0a, 0x1d,
      0xc9, 0x0d, 0xcf, 0xeb },
};

static const char test_uri[] = "coap://[ff02::1]/fw/test";
static const uint8_t test_storid[] = { 0x00, 0x01 };

static char uri_buf[128];

static void _test_cond(const suit_cbor_manifest_t *manifest,
                       size_t idx, int expected_type)
{
    int cond_type = 0;
    uint8_t param[16];

    TEST_ASSERT(suit_cbor_get_condition_type(manifest, idx, &cond_type) >= 0);
    TEST_ASSERT_EQUAL_INT(expected_type, cond_type);

    size_t param_len = sizeof(param);
    size_t res = suit_cbor_get_condition_parameter(manifest, idx, param,
                                                   &param_len);

    TEST_ASSERT_EQUAL_INT(param_len, res);
    TEST_ASSERT_EQUAL_INT(0, memcmp(param, cond[idx], res));
}

void test_suit_cbor_parse(void)
{
    suit_cbor_manifest_t manifest;

    uint8_t digest[64];
    size_t dlen = sizeof(digest);

    TEST_ASSERT_EQUAL_INT(suit_cbor_parse(&manifest, test_suit_cbor_full,
                                          sizeof(test_suit_cbor_full)), 0);

    uint32_t version = 0;
    TEST_ASSERT_EQUAL_INT(suit_cbor_get_version(&manifest, &version), SUIT_OK);
    TEST_ASSERT_EQUAL_INT(version, 2);

    uint32_t seq_no;
    TEST_ASSERT_EQUAL_INT(suit_cbor_get_seq_no(&manifest, &seq_no), SUIT_OK);
    TEST_ASSERT_EQUAL_INT(seq_no, 1527518505);

    /* Test conditional types */
    _test_cond(&manifest, 0, 1);    /* First condition, type 1 */
    _test_cond(&manifest, 1, 2);    /* Second condition, type 2 */
    _test_cond(&manifest, 2, 3);    /* Third condition, type 3 */


    /* Nonexisting condition */
    TEST_ASSERT_EQUAL_INT(SUIT_ERR_COND,
                          suit_cbor_get_condition_type(&manifest, 3,
                                                       NULL));

    uint32_t size;
    TEST_ASSERT_EQUAL_INT(suit_cbor_payload_get_size(&manifest, &size), SUIT_OK);
    TEST_ASSERT_EQUAL_INT(size, 71480);

    suit_cbor_digest_t algo = -1;
    TEST_ASSERT_EQUAL_INT(suit_cbor_payload_get_digestalgo(&manifest, &algo),
                          SUIT_OK);
    TEST_ASSERT_EQUAL_INT(algo, SUIT_DIGEST_SHA256);

    int res = suit_cbor_payload_get_digest(&manifest, SUIT_DIGEST_TYPE_RAW,
                                           digest, &dlen);
    TEST_ASSERT_EQUAL_INT(1, res);

    res = suit_cbor_payload_get_digest(&manifest, SUIT_DIGEST_TYPE_CIPHERTEXT,
                                       digest, &dlen);
    TEST_ASSERT_EQUAL_INT(0, res);

    TEST_ASSERT_EQUAL_INT(suit_cbor_get_url(&manifest, uri_buf, sizeof(uri_buf)), 24);
    TEST_ASSERT_EQUAL_STRING((char *)test_uri, (char *)uri_buf);

    uint8_t storid[4];
    size_t storid_len = sizeof(storid);
    TEST_ASSERT_EQUAL_INT(suit_cbor_payload_get_storid(&manifest, storid,
                                                       &storid_len), SUIT_OK);
    TEST_ASSERT_EQUAL_INT(storid_len, 2);
    TEST_ASSERT_EQUAL_INT(memcmp(test_storid, storid, storid_len), 0);
}

Test *tests_suit_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_suit_cbor_parse),
    };

    EMB_UNIT_TESTCALLER(suit_cbor_tests, NULL, NULL, fixtures);

    return (Test *)&suit_cbor_tests;
}

void tests_suit(void)
{
    TESTS_RUN(tests_suit_tests());
}
