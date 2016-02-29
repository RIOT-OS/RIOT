/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "mbox.h"
#include "sema.h"
#include "thread.h"
#include "xtimer.h"

#define ASSERT(x)   if (!(x)) { \
        printf("%s:%d: ASSERTION (" # x ") FAILED\n", RIOT_FILE_RELATIVE, \
               __LINE__); \
        core_panic(PANIC_ASSERT_FAIL, assert_crash_message); \
}


#define MBOX_QUEUE_SIZE     (8)
#define THREAD_PRIO         (THREAD_PRIORITY_MAIN)
#define MSG_TYPE            (0xf212)

static kernel_pid_t thread_pid, main_pid;
static char thread_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
static sema_t sync = SEMA_CREATE(0);    /* Synchronisation semaphore */
static msg_t mbox_queue[MBOX_QUEUE_SIZE];
static mbox_t mbox = MBOX_CREATE(mbox_queue, MBOX_QUEUE_SIZE);

static void test_put1(void)
{
    msg_t m;
    int res;

    m.type = MSG_TYPE;
    m.content.value = 1234U;
    res = mbox_put(&mbox, &m);
    ASSERT(res == 0);
    printf("%" PRIu16 ": test_put1: mbox_put() => mbox_get() passed\n",
           sched_active_thread->pid);
}

static void test_get1(void)
{
    msg_t m;
    int res = mbox_get(&mbox, &m);

    ASSERT((res == 0) && (m.type == MSG_TYPE) && (m.content.value == 1234U));
    printf("%" PRIu16 ": test_get1: mbox_put() => mbox_get() passed\n",
           sched_active_thread->pid);
}

static void test_put2(void)
{
    mbox_destroy(&mbox);
    printf("%" PRIu16 ": test_put2: destroy message box\n", sched_active_thread->pid);
}

static void test_get2(void)
{
    msg_t m;
    int res = mbox_get(&mbox, &m);

    ASSERT(res == -ECANCELED);
    printf("%" PRIu16 ": test_get2: mbox_get() message box was destroyed\n",
           sched_active_thread->pid);
}

static void test_put3(void)
{
    msg_t m;

    m.type = MSG_TYPE;
    for (uint32_t i = 0; i < (MBOX_QUEUE_SIZE * 2); i++) {
        int res;
        m.content.value = i;
        res = mbox_put(&mbox, &m);
        ASSERT(res == 0);
        printf("%" PRIu16 ": test_put3: mbox_put() (i == %" PRIu32 ") => mbox_get() passed\n",
               sched_active_thread->pid, i);
    }
}

static void test_get3(void)
{
    for (uint32_t i = 0; i < (MBOX_QUEUE_SIZE * 2); i++) {
        msg_t m;
        int res = mbox_get(&mbox, &m);
        ASSERT((res == 0) && (m.type == MSG_TYPE) && (m.content.value == i));
        printf("%" PRIu16 ": test_get3: mbox_put() => mbox_get() (i == %" PRIu32 ") passed\n",
               sched_active_thread->pid, i);
    }
    ASSERT(mbox_is_empty(&mbox));
    printf("%" PRIu16 ": test_get3: mbox is empty as expected\n", sched_active_thread->pid);
}

static void test_put4(void)
{
    msg_t m;

    m.type = MSG_TYPE;
    for (uint32_t i = 0; i < MBOX_QUEUE_SIZE + 1; i++) {
        int res;
        m.content.value = i;
        res = mbox_tryput(&mbox, &m);
        if (res == 0) {
            printf("%" PRIu16 ": test_put4: mbox_tryput() (i == %" PRIu32 ") => mbox_get() passed\n",
                   sched_active_thread->pid, i);
        }
        else if (res == -EAGAIN) {
            ASSERT(mbox_is_full(&mbox));
            printf("%" PRIu16 ": test_put4: mbox_tryput() (i == %"
                   PRIu32 ") failed with EAGAIN as expected\n",
                   sched_active_thread->pid, i);
            break;
        }
        else {
            printf("%" PRIu16 ": test_put4: unexpected behavior\n", sched_active_thread->pid);
            ASSERT(false);
        }
    }
}

