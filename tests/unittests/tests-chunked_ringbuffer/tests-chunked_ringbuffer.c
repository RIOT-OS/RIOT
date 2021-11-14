/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <string.h>
#include "embUnit.h"

#include "chunked_ringbuffer.h"

static void test_crb_add_and_consume(void)
{
    size_t len;
    uint8_t buffer[16];
    char buf_out[6];
    chunk_ringbuf_t cb;

    crb_init(&cb, buffer, sizeof(buffer));

    TEST_ASSERT(crb_add_chunk(&cb, "one", 4));
    TEST_ASSERT(crb_add_chunk(&cb, "two", 4));
    TEST_ASSERT(crb_add_chunk(&cb, "three", 6));

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(4, len);
    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("one", buf_out);

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(4, len);
    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("two", buf_out);

    TEST_ASSERT(crb_add_chunk(&cb, "four", 5));

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(6, len);
    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("three", buf_out);

    TEST_ASSERT(crb_add_chunk(&cb, "five", 5));
    TEST_ASSERT(crb_add_chunk(&cb, "six", 4));

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("four", buf_out);

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("five", buf_out);

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(4, len);
    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("six", buf_out);
}

static void test_crb_add_while_consume(void)
{
    size_t len;
    uint8_t buffer[16];
    char buf_out[12];
    chunk_ringbuf_t cb;

    crb_init(&cb, buffer, sizeof(buffer));

    TEST_ASSERT(crb_add_chunk(&cb, "one", 4));

    TEST_ASSERT(crb_start_chunk(&cb));
    TEST_ASSERT(crb_add_bytes(&cb, "Hello", 5));

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(4, len);
    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("one", buf_out);

    TEST_ASSERT(!crb_get_chunk_size(&cb, &len));
    TEST_ASSERT(!crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));

    TEST_ASSERT(crb_add_bytes(&cb, "World", 6));
    TEST_ASSERT(crb_end_chunk(&cb, true));

    TEST_ASSERT(crb_get_chunk_size(&cb, &len));
    TEST_ASSERT_EQUAL_INT(11, len);

    TEST_ASSERT(crb_peek_bytes(&cb, buf_out, 5, 6));
    TEST_ASSERT_EQUAL_STRING("World", buf_out);

    TEST_ASSERT(crb_consume_chunk(&cb, buf_out, sizeof(buf_out)));
    TEST_ASSERT_EQUAL_STRING("HelloWorld", buf_out);
}

static Test *chunked_ringbuffer_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crb_add_and_consume),
        new_TestFixture(test_crb_add_while_consume),
    };

    EMB_UNIT_TESTCALLER(crb_tests, NULL, NULL, fixtures);

    return (Test *)&crb_tests;
}

void tests_chunked_ringbuffer(void)
{
    TESTS_RUN(chunked_ringbuffer_tests());
}
/** @} */
