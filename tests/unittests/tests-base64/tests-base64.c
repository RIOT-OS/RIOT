/*
 * SPDX-FileCopyrightText: 2015 Martin Landsmann
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#define TEST_BASE64_SHOW_OUTPUT (0) /**< set if encoded/decoded string is displayed */

#if (TEST_BASE64_SHOW_OUTPUT == 1)
#include <stdio.h>
#endif
#include <stdint.h>
#include <string.h>
#include "embUnit.h"
#include "tests-base64.h"

#include "base64.h"

static void test_base64_01_encode_string(void)
{
    static const char data_in[] = "Hello RIOT this is a base64 test!\n"
                                  "This should work as intended.";

    static const char expected_encoding[] =
        "SGVsbG8gUklPVCB0aGlzIGlzIGEgYmFzZTY0IHR"
        "lc3QhClRoaXMgc2hvdWxkIHdvcmsgYXMgaW50ZW5kZWQu";

    size_t data_in_size = strlen(data_in);

    size_t base64_out_size = 0;
    char base64_out[sizeof(expected_encoding)];

    /*
    * @Note:
    * The first encoding attempt fails, but reveals the required out size.
    *
    * This size is a lower bound estimation,
    * thus it can require few more bytes then the actual used size for the output.
    */
    int ret = base64_encode(data_in, data_in_size, NULL, &base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_encode(data_in, data_in_size, base64_out, &base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    TEST_ASSERT_MESSAGE((0 == memcmp(expected_encoding, base64_out,
                                     base64_out_size)),
                        "encoding failed!(produced unexpected output)");

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 01 Encoded:");

    for (size_t i = 0; i < base64_out_size; ++i) {
        printf("%c", base64_out[i]);
    }

    printf("\nFrom:\n%s\n", data_in);
#endif
}

static void test_base64_02_decode_base64(void)
{
    static const char encoded_base64[] =
        "SGVsbG8gUklPVCB0aGlzIGlzIGEgYmFzZTY0IHRlc3Q"
        "hClRoaXMgc2hvdWxkIHdvcmsgYXMgaW50ZW5kZWQu";

    static const char expected_string[] = "Hello RIOT this is a base64 test!\n"
                                          "This should work as intended.";

    size_t base64_size = strlen(encoded_base64);

    size_t data_out_size = 0;
    unsigned char data_out[sizeof(expected_string)];

    int ret = base64_decode(encoded_base64, base64_size, NULL, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_decode(encoded_base64, base64_size, NULL, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT, ret);

    ret = base64_decode(encoded_base64, base64_size, data_out, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    TEST_ASSERT_MESSAGE((0 == memcmp(expected_string, data_out,
                                     data_out_size)),
                        "decoding failed!(produced unexpected output)");

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 02 Decoded:");

    for (size_t i = 0; i < data_out_size; ++i) {
        printf("%c", data_out[i]);
    }

    printf("\nFrom:\n%s\n", encoded_base64);
#endif
}

static void test_base64_03_single_character(void)
{
    size_t element_size = 1;
    static const char element[] = "1";

    unsigned char element_decode[3];
    size_t element_decode_size = sizeof(element_decode);

    unsigned char element_base64_out[10];
    size_t element_base64_out_size = sizeof(element_base64_out);

    int ret = base64_encode(element, element_size,
                            element_base64_out, &element_base64_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    ret = base64_decode(element_base64_out, element_base64_out_size,
                        element_decode, &element_decode_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    TEST_ASSERT_MESSAGE((0 == memcmp(element, element_decode,
                                     element_decode_size)),
                        "encoding failed!(produced unexpected output)");
}

static void test_base64_04_free_conversion(void)
{
    size_t elements = 255;
    char elm[elements];

    size_t elem_base64_out_size = 0;
    unsigned char elm_base64_out[((elements / 3) * 4) + (elements / 10)];

    size_t elem_base64_out_decoded_size = 0;
    unsigned char elem_base64_out_decoded[ elements + 10 ];

    /* fill some values */
    for (size_t i = 0; i < elements; ++i) {
        elm[i] = i;
    }

    int ret = base64_encode(elm, elements, NULL, &elem_base64_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_encode(elm, elements, elm_base64_out, &elem_base64_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    ret = base64_decode(elm_base64_out, elem_base64_out_size,
                        NULL, &elem_base64_out_decoded_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_decode(elm_base64_out, elem_base64_out_size,
                        elem_base64_out_decoded, &elem_base64_out_decoded_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    TEST_ASSERT_MESSAGE((0 == memcmp(elem_base64_out_decoded, elm,
                                     elements)),
                        "decoding failed!(produced unexpected output)");
}

static void test_base64_05_decode_larger(void)
{
    static const char decodeit[] =
        "SG93IG11Y2ggd29vZCB3b3VsZCBhIHdvb2RjaHVjayBjaHVj"
        "awppZiBhIHdvb2RjaHVjayBjb3VsZCBjaHVjayB3b29kPwpI"
        "ZSB3b3VsZCBjaHVjaywgaGUgd291bGQsIGFzIG11Y2ggYXMg"
        "aGUgY291bGQsCmFuZCBjaHVjayBhcyBtdWNoIHdvb2QgYXMg"
        "YSB3b29kY2h1Y2sgd291bGQKaWYgYSB3b29kY2h1Y2sgY291"
        "bGQgY2h1Y2sgd29vZC4==";

    size_t data_size = 199;
    char data[data_size];

    size_t decodeit_size = strlen(decodeit);

    int ret = base64_decode(decodeit, decodeit_size, data, &data_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 05 Decoded:");

    for (size_t i = 0; i < data_size; ++i) {
        printf("%c", data[i]);
    }

    printf("\nFrom:\n%s\n", decodeit);
#endif
}

static void test_base64_06_stream_encode(void)
{
    /*
    * @Note:
    * In this test we divide the `stream_encode[]` input for the encoding
    * into several portions (chunks).
    *
    * Every chunk is encoded and appended to the base64 `encode_result[]`.
    * To enable appending further, the chunks MUST be dividable by 3.
    * Only the final chunk MAY be prime to 3. (it is in this test)
    *
    */

    static const char stream_encode[] =
        "Peter Piper picked a peck of pickled peppers.\n"
        "A peck of pickled peppers Peter Piper picked.\n"
        "If Peter Piper picked a peck of pickled peppers,\n"
        "Where's the peck of pickled peppers Peter Piper picked?";

    /* required output size +2 extra bytes */
    size_t encoded_size = 264 + 2;
    /* cppcheck-suppress unassignedVariable
     * (reason: the above array is used/assigned in base64_encode() using its pointer) */
    char encode_result[encoded_size];

    int remain = strlen(stream_encode);
    int out_iter = 0;

    int ret = BASE64_SUCCESS;

    for (int i = 3; i < remain; (i += 3)) {
        size_t size_used = encoded_size - out_iter;
        ret = base64_encode(stream_encode + (strlen(stream_encode) - remain),
                            i, encode_result + out_iter, &size_used);
        TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

        out_iter += size_used;
        remain -= i;
    }

    /* this final chunk we want to encode and append is prime to 3 */
    size_t finish = encoded_size - out_iter;
    ret = base64_encode(stream_encode + (strlen(stream_encode) - remain),
                        remain, encode_result + out_iter, &finish);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    out_iter += finish;
    puts("Test 06 Encoded:");

    for (size_t i = 0; i < out_iter; ++i) {
        printf("%c", encode_result[i]);
    }

    printf("\nFrom:\n%s\n", stream_encode);
#endif
}

static void test_base64_07_stream_decode(void)
{
    /* @Note:
    * Here we decode the base64 string `encoded[]`
    *
    * Every base64 chunk is decoded and appended to `stream_decoded[]`.
    * The chunks passed to decode MUST be dividable by 4.
    */

    static const char encoded[] =
        "UGV0ZXIgUGlwZXIgcGlja2VkIGEgcGVjayBvZiBwaWNrbGVkIH"
        "BlcHBlcnMuCkEgcGVjayBvZiBwaWNrbGVkIHBlcHBlcnMgUGV0"
        "ZXIgUGlwZXIgcGlja2VkLgpJZiBQZXRlciBQaXBlciBwaWNrZW"
        "QgYSBwZWNrIG9mIHBpY2tsZWQgcGVwcGVycywKV2hlcmUncyB0"
        "aGUgcGVjayBvZiBwaWNrbGVkIHBlcHBlcnMgUGV0ZXIgUGlwZX"
        "IgcGlja2VkPw==";

    /* required output size +2 extra bytes */
    size_t decoded_size = 196 + 2;

    /* cppcheck-suppress unassignedVariable
     * (reason: the above array is used/assigned in base64_decode() using its pointer) */
    char stream_decoded[decoded_size];

    size_t encoded_size = strlen(encoded);
    int remain = encoded_size;
    int out_iter = 0;

    int ret = BASE64_SUCCESS;

    for (int i = 4; i < remain; (i += 4)) {
        size_t size_used = decoded_size - out_iter;
        ret = base64_decode(encoded + (encoded_size - remain),
                            i, stream_decoded + out_iter, &size_used);
        TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

        out_iter += size_used;
        remain -= i;
    }

    size_t finish = decoded_size - out_iter;
    ret = base64_decode(encoded + (encoded_size - remain),
                        remain, stream_decoded + out_iter, &finish);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    out_iter += finish;
    puts("Test 07 Decoded:");

    for (size_t i = 0; i < out_iter; ++i) {
        printf("%c", stream_decoded[i]);
    }

    printf("\nFrom:\n%s\n", encoded);
#endif
}

static void test_base64_08_encode_16_bytes(void)
{
    unsigned char buffer[16];
    size_t buffer_size = sizeof(buffer);
    for (size_t i = 0; i < buffer_size; ++i) {
        buffer[i] = 'a';
    }

    char element_base64_out[24];
    size_t expected_out_size = sizeof(element_base64_out);
    size_t element_base64_out_size = expected_out_size;

    size_t required_out_size = 0;
    int ret = base64_encode(buffer, buffer_size,
                            element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    ret = base64_encode(buffer, buffer_size,
                        element_base64_out, &element_base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);
    TEST_ASSERT_EQUAL_INT(expected_out_size, element_base64_out_size);
}

static void test_base64_09_encode_size_determination(void)
{
    enum { buffer_size = 20 };
    unsigned char buffer[buffer_size];
    for (int i = 0; i < buffer_size; ++i) {
        buffer[i] = 'a';
    }

    size_t expected_out_size = 28;
    size_t element_base64_out_size = expected_out_size;
    unsigned char element_base64_out[expected_out_size];

    // test 20 bytes input, expected 28 bytes output
    size_t required_out_size = 0;
    int ret = base64_encode(buffer, buffer_size,
                            element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    ret = base64_encode(buffer, buffer_size,
                        element_base64_out, &element_base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);
    TEST_ASSERT_EQUAL_INT(expected_out_size, element_base64_out_size);

    // test 19 bytes input, expected 28 bytes output
    required_out_size = 0;
    ret = base64_encode(buffer, 19,
                        element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    // test 18 bytes input, expected 24 bytes output
    expected_out_size = 24;
    required_out_size = 0;
    ret = base64_encode(buffer, 18,
                        element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    // test 17 bytes input, expected 24 bytes output
    expected_out_size = 24;
    required_out_size = 0;
    ret = base64_encode(buffer, 17,
                        element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);
}

static void test_base64_10_encode_empty(void)
{
    unsigned char data_in[] = "";

    size_t base64_out_size = 8;
    unsigned char base64_out[8];

    int ret = base64_encode(data_in, 0, base64_out, &base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);
    TEST_ASSERT_EQUAL_INT(0, base64_out_size);
}

static void test_base64_10_decode_empty(void)
{
    unsigned char data_in[] = "";

    size_t base64_out_size = 8;
    unsigned char base64_out[8];

    int ret = base64_decode(data_in, 0, base64_out, &base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);
    TEST_ASSERT_EQUAL_INT(0, base64_out_size);
}

static void test_base64_11_urlsafe_encode_int(void)
{
    uint32_t data_in = 4345;
    unsigned char expected_encoding[] = "-RAAAA";

    size_t base64_out_size = 0;

    /* Up to two = signs are suppressed in urlsafe encoding at the end, but
     * still written in the implementation. Just allocate 2 bytes more as worst
     * case */
    char base64_out[sizeof(expected_encoding) + 2];

    /*
    * @Note:
    * The first encoding attempt fails, but reveals the required out size.
    *
    * This size is a lower bound estimation,
    * thus it can require few more bytes then the actual used size for the output.
    */
    int ret = base64url_encode(&data_in, sizeof(data_in), NULL, &base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64url_encode(&data_in, sizeof(data_in), base64_out, &base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    TEST_ASSERT_MESSAGE((0 == memcmp(expected_encoding, base64_out,
                                     base64_out_size)),
                        "encoding failed!(produced unexpected output)");

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 11 Encoded:");

    for (size_t i = 0; i < base64_out_size; ++i) {
        printf("%c", base64_out[i]);
    }

    printf("\nFrom:\n%x\n", data_in);
#endif
}

static void test_base64_12_urlsafe_decode_int(void)
{
    static const char encoded_base64[]  = "_____wAA";
    static const uint8_t expected[]     = {0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0};

    size_t base64_size = strlen(encoded_base64);

    size_t data_out_size = 0;
    unsigned char data_out[sizeof(expected)];

    int ret = base64_decode(encoded_base64, base64_size, NULL, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_decode(encoded_base64, base64_size, NULL, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT, ret);

    ret = base64_decode(encoded_base64, base64_size, data_out, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    TEST_ASSERT_MESSAGE((0 == memcmp(expected, data_out, data_out_size)),
                        "decoding failed!(produced unexpected output)");

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 11 Decoded:");

    for (size_t i = 0; i < data_out_size; ++i) {
        printf("%x", data_out[i]);
    }

    printf("\nFrom:\n%s\n", encoded_base64);
#endif
}

static void test_base64_13_size_estimation(void) {
    size_t expected = 0;
    for (size_t i = 0; i < 33; i += 3) {
        TEST_ASSERT_EQUAL_INT(expected, base64_estimate_encode_size(i + 0));
        expected += 4;
        TEST_ASSERT_EQUAL_INT(expected, base64_estimate_encode_size(i + 1));
        TEST_ASSERT_EQUAL_INT(expected, base64_estimate_encode_size(i + 2));
    }

    expected = 0;
    for (size_t i = 0; i < 44; i += 4) {
        TEST_ASSERT_EQUAL_INT(expected, base64_estimate_decode_size(i + 0));
        expected += 3;
        TEST_ASSERT_EQUAL_INT(expected, base64_estimate_decode_size(i + 1));
        TEST_ASSERT_EQUAL_INT(expected, base64_estimate_decode_size(i + 2));
        TEST_ASSERT_EQUAL_INT(expected, base64_estimate_decode_size(i + 3));
    }
}

Test *tests_base64_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_base64_01_encode_string),
        new_TestFixture(test_base64_02_decode_base64),
        new_TestFixture(test_base64_03_single_character),
        new_TestFixture(test_base64_04_free_conversion),
        new_TestFixture(test_base64_05_decode_larger),
        new_TestFixture(test_base64_06_stream_encode),
        new_TestFixture(test_base64_07_stream_decode),
        new_TestFixture(test_base64_08_encode_16_bytes),
        new_TestFixture(test_base64_09_encode_size_determination),
        new_TestFixture(test_base64_10_encode_empty),
        new_TestFixture(test_base64_10_decode_empty),
        new_TestFixture(test_base64_11_urlsafe_encode_int),
        new_TestFixture(test_base64_12_urlsafe_decode_int),
        new_TestFixture(test_base64_13_size_estimation),
    };

    EMB_UNIT_TESTCALLER(base64_tests, NULL, NULL, fixtures);

    return (Test *)&base64_tests;
}

void tests_base64(void)
{
    TESTS_RUN(tests_base64_tests());
}
