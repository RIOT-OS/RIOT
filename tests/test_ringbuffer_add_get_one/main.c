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

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test ringbuffer_(add|get)_one().
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "flags.h"
#include "kernel.h"
#include "ringbuffer.h"
#include "mutex.h"

#define STACK_SIZE (KERNEL_CONF_STACKSIZE_DEFAULT + KERNEL_CONF_STACKSIZE_PRINTF)

/* (ITERATIONS * (BUF_SIZE + 1)) needs to be <= 127! Otherwise `char` overflows. */
#define ITERATIONS 15
#define BUF_SIZE 7

static char stack_add[STACK_SIZE], stack_get[STACK_SIZE];

static char rb_buf[BUF_SIZE];
static ringbuffer_t rb = RINGBUFFER_INIT(rb_buf);
static mutex_t mutex;
static unsigned pid_add, pid_get;

static void assert_avail(unsigned assumed)
{
    unsigned avail = rb.avail;
    if (avail != assumed) {
        printf("ERROR: %s, rb->avail == (actual) %u != (assumed) %u\n", sched_active_thread->name, avail, assumed);
        thread_sleep();
    }
}

static void assert_add_one(char to_add, int assumed_result)
{
    int actual_result = ringbuffer_add_one(&rb, to_add);
    if (actual_result != assumed_result) {
        printf("ERROR: %s: (actual) %i != (assumed) %i\n", __func__, actual_result, assumed_result);
        thread_sleep();
    }
}

static void assert_get_one(int assumed_result)
{
    int actual_result = ringbuffer_get_one(&rb);
    if (actual_result != assumed_result) {
        printf("ERROR: %s: (actual) %i != (assumed) %i\n", __func__, actual_result, assumed_result);
        thread_sleep();
    }
}

static void run_add(void)
{
    puts("Start run_add.");
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
    puts("Done run_add.");

    thread_wakeup(pid_get);
}

static void run_get(void)
{
    puts("Start run_get.");
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
    puts("Done run_get.");
}

int main(void)
{
    puts("Start main.");
    pid_add = thread_create(stack_add, sizeof (stack_add), PRIORITY_MAIN, CREATE_SLEEPING | CREATE_STACKTEST, run_add, "add");
    pid_get = thread_create(stack_get, sizeof (stack_get), PRIORITY_MAIN, CREATE_SLEEPING | CREATE_STACKTEST, run_get, "get");
    puts("Done main.");
    thread_wakeup(pid_add);
    return 0;
}
