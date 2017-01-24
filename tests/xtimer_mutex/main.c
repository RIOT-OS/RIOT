/*
 * Copyright (C) 2017 TriaGnoSys GmbH
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
 * @brief xtimer_mutex test application
 *
 * @author      Víctor Ariño <victor.arino@zii.aero>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "xtimer.h"
#include "thread.h"
#include "msg.h"
#include "fmt.h"

char thread1_stack[THREAD_STACKSIZE_MAIN];
char thread2_stack[THREAD_STACKSIZE_MAIN];

static mutex_t mutex = MUTEX_INIT;

static kernel_pid_t mainpid, pid, pid2;

static int tx_lock(int tx, uint64_t timeout)
{
    printf("thread%d timeout: ", tx);
    print_u64_dec(timeout);
    printf(", mutex: ");
    if (mutex.queue.next == NULL) {
        printf("unlocked");
    }
    else if (mutex.queue.next == MUTEX_LOCKED) {
        printf("locked (1)");
    }
    else {
        printf("locked (2+)");
    }
    printf(" --> ");
    return xtimer_mutex_lock_timeout(&mutex, timeout);
}

static void tx_lock_eq(int tx, uint64_t timeout)
{
    if (tx_lock(tx, timeout) == 0) {
        printf("OK\n");
    }
    else {
        printf("FAIL\n");
    }
}

static void tx_lock_lt(int tx, uint64_t timeout)
{
    if (tx_lock(tx, timeout) < 0) {
        printf("OK\n");
    }
    else {
        printf("FAIL\n");
    }
}

static void test_timeouts(int tx)
{
    uint64_t timeout = 0;
    tx_lock_lt(tx, timeout);

    timeout = XTIMER_BACKOFF - 1;
    tx_lock_lt(tx, timeout);

    timeout = XTIMER_BACKOFF;
    tx_lock_lt(tx, timeout);

    timeout = XTIMER_BACKOFF + 1;
    tx_lock_lt(tx, timeout);

    timeout = (2 * XTIMER_BACKOFF) - 1;
    tx_lock_lt(tx, timeout);

    timeout = (2 * XTIMER_BACKOFF);
    tx_lock_lt(tx, timeout);

    timeout = 1000000;
    tx_lock_lt(tx, timeout);
}

void *thread2(void *arg)
{
    (void) arg;
    msg_t m;

    printf("thread2: running\n");
    test_timeouts(2);

    /* tell thread 1 to unlock the mutex */
    msg_send(&m, pid);
    printf("thread2: waiting for mutex\n");
    if (xtimer_mutex_lock_timeout(&mutex, 2000000) == 0) {
        printf("thread2: got mutex! --> OK \n");
    }
    else {
        printf("thread2: didn't get the mutex --> FAIL\n");
    }

    printf("thread2: done\n");

    return 0;
}

void *thread1(void *arg)
{
    (void) arg;
    msg_t m, r;

    msg_receive(&m);
    tx_lock_eq(1, 0);
    tx_lock_lt(1, 0);

    mutex_unlock(&mutex);
    msg_reply(&m, &r);

    /* let main thread lock the mutex */
    msg_receive(&m);
    test_timeouts(1);

    /* mutex will be unlocked while waiting */
    msg_reply(&m, &r);
    printf("thread1: waiting for mutex\n");
    if (xtimer_mutex_lock_timeout(&mutex, 2000000) == 0) {
        printf("thread1: got mutex! --> OK \n");
    }
    else {
        printf("thread1: didn't get the mutex --> FAIL\n");
    }

    msg_receive(&m);

    /* got msg from thread 1, we have to unlock the mutex after a delay */
    xtimer_usleep(500000);
    printf("thread1: unlocking\n");
    mutex_unlock(&mutex);

    printf("thread1: done\n");

    return 0;
}

int main(void)
{
    msg_t m, r;

    mainpid = thread_getpid();
    pid = thread_create(
        thread1_stack,
        sizeof(thread1_stack),
        THREAD_PRIORITY_MAIN + 1,
        THREAD_CREATE_STACKTEST,
        thread1,
        NULL,
        "thread 1");

    printf("main: tests with unlocked mutex:\n");
    msg_send_receive(&m, &r, pid);

    printf("main: locking mutex\n");
    mutex_lock(&mutex);

    printf("main: tests with mutex locked once:\n");
    msg_send_receive(&m, &r, pid);

    printf("main: test unlocking while waiting\n");
    xtimer_usleep(500000);

    printf("main: unlocking...\n");
    mutex_unlock(&mutex);
    xtimer_usleep(500000);

    /* tests with 2+ nodes locking the mutex */
    printf("main: starting thread2\n");
    pid2 = thread_create(
        thread2_stack,
        sizeof(thread2_stack),
        THREAD_PRIORITY_MAIN + 2,
        THREAD_CREATE_STACKTEST,
        thread2,
        NULL,
        "thread 2");

    printf("main: locking mutex\n");
    mutex_lock(&mutex);

    printf("main: got mutex, unlocking\n");
    mutex_unlock(&mutex);

    printf("main: done\n");

    return 0;
}
