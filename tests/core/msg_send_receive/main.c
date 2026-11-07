/*
 * SPDX-FileCopyrightText: 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test msg_send_receive().
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "cpu_conf.h"
#include "mutex.h"
#include "thread.h"

#define THREAD1_STACKSIZE   (THREAD_STACKSIZE_MAIN)
#define THREAD2_STACKSIZE   (THREAD_STACKSIZE_MAIN)

#ifndef TEST_EXECUTION_NUM
#define TEST_EXECUTION_NUM  (10)
#endif

static char thread1_stack[THREAD1_STACKSIZE];
static char thread2_stack[THREAD2_STACKSIZE];

static kernel_pid_t thread1_pid, thread2_pid;

static int counter1 = 0;
static int counter2 = 0;

static mutex_t _mtx = MUTEX_INIT_LOCKED;

static void *thread1(void *args)
{
    (void)args;

    msg_t msg_req, msg_resp;
    int success = 1;

    msg_resp.content.ptr = NULL;
    msg_req.content.ptr = &counter1;

    for (int i = 0; i < TEST_EXECUTION_NUM; i++) {
        msg_send_receive(&msg_req, &msg_resp, thread2_pid);

        if ((NULL == msg_resp.content.ptr) ||
            (&counter1 != ((int *) msg_req.content.ptr)) ||
            (counter1 != (*(int *) msg_resp.content.ptr)) ||
            (counter1 != (*(int *) msg_req.content.ptr))) {
            success = 0;
            break;
        }
    }

    if (success) {
        mutex_unlock(&_mtx);
    }
    else {
        puts("Test failed.");
    }

    return NULL;
}

static void *thread2(void *args)
{
    (void)args;

    msg_t msg_req, msg_resp;

    msg_resp.content.ptr = &counter2;

    for (int i = 0; i < TEST_EXECUTION_NUM; i++) {
        msg_receive(&msg_req);

        ++*(int *) msg_req.content.ptr;
        ++*(int *) msg_resp.content.ptr;
        printf("Incremented counters to %d and %d\n",
               *(int *) msg_req.content.ptr, *(int *) msg_resp.content.ptr);

        msg_reply(&msg_req, &msg_resp);
    }

    return NULL;
}

int main(void)
{
    msg_t msg = { 0 };

    /* Test lower PID boundary */
    int res = msg_send_receive(&msg, &msg, KERNEL_PID_FIRST - 1);
    if (res != -1) {
        puts("msg_send_receive() did not return -1 for invalid PID.");
        return -1;
    }

    /* Test upper PID boundary */
    res = msg_send_receive(&msg, &msg, KERNEL_PID_LAST + 1);
    if (res != -1) {
        puts("msg_send_receive() did not return -1 for invalid PID.");
        return -1;
    }

    /* Check that this thread was not put to @ref STATUS_REPLY_BLOCKED by accident */
    thread_yield();

    thread2_pid = thread_create(thread2_stack, THREAD2_STACKSIZE, THREAD_PRIORITY_MAIN - 2,
                                0, thread2, NULL, "thread2");
    thread1_pid = thread_create(thread1_stack, THREAD1_STACKSIZE, THREAD_PRIORITY_MAIN - 1,
                                0, thread1, NULL, "thread1");

    /* Wait for thread1 to unlock the mutex on success */
    mutex_lock(&_mtx);

    /* Test PID of stopped thread */
    res = msg_send_receive(&msg, &msg, thread1_pid);
    if (res != -1) {
        puts("msg_send_receive() did not return -1 for invalid PID.");
        return -1;
    }

    /* Check that this thread was not put to @ref STATUS_REPLY_BLOCKED by accident */
    thread_yield();

    puts("Test successful.");

    return 0;
}
