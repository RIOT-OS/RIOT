/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Short NanoCBOR library test
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <nanocbor/nanocbor.h>
#include <string.h>
#include "embUnit.h"

static uint8_t buf[64];

static uint8_t expected[] = {
  0x9f, 0xf5, 0xf4, 0x1a, 0xff, 0xff, 0xff, 0xff, 0x3a, 0x7f, 0xff, 0xff,
  0xff, 0xa4, 0x08, 0x18, 0x1e, 0x38, 0x1d, 0x19, 0x01, 0xf4, 0x39, 0x01,
  0xf3, 0x75, 0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20,
  0x6c, 0x6f, 0x6e, 0x67, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0xfa,
  0x3e, 0xae, 0x14, 0x7b, 0x46, 0x62, 0x79, 0x74, 0x65, 0x7a, 0x00, 0xf6,
  0xff
};

void test_nanocbor_encode(void)
{
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, buf, sizeof(buf));

    nanocbor_fmt_array_indefinite(&enc);
    nanocbor_fmt_bool(&enc, true);
    nanocbor_fmt_bool(&enc, false);
    nanocbor_fmt_uint(&enc, UINT32_MAX);
    nanocbor_fmt_int(&enc, INT32_MIN);
    nanocbor_fmt_map(&enc, 4);
    nanocbor_fmt_uint(&enc, 8);
    nanocbor_fmt_int(&enc, 30);
    nanocbor_fmt_int(&enc, -30);
    nanocbor_fmt_int(&enc, 500);
    nanocbor_fmt_int(&enc, -500);
    nanocbor_put_tstr(&enc, "this is a long string");
    nanocbor_fmt_float(&enc, 0.34);
    nanocbor_put_bstr(&enc, (uint8_t*)"bytez", sizeof("bytez"));
    nanocbor_fmt_null(&enc);
    nanocbor_fmt_end_indefinite(&enc);

    size_t required = nanocbor_encoded_len(&enc);

    TEST_ASSERT_EQUAL_INT(required, sizeof(expected));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

Test *tests_nanocbor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nanocbor_encode),
    };
    EMB_UNIT_TESTCALLER(nanocbor_tests, NULL, NULL, fixtures);
    return (Test *)&nanocbor_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_nanocbor());
    TESTS_END();
    return 0;
}
