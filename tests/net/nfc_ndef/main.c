/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "net/nfc/ndef/ndef.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "embUnit.h"

/**
 * @brief Compares two NDEF message buffers
 *
 * @param buffer1 First buffer to compare
 * @param buffer2 Second buffer to compare
 * @return true     if the buffers are equal
 * @return false    if the buffers are not equal
 */
static bool compare_ndef_buffers(ndef_buffer_t buffer1, ndef_buffer_t buffer2)
{
    size_t size1 = buffer1.cursor - buffer1.memory;
    size_t size2 = buffer2.cursor - buffer2.memory;

    if (size1 != size2) {
        return false;
    }

    return memcmp(buffer1.memory, buffer2.memory, size1) == 0;
}

static void test_ndef_text_record(void)
{
    puts("NDEF text record test");
    uint8_t ndef_text_record_data[] = {
        0xD1, 0x01, 0x0E, 0x54,
        0x02,                                                            /* status byte */
        0x65, 0x6E,                                                      /* en */
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64 /* Hello World */
    };
    ndef_buffer_t ndef_text_record_buffer = {
        .memory = ndef_text_record_data,
        .cursor = ndef_text_record_data + sizeof(ndef_text_record_data),
        .memory_end = ndef_text_record_data + sizeof(ndef_text_record_data)
    };

    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);

    TEST_ASSERT(compare_ndef_buffers(message.buffer, ndef_text_record_buffer));
}

static void test_ndef_uri_record(void)
{
    puts("NDEF URI record test");
    uint8_t ndef_uri_record_data[] = {
        0xD1, 0x01, 0x0C, 0x55,
        0x02,                                                            /* https */
        0x72, 0x69, 0x6F, 0x74, 0x2D, 0x6F, 0x73, 0x2E, 0x6F, 0x72, 0x67 /* riot-os.org */
    };
    ndef_buffer_t ndef_uri_record_buffer = {
        .memory = ndef_uri_record_data,
        .cursor = ndef_uri_record_data + sizeof(ndef_uri_record_data),
        .memory_end = ndef_uri_record_data + sizeof(ndef_uri_record_data)
    };

    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_record_add_uri(&message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);

    TEST_ASSERT(compare_ndef_buffers(message.buffer, ndef_uri_record_buffer));
}

static void test_ndef_mime_record(void)
{
    puts("NDEF MIME record test");
    uint8_t ndef_uri_record_data[] = {
        0xD2, 0x0A, 0x0B,
        0x74, 0x65, 0x78, 0x74, 0x2F, 0x70, 0x6C, 0x61, 0x69, 0x6E,      /* text/plain */
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64 /* Hello World */
    };
    ndef_buffer_t ndef_uri_record_buffer = {
        .memory = ndef_uri_record_data,
        .cursor = ndef_uri_record_data + sizeof(ndef_uri_record_data),
        .memory_end = ndef_uri_record_data + sizeof(ndef_uri_record_data)
    };
    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_record_add_mime(&message, "text/plain", 10, (uint8_t *)"Hello World", 11);

    TEST_ASSERT(compare_ndef_buffers(message.buffer, ndef_uri_record_buffer));
}

static void test_ndef_two_records(void)
{
    puts("NDEF two records test");
    uint8_t ndef_two_record_data[] = {
        0x91, 0x01, 0x0E, 0x54,
        0x02,                                                             /* status byte */
        0x65, 0x6E,                                                       /* en */
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, /* Hello World */

        0x51, 0x01, 0x0D, 0x54,
        0x02,                                                      /* status byte */
        0x64, 0x61,                                                /* da */
        0x48, 0x65, 0x6A, 0x20, 0x56, 0x65, 0x72, 0x64, 0x65, 0x6E /* Hej Verden */
    };
    ndef_buffer_t ndef_two_record_buffer = {
        .memory = ndef_two_record_data,
        .cursor = ndef_two_record_data + sizeof(ndef_two_record_data),
        .memory_end = ndef_two_record_data + sizeof(ndef_two_record_data)
    };

    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_record_add_text(&message, "Hej Verden", 10, "da", 2, UTF8);

    TEST_ASSERT(compare_ndef_buffers(message.buffer, ndef_two_record_buffer));
}

