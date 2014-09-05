/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */
#include <errno.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "packetbuf.h"

#include "tests-packetbuf.h"

#include "../unittests.h"

#define BUFSIZE (256)

static char buffer[BUFSIZE];

static packetbuf_t buf;

static void set_up(void)
{
    packetbuf_init(&buf, buffer, BUFSIZE);
}

static void test_packetbuf_add_to_NULL_DEFAULT(void)
{
    TEST_ASSERT_NULL(packetbuf_add(NULL, NULL, 0, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_one_len_0_DEFAULT(void)
{
    TEST_ASSERT_NULL(packetbuf_add(&buf, NULL, 0, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_one_len_too_long_DEFAULT(void)
{
    TEST_ASSERT_NULL(packetbuf_add(&buf, NULL, BUFSIZE+1, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_one_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
}

static void test_packetbuf_add_two_len_too_long_DEFAULT(void)
{
    char data[] = "five";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
    TEST_ASSERT_NULL(packetbuf_add(&buf, NULL, BUFSIZE, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_two_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *exp_head, *data_ptr;

    TEST_ASSERT((exp_head = packetbuf_add(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, exp_head, 5));
    TEST_ASSERT_NOT_NULL(packetbuf_add(&buf, NULL, 65, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(5, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_three_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *exp_head, *data_ptr;

    TEST_ASSERT((exp_head = packetbuf_add(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, exp_head, 5));
    TEST_ASSERT_NOT_NULL(packetbuf_add(&buf, NULL, 65, PACKETBUF_DEFAULT));
    TEST_ASSERT_NOT_NULL(packetbuf_add(&buf, NULL, 117, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(5, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_to_NULL_NO_COPY(void)
{
    TEST_ASSERT_NULL(packetbuf_add(NULL, NULL, 0, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_NULL_NO_COPY(void)
{
    TEST_ASSERT_NULL(packetbuf_add(&buf, NULL, 0, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_one_len_0_NO_COPY(void)
{
    char data[] = "";
    TEST_ASSERT_NULL(packetbuf_add(&buf, data, 0, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_one_len_too_long_NO_COPY(void)
{
    char data[] = "";
    TEST_ASSERT_NULL(packetbuf_add(&buf, data, BUFSIZE+1, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_one_len_random_NO_COPY(void)
{
    char data[] = "five";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add(&buf, data, 5, PACKETBUF_NO_COPY))
            == &(data[0]));
}

static void test_packetbuf_add_two_len_too_long_NO_COPY(void)
{
    char data[] = "five";
    char data2[] = "";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add(&buf, data, 5, PACKETBUF_NO_COPY))
            == &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
    TEST_ASSERT_NULL(packetbuf_add(&buf, data2, BUFSIZE, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_two_len_random_NO_COPY(void)
{
    char data[] = "five";
    char data2[65];
    void *exp_head, *data_ptr;

    TEST_ASSERT((exp_head = packetbuf_add(&buf, data, 5, PACKETBUF_NO_COPY))
            == &(data[0]));
    TEST_ASSERT(packetbuf_add(&buf, data2, 65, PACKETBUF_NO_COPY) == &(data2[0]));
    TEST_ASSERT_EQUAL_INT(5, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_three_len_random_NO_COPY(void)
{
    char data[] = "five";
    char data2[65];
    char data3[117];
    void *exp_head, *data_ptr;

    TEST_ASSERT((exp_head = packetbuf_add(&buf, data, 5, PACKETBUF_NO_COPY))
            == &(data[0]));
    TEST_ASSERT(packetbuf_add(&buf, data2, 65, PACKETBUF_NO_COPY) == &(data2[0]));
    TEST_ASSERT(packetbuf_add(&buf, data3, 117, PACKETBUF_NO_COPY) == &(data3[0]));
    TEST_ASSERT_EQUAL_INT(5, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_high_priority_to_NULL_DEFAULT(void)
{
    TEST_ASSERT_NULL(packetbuf_add_high_priority(NULL, NULL, 0, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_high_priority_one_len_0_DEFAULT(void)
{
    TEST_ASSERT_NULL(packetbuf_add_high_priority(&buf, NULL, 0, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_high_priority_one_len_too_long_DEFAULT(void)
{
    TEST_ASSERT_NULL(packetbuf_add_high_priority(&buf, NULL, BUFSIZE+1, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_high_priority_one_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
}

static void test_packetbuf_add_high_priority_two_len_too_long_DEFAULT(void)
{
    char data[] = "five";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
    TEST_ASSERT_NULL(packetbuf_add_high_priority(&buf, NULL, BUFSIZE, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_high_priority_two_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *exp_head, *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
    TEST_ASSERT_NOT_NULL((exp_head = packetbuf_add_high_priority(&buf, NULL, 65, PACKETBUF_DEFAULT)));
    TEST_ASSERT_EQUAL_INT(65, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_high_priority_three_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *exp_head, *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
    TEST_ASSERT_NOT_NULL(packetbuf_add_high_priority(&buf, NULL, 65, PACKETBUF_DEFAULT));
    TEST_ASSERT_NOT_NULL((exp_head = packetbuf_add_high_priority(&buf, NULL, 117, PACKETBUF_DEFAULT)));
    TEST_ASSERT_EQUAL_INT(117, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_high_priority_to_NULL_NO_COPY(void)
{
    TEST_ASSERT_NULL(packetbuf_add_high_priority(NULL, NULL, 0, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_high_priority_NULL_NO_COPY(void)
{
    TEST_ASSERT_NULL(packetbuf_add_high_priority(&buf, NULL, 0, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_high_priority_one_len_0_NO_COPY(void)
{
    char data[] = "";
    TEST_ASSERT_NULL(packetbuf_add_high_priority(&buf, data, 0, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_add_high_priority_one_len_too_long_NO_COPY(void)
{
    char data[] = "";
    TEST_ASSERT_NULL(packetbuf_add_high_priority(&buf, data, BUFSIZE+1, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_high_priority_one_len_random_NO_COPY(void)
{
    char data[] = "five";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_NO_COPY))
            == &(data[0]));
}

static void test_packetbuf_add_high_priority_two_len_too_long_NO_COPY(void)
{
    char data[] = "five";
    char data2[] = "";
    void *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_NO_COPY))
            == &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
    TEST_ASSERT_NULL(packetbuf_add_high_priority(&buf, data2, BUFSIZE, PACKETBUF_NO_COPY));
    TEST_ASSERT_EQUAL_INT(ENOBUFS, errno);
}

static void test_packetbuf_add_high_priority_two_len_random_NO_COPY(void)
{
    char data[] = "five";
    char data2[65];
    void *exp_head, *data_ptr;

    TEST_ASSERT(packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_NO_COPY) == &(data[0]));
    TEST_ASSERT((exp_head = packetbuf_add_high_priority(&buf, data2, 65, PACKETBUF_NO_COPY))
            == &(data2[0]));
    TEST_ASSERT_EQUAL_INT(65, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_high_priority_three_len_random_NO_COPY(void)
{
    char data[] = "five";
    char data2[65];
    char data3[117];
    void *exp_head, *data_ptr;

    TEST_ASSERT(packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_NO_COPY) == &(data[0]));
    TEST_ASSERT(packetbuf_add_high_priority(&buf, data2, 65, PACKETBUF_NO_COPY) == &(data2[0]));
    TEST_ASSERT((exp_head = packetbuf_add_high_priority(&buf, data3, 117, PACKETBUF_NO_COPY))
            == &(data3[0]));
    TEST_ASSERT_EQUAL_INT(117, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_mixed_two_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *exp_head, *data_ptr;

    TEST_ASSERT((exp_head = packetbuf_add_high_priority(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, exp_head, 5));
    TEST_ASSERT_NOT_NULL(packetbuf_add(&buf, NULL, 65, PACKETBUF_DEFAULT));
    TEST_ASSERT_EQUAL_INT(5, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_add_mixed2_two_len_random_DEFAULT(void)
{
    char data[] = "five";
    void *exp_head, *data_ptr;

    TEST_ASSERT((data_ptr = packetbuf_add(&buf, data, 5, PACKETBUF_DEFAULT))
            != &(data[0]));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data_ptr, 5));
    TEST_ASSERT_NOT_NULL((exp_head = packetbuf_add_high_priority(&buf, NULL, 65, PACKETBUF_DEFAULT)));
    TEST_ASSERT_EQUAL_INT(65, packetbuf_get(&buf, &data_ptr))
    TEST_ASSERT(exp_head == data_ptr);
}

static void test_packetbuf_get_on_empty(void)
{
    void *data_ptr;

    TEST_ASSERT_EQUAL_INT(0, packetbuf_get(&buf, &data_ptr));
    TEST_ASSERT_EQUAL_INT(ENODATA, errno);
}

static void test_packetbuf_get_buf_is_NULL(void)
{
    void *data_ptr;

    TEST_ASSERT_EQUAL_INT(0, packetbuf_get(NULL, &data_ptr));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_get_packet_data_is_NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, packetbuf_get(&buf, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_get_both_NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, packetbuf_get(NULL, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_remove_on_empty(void)
{
    void *data_ptr;

    TEST_ASSERT_EQUAL_INT(0, packetbuf_remove(&buf, &data_ptr));
    TEST_ASSERT_EQUAL_INT(ENODATA, errno);
}

static void test_packetbuf_remove_buf_is_NULL(void)
{
    void *data_ptr;

    TEST_ASSERT_EQUAL_INT(0, packetbuf_remove(NULL, &data_ptr));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_remove_packet_data_is_NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, packetbuf_remove(&buf, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_remove_both_NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, packetbuf_remove(NULL, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_remove_one_random(void)
{
    char data[] = "five";
    char data_out[8];
    char *data_ptr = &data_out[0];

    packetbuf_add(&buf, &data, 5, PACKETBUF_DEFAULT);

    TEST_ASSERT_EQUAL_INT(5, packetbuf_remove(&buf, data_ptr));
    TEST_ASSERT_EQUAL_STRING("five", data_ptr);
    TEST_ASSERT_EQUAL_INT(0, packetbuf_remove(&buf, data_ptr));
    TEST_ASSERT_EQUAL_INT(ENODATA, errno);
}

static void test_packetbuf_remove_two_random(void)
{
    char data_out[8];
    char *data_ptr = &data_out[0];

    packetbuf_add(&buf, "five", 5, PACKETBUF_DEFAULT);
    packetbuf_add(&buf, "for", 4, PACKETBUF_DEFAULT);

    TEST_ASSERT_EQUAL_INT(5, packetbuf_remove(&buf, data_ptr));
    TEST_ASSERT_EQUAL_STRING("five", data_ptr);
    TEST_ASSERT_EQUAL_INT(4, packetbuf_remove(&buf, data_ptr));
    TEST_ASSERT_EQUAL_STRING("for", data_ptr);
    TEST_ASSERT_EQUAL_INT(0, packetbuf_remove(&buf, data_ptr));
    TEST_ASSERT_EQUAL_INT(ENODATA, errno);
}

static void test_packetbuf_move_right_on_empty(void)
{
    TEST_ASSERT_NULL(packetbuf_move_right(&buf, "something", 4));
    TEST_ASSERT_EQUAL_INT(EADDRNOTAVAIL, errno);
}

static void test_packetbuf_move_right_buf_is_NULL(void)
{
    TEST_ASSERT_NULL(packetbuf_move_right(NULL, "something", 4));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_move_right_packet_is_NULL(void)
{
    TEST_ASSERT_NULL(packetbuf_move_right(&buf, NULL, 0));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_move_right_both_NULL(void)
{
    TEST_ASSERT_NULL(packetbuf_move_right(NULL, NULL, 0));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_move_right_offset_too_large(void)
{
    char *data_out;

    data_out = packetbuf_add(&buf, "something", sizeof("something"),
                             PACKETBUF_DEFAULT);

    TEST_ASSERT_NULL(packetbuf_move_right(&buf, data_out, 100));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

static void test_packetbuf_move_right_random_offset(void)
{
    char *data_out, *data_check;

    data_out = packetbuf_add(&buf, "something", sizeof("something"),
                             PACKETBUF_DEFAULT);

    TEST_ASSERT_NOT_NULL((data_check = packetbuf_move_right(&buf, data_out, 5)));
    TEST_ASSERT(data_check == data_out + 5);
    TEST_ASSERT_EQUAL_STRING("hing", data_check);
}

Test *tests_packetbuf_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_packetbuf_add_to_NULL_DEFAULT),
        new_TestFixture(test_packetbuf_add_one_len_0_DEFAULT),
        new_TestFixture(test_packetbuf_add_one_len_too_long_DEFAULT),
        new_TestFixture(test_packetbuf_add_one_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_add_two_len_too_long_DEFAULT),
        new_TestFixture(test_packetbuf_add_two_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_add_three_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_add_to_NULL_NO_COPY),
        new_TestFixture(test_packetbuf_add_NULL_NO_COPY),
        new_TestFixture(test_packetbuf_add_one_len_0_NO_COPY),
        new_TestFixture(test_packetbuf_add_one_len_too_long_NO_COPY),
        new_TestFixture(test_packetbuf_add_one_len_random_NO_COPY),
        new_TestFixture(test_packetbuf_add_two_len_too_long_NO_COPY),
        new_TestFixture(test_packetbuf_add_two_len_random_NO_COPY),
        new_TestFixture(test_packetbuf_add_three_len_random_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_to_NULL_DEFAULT),
        new_TestFixture(test_packetbuf_add_high_priority_one_len_0_DEFAULT),
        new_TestFixture(test_packetbuf_add_high_priority_one_len_too_long_DEFAULT),
        new_TestFixture(test_packetbuf_add_high_priority_one_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_add_high_priority_two_len_too_long_DEFAULT),
        new_TestFixture(test_packetbuf_add_high_priority_two_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_add_high_priority_three_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_add_high_priority_to_NULL_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_NULL_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_one_len_0_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_one_len_too_long_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_one_len_random_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_two_len_too_long_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_two_len_random_NO_COPY),
        new_TestFixture(test_packetbuf_add_high_priority_three_len_random_NO_COPY),
        new_TestFixture(test_packetbuf_add_mixed_two_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_add_mixed2_two_len_random_DEFAULT),
        new_TestFixture(test_packetbuf_get_on_empty),
        new_TestFixture(test_packetbuf_get_buf_is_NULL),
        new_TestFixture(test_packetbuf_get_packet_data_is_NULL),
        new_TestFixture(test_packetbuf_get_both_NULL),
        new_TestFixture(test_packetbuf_remove_on_empty),
        new_TestFixture(test_packetbuf_remove_buf_is_NULL),
        new_TestFixture(test_packetbuf_remove_packet_data_is_NULL),
        new_TestFixture(test_packetbuf_remove_both_NULL),
        new_TestFixture(test_packetbuf_remove_one_random),
        new_TestFixture(test_packetbuf_remove_two_random),
        new_TestFixture(test_packetbuf_move_right_on_empty),
        new_TestFixture(test_packetbuf_move_right_buf_is_NULL),
        new_TestFixture(test_packetbuf_move_right_packet_is_NULL),
        new_TestFixture(test_packetbuf_move_right_both_NULL),
        new_TestFixture(test_packetbuf_move_right_offset_too_large),
        new_TestFixture(test_packetbuf_move_right_random_offset),
    };

    EMB_UNIT_TESTCALLER(sys_lib_ringbuffer_tests, set_up, NULL,
                        fixtures);

    return (Test *)&sys_lib_ringbuffer_tests;
}

void tests_packetbuf(void)
{
    TESTS_RUN(tests_packetbuf_tests());
}
