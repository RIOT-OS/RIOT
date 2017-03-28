/*
* Copyright (C) 2015 Martin Landsmann
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

#define TEST_BASE64_SHOW_OUTPUT (0) /**< set if encoded/decoded string is displayed */

#if (TEST_BASE64_SHOW_OUTPUT == 1)
#include <stdio.h>
#endif
#include <string.h>
#include "embUnit.h"
#include "tests-base64.h"

#include "base64.h"

static void test_base64_01_encode_string(void)
{
    unsigned char data_in[] = "Hello RIOT this is a base64 test!\n"
                              "This should work as intended.";

    unsigned char expected_encoding[] = "SGVsbG8gUklPVCB0aGlzIGlzIGEgYmFzZTY0IHR"
                                        "lc3QhClRoaXMgc2hvdWxkIHdvcmsgYXMgaW50ZW5kZWQu";

    size_t data_in_size = strlen((char *)data_in);

    size_t base64_out_size = 0;
    unsigned char base64_out[ strlen((char *)expected_encoding) ];

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

    for (int i = 0; i < (int)base64_out_size; ++i) {
        TEST_ASSERT_MESSAGE(base64_out[i] == expected_encoding[i], \
                            "encoding failed!(produced unexpected output)");
    }

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 01 Encoded:");

    for (int i = 0; i < (int)base64_out_size; ++i) {
        printf("%c", base64_out[i]);
    }

    printf("\nFrom:\n%s\n", (char *)data_in);
#endif
}

