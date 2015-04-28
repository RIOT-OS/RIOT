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

#include "tests-ubjson.h"

#include "thread.h"
#include "sched.h"
#include "msg.h"
#include "mutex.h"
#include "pipe.h"
#include "irq.h"

static pipe_t communication_pipe;
static ringbuffer_t pipe_rb;
static char pipe_buffer[16];

static char receiver_stack[THREAD_STACKSIZE_DEFAULT];

typedef struct {
    void (*run)(void);
    tcb_t *main_thread;
    mutex_t mutexes[2];
} test_ubjson_receiver_data_t;

static void ubjson_set_up(void)
{
    ringbuffer_init(&pipe_rb, pipe_buffer, sizeof(pipe_buffer));
    pipe_init(&communication_pipe, &pipe_rb, NULL);
}

ssize_t test_ubjson_write_fun(ubjson_cookie_t *restrict cookie, const void *buf, size_t len)
{
    (void) cookie;
    ssize_t total = 0;
    while (total < (ssize_t) len) {
        ssize_t subtotal = pipe_write(&communication_pipe, buf, len);
        if (subtotal < 0) {
            return subtotal;
        }
        total += subtotal;
    }
    return total;
}

ssize_t test_ubjson_read_fun(ubjson_cookie_t *restrict cookie, void *buf, size_t len)
{
    (void) cookie;
    return pipe_read(&communication_pipe, buf, len);
}

static void *test_ubjson_receiver_trampoline(void *arg)
{
    test_ubjson_receiver_data_t *data = arg;
    data->run();

    mutex_unlock(&data->mutexes[0]);
    mutex_lock(&data->mutexes[1]);

    disableIRQ();
    sched_set_status(data->main_thread, STATUS_PENDING);
    return NULL;
}

void test_ubjson_test(void (*sender_fun)(void), void (*receiver_fun)(void))
{
    test_ubjson_receiver_data_t data = {
        .run = receiver_fun,
        .main_thread = (tcb_t *) sched_active_thread,
        .mutexes = {
            { 1, PRIORITY_QUEUE_INIT },
            { 1, PRIORITY_QUEUE_INIT },
        },
    };
    kernel_pid_t receiver_pid = thread_create(receiver_stack, sizeof(receiver_stack),
                                              THREAD_PRIORITY_MAIN, CREATE_WOUT_YIELD,
                                              test_ubjson_receiver_trampoline, &data, "receiver");
    TEST_ASSERT(pid_is_valid(receiver_pid));

    sender_fun();

    mutex_lock(&data.mutexes[0]);
    mutex_unlock_and_sleep(&data.mutexes[1]);
}

static Test *tests_ubjson_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ubjson_empty_array),
        new_TestFixture(test_ubjson_empty_object),
    };

    EMB_UNIT_TESTCALLER(ubjson_tests, ubjson_set_up, NULL, fixtures);

    return (Test *) &ubjson_tests;
}

void tests_ubjson(void)
{
    TESTS_RUN(tests_ubjson_tests());
}
