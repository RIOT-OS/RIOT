/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "thread.h"
#include "flags.h"
#include "kernel.h"
#include "ringbuffer.h"
#include "mutex.h"

#include "tests-lib.h"

/* (ITERATIONS * (BUF_SIZE + 1)) needs to be <= 127! Otherwise `char` overflows. */
#define ITERATIONS 15
#define BUF_SIZE 7

static char stack_get[KERNEL_CONF_STACKSIZE_DEFAULT];

static char rb_buf[BUF_SIZE];
static ringbuffer_t rb = RINGBUFFER_INIT(rb_buf);
static mutex_t mutex;
static unsigned pid_add, pid_get;

static void assert_avail(unsigned assumed)
{
    TEST_ASSERT_EQUAL_INT(assumed, rb.avail);
}

static void assert_add_one(char to_add, int assumed_result)
{
    int actual_result = ringbuffer_add_one(&rb, to_add);
    TEST_ASSERT_EQUAL_INT(assumed_result, actual_result);
}

static void assert_get_one(int assumed_result)
{
    int actual_result = ringbuffer_get_one(&rb);
    TEST_ASSERT_EQUAL_INT(assumed_result, actual_result);
}

static void run_add(void)
{
    char next = 0;
    for (unsigned iteration = 0; iteration < ITERATIONS; ++iteration) {
        mutex_lock(&mutex);

        for (unsigned i = 0; i < BUF_SIZE; ++i) {
            assert_avail(i);
            assert_add_one(next, -1);
            assert_avail(i + 1);
            ++next;
        }

        /* Overwrite oldest element. It should be returned to us. */
        assert_avail(BUF_SIZE);
        assert_add_one(next, next - BUF_SIZE);
        assert_avail(BUF_SIZE);
        ++next;

        thread_wakeup(pid_get);
        mutex_unlock_and_sleep(&mutex);
    }

    thread_wakeup(pid_get);
}

static void *run_get(void *arg)
{
    (void) arg;

    char next = 0;
    for (unsigned iteration = 0; iteration < ITERATIONS; ++iteration) {
        ++next; /* the first element of a stride is always overwritten */

        mutex_lock(&mutex);

        for (unsigned i = BUF_SIZE; i > 0; --i) {
            assert_avail(i);
            assert_get_one(next);
            assert_avail(i - 1);
            ++next;
        }

        assert_avail(0);
        assert_get_one(-1);
        assert_avail(0);

        thread_wakeup(pid_add);
        mutex_unlock_and_sleep(&mutex);
    }

    return NULL;
}

static void tests_lib_ringbuffer(void)
{
    pid_add = sched_active_pid;
    pid_get = thread_create(stack_get, sizeof (stack_get),
                            PRIORITY_MAIN, CREATE_SLEEPING | CREATE_STACKTEST,
                            run_get, NULL, "get");
    run_add();
}

Test *tests_lib_ringbuffer_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_lib_ringbuffer),
    };

    EMB_UNIT_TESTCALLER(ringbuffer_tests, NULL, NULL, fixtures);

    return (Test *)&ringbuffer_tests;
}
