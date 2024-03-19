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

/*
    [
        null,
        123456,
        -123456,
        {
            -1: h'31323334353600',
            0: false,
            -999: "hello world"
        },
        true
    ]
*/
static const uint8_t expected[] = {
    0x85, 0xF6, 0x1A, 0x00, 0x01, 0xE2, 0x40, 0x3A, 0x00, 0x01, 0xE2, 0x3F,
    0xA3, 0x20, 0x47, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00, 0x00, 0xF4,
    0x39, 0x03, 0xE6, 0x6B, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F,
    0x72, 0x6C, 0x64, 0xF5};
static uint8_t buf[sizeof(expected)];

static void test_qcbor_encode(void)
{
    QCBOREncodeContext encode_context;
    UsefulBufC encoded_cbor;
    QCBORError error;
    uint8_t bytes[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00};

    QCBOREncode_Init(&encode_context, UsefulBuf_FROM_BYTE_ARRAY(buf));

    QCBOREncode_OpenArray(&encode_context);
    QCBOREncode_AddNULL(&encode_context);
    QCBOREncode_AddUInt64(&encode_context, 123456);
    QCBOREncode_AddInt64(&encode_context, -123456);
    QCBOREncode_OpenMap(&encode_context);
    QCBOREncode_AddBytesToMapN(&encode_context,
                               -1,
                               ((UsefulBufC){bytes, sizeof(bytes)}));
    QCBOREncode_AddBoolToMapN(&encode_context, 0, false);
    QCBOREncode_AddTextToMapN(&encode_context,
                              -999,
                              UsefulBuf_FROM_SZ_LITERAL("hello world"));
    QCBOREncode_CloseMap(&encode_context);
    QCBOREncode_AddBool(&encode_context, true);
    QCBOREncode_CloseArray(&encode_context);
    error = QCBOREncode_Finish(&encode_context, &encoded_cbor);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);

    TEST_ASSERT_EQUAL_INT(0, memcmp(encoded_cbor.ptr,
                                    expected, sizeof(expected)));
}

static void test_qcbor_decode(void)
{
    QCBORDecodeContext decode_context;
    QCBORItem cbor_item;
    QCBORError error;

    QCBORDecode_Init(&decode_context,
                     UsefulBuf_FROM_BYTE_ARRAY_LITERAL(expected),
                     QCBOR_DECODE_MODE_NORMAL);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(QCBOR_TYPE_ARRAY, cbor_item.uDataType);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(QCBOR_TYPE_NULL, cbor_item.uDataType);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(123456, cbor_item.val.uint64);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(-123456, cbor_item.val.int64);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(QCBOR_TYPE_MAP, cbor_item.uDataType);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(QCBOR_TYPE_BYTE_STRING, cbor_item.uDataType);
    TEST_ASSERT_EQUAL_INT(-1, cbor_item.label.int64);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(QCBOR_TYPE_FALSE, cbor_item.uDataType);
    TEST_ASSERT_EQUAL_INT(0, cbor_item.label.uint64);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(QCBOR_TYPE_TEXT_STRING, cbor_item.uDataType);
    TEST_ASSERT_EQUAL_INT(-999, cbor_item.label.int64);
    error = QCBORDecode_GetNext(&decode_context, &cbor_item);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(QCBOR_TYPE_TRUE, cbor_item.uDataType);
    error = QCBORDecode_Finish(&decode_context);
    TEST_ASSERT_EQUAL_INT(QCBOR_SUCCESS, error);
}

Test *tests_qcbor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_qcbor_encode),
        new_TestFixture(test_qcbor_decode),
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