static void test_ndef_remove(void)
{
    puts("NDEF remove test");
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_record_add_text(&message, "Hej Verden", 10, "da", 2, UTF8);

    uint8_t ndef_two_record_data[] = {
        0x91, 0x01, 0x0E, 0x54,
        0x02,                                                             /* status byte */
        0x65, 0x6E,                                                       /* en */
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, /* Hello World */

        0x51, 0x01, 0x0D, 0x54,
        0x02,                                                      /* status byte */
        0x64, 0x61,                                                /* da */
        0x48, 0x65, 0x6A, 0x20, 0x56, 0x65, 0x72, 0x64, 0x65, 0x6E /* Hej Verden */
    };
    ndef_buffer_t ndef_two_record_buffer = {
        .memory = ndef_two_record_data,
        .cursor = ndef_two_record_data + sizeof(ndef_two_record_data),
        .memory_end = ndef_two_record_data + sizeof(ndef_two_record_data)
    };

    TEST_ASSERT(compare_ndef_buffers(message.buffer, ndef_two_record_buffer));

    ndef_remove_last_record(&message);

    uint8_t ndef_one_record_data[] = {
        0xD1, 0x01, 0x0E, 0x54,
        0x02,                                                            /* status byte */
        0x65, 0x6E,                                                      /* en */
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64 /* Hello World */
    };
    ndef_buffer_t ndef_one_record_buffer = {
        .memory = ndef_one_record_data,
        .cursor = ndef_one_record_data + sizeof(ndef_one_record_data),
        .memory_end = ndef_one_record_data + sizeof(ndef_one_record_data)
    };

    TEST_ASSERT(compare_ndef_buffers(message.buffer, ndef_one_record_buffer));
}

static void test_ndef_calculate_size(void)
{
    puts("NDEF calculate size test");
    uint8_t buffer[1024];
    ndef_t message;
    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);
    TEST_ASSERT_EQUAL_INT((uint32_t)(message.buffer.cursor - message.buffer.memory),
                          ndef_record_calculate_text_size(11, 2));
    ndef_clear(&message);

    ndef_record_add_uri(&message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);
    TEST_ASSERT_EQUAL_INT((uint32_t)(message.buffer.cursor - message.buffer.memory),
                          ndef_record_calculate_uri_size(11));
    ndef_clear(&message);

    ndef_record_add_mime(&message, "text/plain", 10, (uint8_t *)"Hello World", 11);
    TEST_ASSERT_EQUAL_INT((uint32_t)(message.buffer.cursor - message.buffer.memory),
                          ndef_record_calculate_mime_size(10, 11));
}

static void test_ndef_pretty_print(void)
{
    puts("NDEF pretty print test");

    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_record_add_text(&message, "Hello World", 11, "en", 2, UTF8);

    ndef_record_desc_t record_descriptors[1];
    ndef_parse(&message, record_descriptors, 1);
    ndef_pretty_print(record_descriptors, 1);
}

static Test *tests_nfc_ndef_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_ndef_text_record),
        new_TestFixture(test_ndef_uri_record),
        new_TestFixture(test_ndef_mime_record),
        new_TestFixture(test_ndef_two_records),
        new_TestFixture(test_ndef_remove),
        new_TestFixture(test_ndef_calculate_size),
        new_TestFixture(test_ndef_pretty_print),
    };

    EMB_UNIT_TESTCALLER(nfc_ndef_tests, NULL, NULL, fixtures);

    return (Test *)&nfc_ndef_tests;
}

int main(void)
{
    puts("Starting NDEF test");
    TESTS_START();
    TESTS_RUN(tests_nfc_ndef_tests());
    TESTS_END();
    puts("Ending NDEF test");
    return 0;
}
