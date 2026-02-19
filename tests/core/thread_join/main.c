/*
 * Copyright (C) 2025 Mihai Renea <mihai.renea@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Thread join test application
 *
 * @author  Mihai Renea <mihai.renea@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "test_utils/expect.h"
#include "thread.h"
#include "ztimer.h"

static void *joinee_f(void *arg)
{
    (void)arg;
    ztimer_sleep(ZTIMER_MSEC, 100);

    return NULL;
}

static kernel_pid_t _spawn_joinee(bool joinable)
{
    static char joinee_stack[THREAD_STACKSIZE_MAIN];
    return thread_create(joinee_stack, sizeof(joinee_stack),
                         THREAD_PRIORITY_MAIN,
                         joinable ? THREAD_CREATE_JOINABLE : 0,
                         joinee_f,
                         NULL,
                         "joinee");
}

int main(void)
{
    puts("START");

    kernel_pid_t joinee_pid = _spawn_joinee(true);
    expect(joinee_pid >= 0);

    int res = thread_join(joinee_pid, NULL);
    expect(res == 0);
    expect(thread_get(joinee_pid) == NULL);

    joinee_pid = _spawn_joinee(true);
    expect(joinee_pid >= 0);

    ztimer_sleep(ZTIMER_MSEC, 150);

    thread_t *joinee = thread_get(joinee_pid);
    expect(joinee && joinee->status == STATUS_ZOMBIE);

    res = thread_join(joinee_pid, NULL);
    expect(res == 0);
    expect(thread_get(joinee_pid) == NULL);

    joinee_pid = _spawn_joinee(false);

    res = thread_join(joinee_pid, NULL);
    expect(res == -EINVAL);

    puts("SUCCESS");

    return 0;
}
