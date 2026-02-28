/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "net/nfc/ndef/ndef.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "log.h"

#include "container.h"
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

    ndef_init(&message, buffer, ARRAY_SIZE(buffer));
    ndef_record_text_add(&message, "Hello World", 11, "en", 2, UTF8);

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

    ndef_init(&message, buffer, ARRAY_SIZE(buffer));
    ndef_record_uri_add(&message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);

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

    ndef_init(&message, buffer, ARRAY_SIZE(buffer));
    ndef_record_mime_add(&message, "text/plain", 10, (uint8_t *)"Hello World", 11);

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

    ndef_init(&message, buffer, ARRAY_SIZE(buffer));
    ndef_record_text_add(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_record_text_add(&message, "Hej Verden", 10, "da", 2, UTF8);

    TEST_ASSERT(compare_ndef_buffers(message.buffer, ndef_two_record_buffer));
}

static void test_ndef_remove(void)
{
    puts("NDEF remove test");
    ndef_t message;
    static uint8_t buffer[1024];
    ndef_init(&message, buffer, ARRAY_SIZE(buffer));
    ndef_record_text_add(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_record_text_add(&message, "Hej Verden", 10, "da", 2, UTF8);

    TEST_ASSERT_EQUAL_INT(message.record_count, 2);

    ndef_record_remove_last(&message);

    TEST_ASSERT_EQUAL_INT(message.record_count, 1);
}

static void test_ndef_from_buffer(void)
{
    puts("NDEF from buffer test");
    ndef_t message;

    /* the buffer contains an NDEF message with two records */
    static uint8_t buffer[] = {
        0x91, 0x01, 0x0E, 0x54,
        0x02,                                                             /* status byte */
        0x65, 0x6E,                                                       /* en */
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, /* Hello World */

        0x51, 0x01, 0x0D, 0x54,
        0x02,                                                             /* status byte */
        0x64, 0x61,                                                       /* da */
        0x48, 0x65, 0x6A, 0x20, 0x56, 0x65, 0x72, 0x64, 0x65, 0x6E        /* Hej Verden */
    };

    TEST_ASSERT_EQUAL_INT(ndef_from_buffer(&message, buffer, sizeof(buffer)), 0);
    TEST_ASSERT_EQUAL_INT(ndef_get_size(&message), sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(message.record_count, 2);
}

static void test_ndef_long_record(void) {
    static uint8_t buffer[] = {
        0xC1, 0x01,
        0x00, 0x00, 0x01, 0xC0, /* payload length of 448 (MSB, 4 bytes) */
        0x54,
        0x02,                                                             /* status byte */
        0x65, 0x6E,                                                       /* en */
        0x4C, 0X6F, 0X72, 0X65, 0X6D, 0X20, 0X69, 0X70, 0X73, 0X75,       /* Lorem ipsum */
        0X6D, 0X20, 0X64, 0X6F, 0X6C, 0X6F, 0X72, 0X20, 0X73, 0X69,
        0X74, 0X20, 0X61, 0X6D, 0X65, 0X74, 0X2C, 0X20, 0X63, 0X6F,
        0X6E, 0X73, 0X65, 0X63, 0X74, 0X65, 0X74, 0X75, 0X72, 0X20,
        0X61, 0X64, 0X69, 0X70, 0X69, 0X73, 0X63, 0X69, 0X6E, 0X67,
        0X20, 0X65, 0X6C, 0X69, 0X74, 0X2C, 0X20, 0X73, 0X65, 0X64,
        0X20, 0X64, 0X6F, 0X20, 0X65, 0X69, 0X75, 0X73, 0X6D, 0X6F,
        0X64, 0X20, 0X74, 0X65, 0X6D, 0X70, 0X6F, 0X72, 0X20, 0X69,
        0X6E, 0X63, 0X69, 0X64, 0X69, 0X64, 0X75, 0X6E, 0X74, 0X20,
        0X75, 0X74, 0X20, 0X6C, 0X61, 0X62, 0X6F, 0X72, 0X65, 0X20,
        0X65, 0X74, 0X20, 0X64, 0X6F, 0X6C, 0X6F, 0X72, 0X65, 0X20,
        0X6D, 0X61, 0X67, 0X6E, 0X61, 0X20, 0X61, 0X6C, 0X69, 0X71,
        0X75, 0X61, 0X2E, 0X20, 0X55, 0X74, 0X20, 0X65, 0X6E, 0X69,
        0X6D, 0X20, 0X61, 0X64, 0X20, 0X6D, 0X69, 0X6E, 0X69, 0X6D,
        0X20, 0X76, 0X65, 0X6E, 0X69, 0X61, 0X6D, 0X2C, 0X20, 0X71,
        0X75, 0X69, 0X73, 0X20, 0X6E, 0X6F, 0X73, 0X74, 0X72, 0X75,
        0X64, 0X20, 0X65, 0X78, 0X65, 0X72, 0X63, 0X69, 0X74, 0X61,
        0X74, 0X69, 0X6F, 0X6E, 0X20, 0X75, 0X6C, 0X6C, 0X61, 0X6D,
        0X63, 0X6F, 0X20, 0X6C, 0X61, 0X62, 0X6F, 0X72, 0X69, 0X73,
        0X20, 0X6E, 0X69, 0X73, 0X69, 0X20, 0X75, 0X74, 0X20, 0X61,
        0X6C, 0X69, 0X71, 0X75, 0X69, 0X70, 0X20, 0X65, 0X78, 0X20,
        0X65, 0X61, 0X20, 0X63, 0X6F, 0X6D, 0X6D, 0X6F, 0X64, 0X6F,
        0X20, 0X63, 0X6F, 0X6E, 0X73, 0X65, 0X71, 0X75, 0X61, 0X74,
        0X2E, 0X20, 0X44, 0X75, 0X69, 0X73, 0X20, 0X61, 0X75, 0X74,
        0X65, 0X20, 0X69, 0X72, 0X75, 0X72, 0X65, 0X20, 0X64, 0X6F,
        0X6C, 0X6F, 0X72, 0X20, 0X69, 0X6E, 0X20, 0X72, 0X65, 0X70,
        0X72, 0X65, 0X68, 0X65, 0X6E, 0X64, 0X65, 0X72, 0X69, 0X74,
        0X20, 0X69, 0X6E, 0X20, 0X76, 0X6F, 0X6C, 0X75, 0X70, 0X74,
        0X61, 0X74, 0X65, 0X20, 0X76, 0X65, 0X6C, 0X69, 0X74, 0X20,
        0X65, 0X73, 0X73, 0X65, 0X20, 0X63, 0X69, 0X6C, 0X6C, 0X75,
        0X6D, 0X20, 0X64, 0X6F, 0X6C, 0X6F, 0X72, 0X65, 0X20, 0X65,
        0X75, 0X20, 0X66, 0X75, 0X67, 0X69, 0X61, 0X74, 0X20, 0X6E,
        0X75, 0X6C, 0X6C, 0X61, 0X20, 0X70, 0X61, 0X72, 0X69, 0X61,
        0X74, 0X75, 0X72, 0X2E, 0X20, 0X45, 0X78, 0X63, 0X65, 0X70,
        0X74, 0X65, 0X75, 0X72, 0X20, 0X73, 0X69, 0X6E, 0X74, 0X20,
        0X6F, 0X63, 0X63, 0X61, 0X65, 0X63, 0X61, 0X74, 0X20, 0X63,
        0X75, 0X70, 0X69, 0X64, 0X61, 0X74, 0X61, 0X74, 0X20, 0X6E,
        0X6F, 0X6E, 0X20, 0X70, 0X72, 0X6F, 0X69, 0X64, 0X65, 0X6E,
        0X74, 0X2C, 0X20, 0X73, 0X75, 0X6E, 0X74, 0X20, 0X69, 0X6E,
        0X20, 0X63, 0X75, 0X6C, 0X70, 0X61, 0X20, 0X71, 0X75, 0X69,
        0X20, 0X6F, 0X66, 0X66, 0X69, 0X63, 0X69, 0X61, 0X20, 0X64,
        0X65, 0X73, 0X65, 0X72, 0X75, 0X6E, 0X74, 0X20, 0X6D, 0X6F,
        0X6C, 0X6C, 0X69, 0X74, 0X20, 0X61, 0X6E, 0X69, 0X6D, 0X20,
        0X69, 0X64, 0X20, 0X65, 0X73, 0X74, 0X20, 0X6C, 0X61, 0X62,
        0X6F, 0X72, 0X75, 0X6D, 0X2E
    };

    ndef_t message;
    TEST_ASSERT_EQUAL_INT(ndef_from_buffer(&message, buffer, sizeof(buffer)), 0);
    TEST_ASSERT_EQUAL_INT(ndef_get_size(&message), sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(message.record_count, 1);
}

static void test_ndef_pretty_print(void)
{
    puts("NDEF pretty print test");

    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, ARRAY_SIZE(buffer));
    ndef_record_text_add(&message, "Hello World", 11, "en", 2, UTF8);

    ndef_pretty_print(&message);
}

static Test *tests_nfc_ndef_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_ndef_text_record),
        new_TestFixture(test_ndef_uri_record),
        new_TestFixture(test_ndef_mime_record),
        new_TestFixture(test_ndef_two_records),
        new_TestFixture(test_ndef_remove),
        new_TestFixture(test_ndef_from_buffer),
        new_TestFixture(test_ndef_long_record),
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
