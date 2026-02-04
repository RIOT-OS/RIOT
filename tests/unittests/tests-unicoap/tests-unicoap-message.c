/*
 * SPDX-FileCopyrightText: 2024-2025 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup unittests
 * @brief   Unit tests for contiguous and noncontiguous payload functionality
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include <stdio.h>
#include <string.h>

#include "tests-unicoap.h"

#include "net/unicoap/message.h"
#include "net/unicoap/options.h"

static void test_contiguous_payload(void)
{
    unicoap_message_t message = { .method = UNICOAP_METHOD_GET };

    TEST_ASSERT(unicoap_message_payload_is_empty(&message));

    uint8_t payload[] = { 0xc0, 0xff, 0xee };
    unicoap_message_payload_set(&message, payload, sizeof(payload));

    _TEST_ASSERT_EQUAL_POINTER(unicoap_message_payload_get(&message), payload);
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_get_size(&message), sizeof(payload));
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_is_empty(&message), false);
}

static void test_contiguous_payload_copy(void)
{
    unicoap_message_t message = { .method = UNICOAP_METHOD_GET };

    uint8_t payload[] = { 0xc0, 0xff, 0xee };
    unicoap_message_payload_set(&message, payload, sizeof(payload));

    uint8_t payload2[sizeof(payload)];
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_copy(&message, payload2,
                                                       sizeof(payload2)), sizeof(payload));
    _TEST_ASSERT_EQUAL_BYTES(payload, payload2, sizeof(payload));
}

static void test_noncontiguous_payload(void)
{
    unicoap_message_t message = { .method = UNICOAP_METHOD_GET };

    TEST_ASSERT(unicoap_message_payload_is_empty(&message));

    uint8_t payload2[] = { 0xee };

    iolist_t chunk2 = {
        .iol_base = payload2,
        .iol_len = sizeof(payload2)
    };

    uint8_t payload[] = { 0xc0, 0xff };

    iolist_t chunk = {
        .iol_base = payload,
        .iol_len = sizeof(payload),
        .iol_next = &chunk2
    };

    unicoap_message_payload_set_chunks(&message, &chunk);
    TEST_ASSERT_EQUAL_INT(message.payload_representation, UNICOAP_PAYLOAD_NONCONTIGUOUS);

    _TEST_ASSERT_EQUAL_POINTER(unicoap_message_payload_get_chunks(&message), &chunk);
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_get_size(&message),
                          sizeof(payload) + sizeof(payload2));
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_is_empty(&message), false);
}

static void test_noncontiguous_payload_append(void)
{
    unicoap_message_t message = { .method = UNICOAP_METHOD_GET };

    uint8_t payload2[] = { 0xee };

    iolist_t chunk2 = {
        .iol_base = payload2,
        .iol_len = sizeof(payload2)
    };

    uint8_t payload[] = { 0xc0, 0xff };

    iolist_t chunk = {
        .iol_base = payload,
        .iol_len = sizeof(payload),
        .iol_next = &chunk2
    };

    unicoap_message_payload_set_chunks(&message, &chunk);

    const char hello[] = "Hello, World!";
    iolist_t chunk3 = {
        .iol_base = (void*)hello,
        .iol_len = static_strlen(hello)
    };

    unicoap_message_payload_append_chunk(&message, &chunk3);
    _TEST_ASSERT_EQUAL_POINTER(unicoap_message_payload_get_chunks(&message), &chunk);
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_get_size(&message),
                          sizeof(payload) + sizeof(payload2) + static_strlen(hello));
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_is_empty(&message), false);
}

static void test_noncontiguous_payload_make_contiguous(void)
{
    unicoap_message_t message = { .method = UNICOAP_METHOD_GET };

    uint8_t payload2[] = { 0xee };

    iolist_t chunk2 = {
        .iol_base = payload2,
        .iol_len = sizeof(payload2)
    };

    uint8_t payload[] = { 0xc0, 0xff };

    iolist_t chunk = {
        .iol_base = payload,
        .iol_len = sizeof(payload),
        .iol_next = &chunk2
    };

    unicoap_message_payload_set_chunks(&message, &chunk);

    const char hello[] = "Hello, World!";
    iolist_t chunk3 = {
        .iol_base = (void*)hello,
        .iol_len = static_strlen(hello)
    };

    unicoap_message_payload_append_chunk(&message, &chunk3);
    TEST_ASSERT_EQUAL_INT(message.payload_representation, UNICOAP_PAYLOAD_NONCONTIGUOUS);

    uint8_t buffer[sizeof(payload) + sizeof(payload2) + static_strlen(hello)];
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_make_contiguous(&message, buffer,
                                                                  sizeof(buffer)), sizeof(buffer));
}

static void test_noncontiguous_payload_copy(void)
{
    unicoap_message_t message = { .method = UNICOAP_METHOD_GET };

    uint8_t payload2[] = { 0xee };

    iolist_t chunk2 = {
        .iol_base = payload2,
        .iol_len = sizeof(payload2)
    };

    uint8_t payload[] = { 0xc0, 0xff };

    iolist_t chunk = {
        .iol_base = payload,
        .iol_len = sizeof(payload),
        .iol_next = &chunk2
    };

    unicoap_message_payload_set_chunks(&message, &chunk);
    TEST_ASSERT(message.payload_representation == UNICOAP_PAYLOAD_NONCONTIGUOUS);

    const char hello[] = "Hello, World!";
    iolist_t chunk3 = {
        .iol_base = (void*)hello,
        .iol_len = static_strlen(hello)
    };

    unicoap_message_payload_append_chunk(&message, &chunk3);
    TEST_ASSERT(message.payload_representation == UNICOAP_PAYLOAD_NONCONTIGUOUS);

    uint8_t buffer[sizeof(payload) + sizeof(payload2) + static_strlen(hello)];
    TEST_ASSERT_EQUAL_INT(unicoap_message_payload_copy(&message, buffer,
                                                       sizeof(buffer)), sizeof(buffer));

    const uint8_t buffer2[] = {
        0xc0, 0xff, 0xee, 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'
    };

    TEST_ASSERT_EQUAL_INT(sizeof(buffer), sizeof(buffer2));
    _TEST_ASSERT_EQUAL_BYTES(buffer, buffer2, sizeof(buffer));
}

Test* tests_unicoap_message(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_contiguous_payload),
        new_TestFixture(test_contiguous_payload_copy),
        new_TestFixture(test_noncontiguous_payload),
        new_TestFixture(test_noncontiguous_payload_append),
        new_TestFixture(test_noncontiguous_payload_make_contiguous),
        new_TestFixture(test_noncontiguous_payload_copy),
    };

    EMB_UNIT_TESTCALLER(test_unicoap, NULL, NULL, fixtures);

    return (Test*)&test_unicoap;
}
