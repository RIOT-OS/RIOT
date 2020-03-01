/*
 * Copyright (C) 2018 Aydogan Ersoz
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
 * @brief       QCBOR library test
 *
 * @author      Aydogan Ersoz <aydoganersoz@protonmail.com>
 *
 * @}
 */

#include "qcbor.h"
#include <string.h>
#include "embUnit.h"

static uint8_t buf[4096];

/* [null, 123456, -123456, {-1: h'31323334353600', 0: false, -999: "hello world", 1111: -42.42}, true] */
static uint8_t expected[] = {
    0x85, 0xF6, 0x1A, 0x00, 0x01, 0xE2, 0x40, 0x3A, 0x00, 0x01, 0xE2, 0x3F,
    0xA4, 0x20, 0x47, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00, 0x00, 0xF4,
    0x39, 0x03, 0xE6, 0x6B, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F,
    0x72, 0x6C, 0x64, 0x19, 0x04, 0x57, 0xFB, 0xC0, 0x45, 0x35, 0xC2, 0x8F,
    0x5C, 0x28, 0xF6, 0xF5};

void test_qcbor_encode(void)
{
    QCBOREncodeContext encode_context;
    UsefulBufC encoded_cbor;
    QCBORError return_value;

    QCBOREncode_Init(&encode_context, UsefulBuf_FROM_BYTE_ARRAY(buf));

    QCBOREncode_AddNULL(&encode_context);
    QCBOREncode_AddUInt64(&encode_context, 123456);
    QCBOREncode_AddInt64(&encode_context, -123456);
    QCBOREncode_OpenMap(&encode_context);
    QCBOREncode_AddBytesToMapN(&encode_context, -1, ((UsefulBufC){(uint8_t[]){0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00}, 7}));
    QCBOREncode_AddBoolToMapN(&encode_context, 0, false);
    QCBOREncode_AddTextToMapN(&encode_context, -999, UsefulBuf_FROM_SZ_LITERAL("hello world"));
    QCBOREncode_AddBigFloatBigNumToMapN(&encode_context, 1111, ((UsefulBufC){(uint8_t[]){0xC2, 0x29, 0xAE, 0x14}, 4}), true, -5);
    QCBOREncode_CloseMap(&encode_context);
    QCBOREncode_AddBool(&encode_context, true);

    return_value = QCBOREncode_Finish(&encode_context, &encoded_cbor);

    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, return_value);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

Test *tests_qcbor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_qcbor_encode),
    };
    EMB_UNIT_TESTCALLER(qcbor_tests, NULL, NULL, fixtures);
    return (Test *)&qcbor_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_qcbor());
    TESTS_END();
    return 0;
}