static void test_base64_02_decode_base64(void)
{
    unsigned char encoded_base64[] = "SGVsbG8gUklPVCB0aGlzIGlzIGEgYmFzZTY0IHRlc3Q"
                                     "hClRoaXMgc2hvdWxkIHdvcmsgYXMgaW50ZW5kZWQu";

    unsigned char expected_string[] = "Hello RIOT this is a base64 test!\n"
                                      "This should work as intended.";

    size_t base64_size = strlen((char *)encoded_base64);

    size_t data_out_size = 0;
    unsigned char data_out[ strlen((char *)expected_string) ];

    int ret = base64_decode(encoded_base64, base64_size, NULL, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_decode(encoded_base64, base64_size, NULL, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT, ret);

    ret = base64_decode(encoded_base64, base64_size, data_out, &data_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    for (int i = 0; i < (int)data_out_size; ++i) {
        TEST_ASSERT_MESSAGE(data_out[i] == expected_string[i], \
                            "decoding failed!(produced unexpected output)");
    }

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 02 Decoded:");

    for (int i = 0; i < (int)data_out_size; ++i) {
        printf("%c", data_out[i]);
    }

    printf("\nFrom:\n%s\n", (char *)encoded_base64);
#endif
}

static void test_base64_03_single_character(void)
{
    size_t element_size = 1;
    unsigned char element[] = "1";

    size_t elementDecodeSize = 3;
    unsigned char elementDecode[3];

    size_t element_base64_out_size = 10;
    unsigned char element_base64_out[10];

    int ret = base64_encode(element, element_size, \
                            element_base64_out, &element_base64_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    ret = base64_decode(element_base64_out, element_base64_out_size, \
                        elementDecode, &elementDecodeSize);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    for (int i = 0; i < (int)elementDecodeSize; ++i) {
        TEST_ASSERT_MESSAGE(element[i] == elementDecode[i], \
                            "decoding failed!(produced unexpected output)");
    }
}

static void test_base64_04_free_conversion(void)
{
    size_t elements = 255;
    unsigned char elm[elements];

    size_t elem_base64_out_size = 0;
    unsigned char elm_base64_out[((elements / 3) * 4) + (elements / 10)];

    size_t elem_base64_out_decoded_size = 0;
    unsigned char elem_base64_out_decoded[ elements + 10 ];

    /* fill some values */
    for (int i = 0; i < (int)elements; ++i) {
        elm[i] = i;
    }

    int ret = base64_encode(elm, elements, NULL, &elem_base64_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_encode(elm, elements, elm_base64_out, &elem_base64_out_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    ret = base64_decode(elm_base64_out, elem_base64_out_size, \
                        NULL, &elem_base64_out_decoded_size);
    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);

    ret = base64_decode(elm_base64_out, elem_base64_out_size, \
                        elem_base64_out_decoded, &elem_base64_out_decoded_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

    for (int i = 0; i < (int)elements; ++i) {
        TEST_ASSERT_MESSAGE(elem_base64_out_decoded[i] == elm[i], \
                            "decoding failed!(produced unexpected output)");
    }
}

static void test_base64_05_decode_larger(void)
{
    unsigned char decodeit[] = "SG93IG11Y2ggd29vZCB3b3VsZCBhIHdvb2RjaHVjayBjaHVj"
                               "awppZiBhIHdvb2RjaHVjayBjb3VsZCBjaHVjayB3b29kPwpI"
                               "ZSB3b3VsZCBjaHVjaywgaGUgd291bGQsIGFzIG11Y2ggYXMg"
                               "aGUgY291bGQsCmFuZCBjaHVjayBhcyBtdWNoIHdvb2QgYXMg"
                               "YSB3b29kY2h1Y2sgd291bGQKaWYgYSB3b29kY2h1Y2sgY291"
                               "bGQgY2h1Y2sgd29vZC4==";

    size_t data_size = 199;
    unsigned char data[data_size];

    size_t decodeit_size = strlen((char *)decodeit);

    int ret = base64_decode(decodeit, decodeit_size, data, &data_size);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    puts("Test 05 Decoded:");

    for (int i = 0; i < (int)data_size; ++i) {
        printf("%c", data[i]);
    }

    printf("\nFrom:\n%s\n", (char *)decodeit);
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

    unsigned char stream_encode[] = "Peter Piper picked a peck of pickled peppers."
                                    "\nA peck of pickled peppers Peter Piper picked."
                                    "\nIf Peter Piper picked a peck of pickled peppers,"
                                    "\nWhere's the peck of pickled peppers Peter Piper picked?";

    /* required output size +2 extra bytes */
    size_t encoded_size = 264 + 2;
    /* cppcheck-suppress unassignedVariable */
    unsigned char encode_result[encoded_size];
    /* the above array is used/assigned in base64_encode() using its pointer. */

    int remain = strlen((char *)stream_encode);
    int out_iter = 0;

    int ret = BASE64_SUCCESS;

    for (int i = 3; i < remain; (i += 3)) {
        size_t size_used = encoded_size - out_iter;
        ret = base64_encode(stream_encode + (strlen((char *)stream_encode) - remain), \
                            i, encode_result + out_iter, &size_used);
        TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

        out_iter += size_used;
        remain -= i;
    }

    /* this final chunk we want to encode and append is prime to 3 */
    size_t finish = encoded_size - out_iter;
    ret = base64_encode(stream_encode + (strlen((char *)stream_encode) - remain), \
                        remain, encode_result + out_iter, &finish);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    out_iter += finish;
    puts("Test 06 Encoded:");

    for (int i = 0; i < (int)out_iter; ++i) {
        printf("%c", encode_result[i]);
    }

    printf("\nFrom:\n%s\n", (char *)stream_encode);
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

    unsigned char encoded[] = "UGV0ZXIgUGlwZXIgcGlja2VkIGEgcGVjayBvZiBwaWNrbGVkIH"
                              "BlcHBlcnMuCkEgcGVjayBvZiBwaWNrbGVkIHBlcHBlcnMgUGV0"
                              "ZXIgUGlwZXIgcGlja2VkLgpJZiBQZXRlciBQaXBlciBwaWNrZW"
                              "QgYSBwZWNrIG9mIHBpY2tsZWQgcGVwcGVycywKV2hlcmUncyB0"
                              "aGUgcGVjayBvZiBwaWNrbGVkIHBlcHBlcnMgUGV0ZXIgUGlwZX"
                              "IgcGlja2VkPw==";

    /* required output size +2 extra bytes */
    size_t decoded_size = 196 + 2;

    /* cppcheck-suppress unassignedVariable */
    unsigned char stream_decoded[decoded_size];
    /* the above array is used/assigned in base64_decode() using its pointer. */

    size_t encoded_size = strlen((char *)encoded);
    int remain = encoded_size;
    int out_iter = 0;

    int ret = BASE64_SUCCESS;

    for (int i = 4; i < remain; (i += 4)) {
        size_t size_used = decoded_size - out_iter;
        ret = base64_decode(encoded + (encoded_size - remain), \
                            i, stream_decoded + out_iter, &size_used);
        TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

        out_iter += size_used;
        remain -= i;
    }

    size_t finish = decoded_size - out_iter;
    ret = base64_decode(encoded + (encoded_size - remain), \
                        remain, stream_decoded + out_iter, &finish);
    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);

#if (TEST_BASE64_SHOW_OUTPUT == 1)
    out_iter += finish;
    puts("Test 07 Decoded:");

    for (int i = 0; i < (int)out_iter; ++i) {
        printf("%c", stream_decoded[i]);
    }

    printf("\nFrom:\n%s\n", (char *)encoded);
#endif
}

static void test_base64_08_encode_16_bytes(void)
{
    /* FIXME: init as enum here and below required,
     * to fix folding-constant compiler error on OS X
     */
    enum { buffer_size = 16 };
    unsigned char buffer[buffer_size];
    for (int i = 0; i < buffer_size; ++i) {
        buffer[i] = 'a';
    }

    enum { expected_out_size = 24 };
    size_t element_base64_out_size = expected_out_size;
    unsigned char element_base64_out[expected_out_size];

    size_t required_out_size = 0;
    int ret = base64_encode(buffer, buffer_size, \
                            element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    ret = base64_encode(buffer, buffer_size, \
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
    int ret = base64_encode(buffer, buffer_size, \
                            element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    ret = base64_encode(buffer, buffer_size, \
                            element_base64_out, &element_base64_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_SUCCESS, ret);
    TEST_ASSERT_EQUAL_INT(expected_out_size, element_base64_out_size);

    // test 19 bytes input, expected 28 bytes output
    required_out_size = 0;
    ret = base64_encode(buffer, 19, \
                            element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    // test 18 bytes input, expected 24 bytes output
    expected_out_size = 24;
    required_out_size = 0;
    ret = base64_encode(buffer, 18, \
                            element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);

    // test 17 bytes input, expected 24 bytes output
    expected_out_size = 24;
    required_out_size = 0;
    ret = base64_encode(buffer, 17, \
                            element_base64_out, &required_out_size);

    TEST_ASSERT_EQUAL_INT(BASE64_ERROR_BUFFER_OUT_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(required_out_size, expected_out_size);
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
    };

    EMB_UNIT_TESTCALLER(base64_tests, NULL, NULL, fixtures);

    return (Test *)&base64_tests;
}

void tests_base64(void)
{
    TESTS_RUN(tests_base64_tests());
}
