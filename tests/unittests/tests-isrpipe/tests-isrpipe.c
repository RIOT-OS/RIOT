/*
 * SPDX-FileCopyrightText: 2025 Prime Controls, Inc.(R)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>

#include "embUnit/embUnit.h"

#include "isrpipe.h"
#include "tests-isrpipe.h"

#include "ztimer.h"

/* ISR pipe to test */
static uint8_t _buffer[8];
static isrpipe_t _pipe;

/* reader thread's stack */
static char _reader_stack[THREAD_STACKSIZE_TINY];

/* entry point of reader thread */
static void* _reader_thread(void *arg)
{
    uint8_t *read_buf = arg;

    /* get the size of the read_buffer */
    unsigned read_buf_size = read_buf[0];

    /* keep reading until read_buf is full */
    while (read_buf_size) {
        const int res = isrpipe_read(&_pipe, read_buf, read_buf_size);
        read_buf += res;
        read_buf_size -= res;
    }

    thread_zombify();
    return NULL;
}

static void setup_up(void)
{
    /* clear buffer contents from previous tests */
    memset(_buffer, 0, sizeof(_buffer));

    /* prep the pipe */
    isrpipe_init(&_pipe, _buffer, ARRAY_SIZE(_buffer));
}

static void test_init(void)
{
    /* statically initialized isrpipe */
    static const isrpipe_t pipe = ISRPIPE_INIT(_buffer);

    /* check that static and runtime init produces the same result */
    TEST_ASSERT_EQUAL_INT(0, memcmp(&_pipe, &pipe, sizeof(isrpipe_t)));
}

static void test_write_then_read(void)
{
    const uint8_t write_buf[] = {1, 2, 3, 4, 5, 6};
    uint8_t read_buf[2];
    int res;

    res = isrpipe_write(&_pipe, write_buf, ARRAY_SIZE(write_buf));
    TEST_ASSERT_EQUAL_INT(ARRAY_SIZE(write_buf), res);

    /* pipe now holds 1, 2, 3, 4, 5, 6 */

    res = isrpipe_read(&_pipe, read_buf, 1);
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_INT(1, read_buf[0]);

    /* pipe now holds 2, 3, 4, 5, 6 */

    res = isrpipe_read(&_pipe, read_buf, 2);
    TEST_ASSERT_EQUAL_INT(2, res);
    TEST_ASSERT_EQUAL_INT(2, read_buf[0]);
    TEST_ASSERT_EQUAL_INT(3, read_buf[1]);

    /* pipe now holds 4, 5, 6 */

    res = isrpipe_read(&_pipe, read_buf, 1);
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_INT(4, read_buf[0]);

    /* pipe now holds 5, 6 */

    res = isrpipe_read(&_pipe, read_buf, 2);
    TEST_ASSERT_EQUAL_INT(2, res);
    TEST_ASSERT_EQUAL_INT(5, read_buf[0]);
    TEST_ASSERT_EQUAL_INT(6, read_buf[1]);

    /* pipe now empty */
}

static void test_read_then_write(void)
{
    uint8_t read_buf[3] = {0}; /* init contents to all zeros */

    thread_t *writer_thread = thread_get_active();
    const unsigned writer_priority = thread_get_priority(writer_thread);
    const unsigned reader_priority = 0;

    /* check assumptions */
    TEST_ASSERT(writer_priority > reader_priority);

    /* sneaky way to tell reader thread the size of the read_buf */
    read_buf[0] = ARRAY_SIZE(read_buf);

    /* create reader thread with priority higher than writer thread so that write
       can only happen once reading blocks on the empty pipe */
    const int reader_pid = thread_create(_reader_stack, sizeof(_reader_stack),
        reader_priority, 0, _reader_thread, read_buf, "reader");

    /* check assumptions */
    TEST_ASSERT(reader_pid >= 0);

    /* reader thread should have preempted us, but is now blocked by reading
       from the empty pipe, which yielded the CPU back to us */

    /* write to the pipe a byte at a time to demonstrate that the reader is
       unblocking with each byte and placing the bytes into read_buf */
    for (unsigned i = 0; i < ARRAY_SIZE(read_buf); i++) {

        /* test that read_buf[i] is not equal to the value we are about to send
           through the pipe */
        TEST_ASSERT(i != read_buf[i]);

        /* write a byte, reader thread will then unblock and write it into
           read_buf[i] */
        TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, i));

        /* check that reader got the byte from the pipe and put it into
           read_buf[i]*/
        TEST_ASSERT_EQUAL_INT(i, read_buf[i]);
    }

    /* cleanup */
    TEST_ASSERT_EQUAL_INT(1, thread_kill_zombie(reader_pid));
}