static void test_get4(void)
{
    for (uint32_t i = 0; i < MBOX_QUEUE_SIZE; i++) {
        msg_t m;
        int res = mbox_get(&mbox, &m);
        ASSERT((res == 0) && (m.type == MSG_TYPE) && (m.content.value == i));
        printf("%" PRIu16 ": test_get4: mbox_tryput() => mbox_get() (i == %" PRIu32 ") passed\n",
               sched_active_thread->pid, i);
    }
    ASSERT(mbox_is_empty(&mbox));
    printf("%" PRIu16 ": test_get4: mbox is empty as expected\n", sched_active_thread->pid);
}

static void test_put5(void)
{
    msg_t m;
    int res;

    m.type = MSG_TYPE;
    m.content.value = 1234U;
    res = mbox_put(&mbox, &m);
    ASSERT(res == 0);
    printf("%" PRIu16 ": test_put5: mbox_put() => mbox_tryget() passed\n",
           sched_active_thread->pid);
}

static void test_get5(void)
{
    msg_t m;
    int res = mbox_tryget(&mbox, &m);

    ASSERT((res == 0) && (m.type == MSG_TYPE) && (m.content.value == 1234U));
    printf("%" PRIu16 ": test_get5: mbox_put() => mbox_tryget() passed\n",
           sched_active_thread->pid);
    res = mbox_tryget(&mbox, &m);
    ASSERT(res == -EAGAIN);
    printf("%" PRIu16 ": test_get5: second mbox_tryget() failed as expected\n",
           sched_active_thread->pid);
}

static void test_put6(void)
{
    msg_t m;
    int res;

    m.type = MSG_TYPE;
    m.content.value = 1234U;
    res = mbox_put(&mbox, &m);
    ASSERT(res == 0);
    printf("%" PRIu16 ": test_put6: mbox_put() => mbox_get_timed() passed\n",
           sched_active_thread->pid);
}

static void test_get6(void)
{
    msg_t m;
    uint32_t time;
    int res;

    time = xtimer_now();
    res = mbox_get_timed(&mbox, &m, 100);
    time = xtimer_now() - time;
    ASSERT((res == 0) && (m.type == MSG_TYPE) && (m.content.value == 1234U) &&
           (time < 100));
    printf("%" PRIu16 ": test_get6: mbox_put() => mbox_get_timed() passed\n",
           sched_active_thread->pid);
}

static void test_put7(void)
{
    printf("%" PRIu16 ": test_put7: waiting for getter to time out\n",
           sched_active_thread->pid);
}

static void test_get7(void)
{
    msg_t m;
    uint32_t time;
    int res;

    time = xtimer_now();
    res = mbox_get_timed(&mbox, &m, 100);
    time = xtimer_now() - time;
    ASSERT((res == -ETIMEDOUT) && (time >= 100));
    printf("%" PRIu16 ": test_get7: mbox_put() => mbox_get_timed() timed out\n",
           sched_active_thread->pid);
}

static void test_put(void)
{
    test_put1();
    sema_wait(&sync);
    test_put2();
    sema_wait(&sync);
    /* was destroyed in test_put2() */
    mbox_create(&mbox, mbox_queue, MBOX_QUEUE_SIZE);
    printf("%" PRIu16 ": create new message box\n", sched_active_thread->pid);
    test_put3();
    sema_wait(&sync);
    test_put4();
    sema_wait(&sync);
    test_put5();
    sema_wait(&sync);
    test_put6();
    sema_wait(&sync);
    test_put7();
    printf("%" PRIu16 ": put tests successful\n", sched_active_thread->pid);
    sema_wait(&sync);
}

static void test_get(void)
{
    test_get1();
    sema_post(&sync);
    test_get2();
    sema_post(&sync);
    test_get3();
    sema_post(&sync);
    test_get4();
    sema_post(&sync);
    test_get5();
    sema_post(&sync);
    test_get6();
    sema_post(&sync);
    test_get7();
    printf("%" PRIu16 ": get tests successful\n", sched_active_thread->pid);
    sema_post(&sync);
}

static void *thread(void *arg)
{
    (void)arg;
    test_get();
    return NULL;
}

int main(void)
{
    xtimer_init();
    main_pid = sched_active_pid;
    thread_pid = thread_create(thread_stack, sizeof(thread_stack), THREAD_PRIO + 1,
                               THREAD_CREATE_STACKTEST, thread, NULL, "thread");
    ASSERT(thread_pid > KERNEL_PID_UNDEF);
    test_put();
    puts("All tests successful");
    return 0;
}

/** @} */