static void test_overflow(void)
{
    int res;

    /* completely fill the pipe */
    for (unsigned i = 0; i < ARRAY_SIZE(_buffer); i++) {
        TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, i));
    }

    /* pipe is now full */

    /* isrpipe_write_one() should return -1 for error */
    TEST_ASSERT_EQUAL_INT(-1, isrpipe_write_one(&_pipe, -1));

    /* isrpipe_write() should return 0 bytes written */
    const uint8_t write_buf[] = {1, 2, 3};
    res = isrpipe_write(&_pipe, write_buf, ARRAY_SIZE(write_buf));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_underflow(void)
{
    const uint8_t write_buf[] = {1, 2, 3};
    uint8_t read_buf[ARRAY_SIZE(write_buf) + 1];
    int res;

    res = isrpipe_write(&_pipe, write_buf, ARRAY_SIZE(write_buf));
    TEST_ASSERT_EQUAL_INT(ARRAY_SIZE(write_buf), res);

    /* pipe now holds 1, 2, 3 */

    /* try to read more data than is in pipe */
    res = isrpipe_read(&_pipe, read_buf, ARRAY_SIZE(read_buf));
    TEST_ASSERT_EQUAL_INT(ARRAY_SIZE(write_buf), res);
}

static void test_write_one(void)
{
    uint8_t read_buf[3];
    int res;

    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, 1));
    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, 2));
    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, 3));

    /* pipe now holds 1, 2, 3 */

    TEST_ASSERT_EQUAL_INT(1, isrpipe_read(&_pipe, read_buf, 1));
    TEST_ASSERT_EQUAL_INT(1, read_buf[0]);

    /* pipe now holds 2, 3 */

    TEST_ASSERT_EQUAL_INT(1, isrpipe_read(&_pipe, read_buf, 1));
    TEST_ASSERT_EQUAL_INT(2, read_buf[0]);

    /* pipe now holds 3 */

    TEST_ASSERT_EQUAL_INT(1, isrpipe_read(&_pipe, read_buf, 1));
    TEST_ASSERT_EQUAL_INT(3, read_buf[0]);

    /* pipe now empty */

    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, 4));
    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, 5));
    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&_pipe, 6));

    /* pipe now holds 4, 5, 6 */

    res = isrpipe_read(&_pipe, read_buf, ARRAY_SIZE(read_buf));
    TEST_ASSERT_EQUAL_INT(ARRAY_SIZE(read_buf), res);
    TEST_ASSERT_EQUAL_INT(4, read_buf[0]);
    TEST_ASSERT_EQUAL_INT(5, read_buf[1]);
    TEST_ASSERT_EQUAL_INT(6, read_buf[2]);

    /* pipe now empty */
}

Test *tests_isrpipe_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_init),
        new_TestFixture(test_write_then_read),
        new_TestFixture(test_read_then_write),
        new_TestFixture(test_overflow),
        new_TestFixture(test_underflow),
        new_TestFixture(test_write_one),
    };

    EMB_UNIT_TESTCALLER(isrpipe_tests, setup_up, NULL, fixtures);

    return (Test *)&isrpipe_tests;
}

void tests_isrpipe(void)
{
    TESTS_RUN(tests_isrpipe_tests());
    TESTS_RUN(tests_isrpipe_read_timeout_tests());
}
