/*
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
 * Copyright (C) 2013 René Kijewski <rene.kijewski@fu-berlin.de>
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @brief Semaphore test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <semaphore.h>

#include "msg.h"
#include "timex.h"
#include "thread.h"
#include "vtimer.h"

#define SEMAPHORE_MSG_QUEUE_SIZE        (8)
#define SEMAPHORE_TEST_THREADS          (5)
static char test1_thread_stack[THREAD_STACKSIZE_MAIN];
static char test2_thread_stack[SEMAPHORE_TEST_THREADS][THREAD_STACKSIZE_MAIN];
static msg_t main_msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];
static msg_t test1_msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];

static sem_t s1;
static sem_t s2;

static void *test1_second_thread(void *arg)
{
    (void) arg;
    msg_init_queue(test1_msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    puts("second: sem_trywait");

    if (sem_trywait(&s1) < 0) {
        puts("second: sem_trywait failed");
    }

    puts("second: sem_trywait done with == 0");

    puts("second: wait for post");

    if (sem_wait(&s1) < 0) {
        puts("second: sem_wait failed");
    }

    puts("second: sem was posted");

    puts("second: end");
    return NULL;
}

static void test1(void)
{
    puts("first: sem_init");

    if (sem_init(&s1, 0, 0) < 0) {
        puts("first: sem_init failed");
    }

    puts("first: thread create");
    kernel_pid_t pid = thread_create(test1_thread_stack,
                                     sizeof(test1_thread_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     CREATE_STACKTEST | CREATE_WOUT_YIELD,
                                     test1_second_thread,
                                     NULL,
                                     "second");
    if (pid == KERNEL_PID_UNDEF) {
        puts("first: thread create failed");
    }
    puts("first: thread created");

    puts("first: sem_getvalue");
    int val;

    if (sem_getvalue(&s1, &val) < 0 || val != 0) {
        puts("first: sem_getvalue FAILED");
    }

    puts("first: sem_getvalue != 0");

    puts("first: do yield");
    thread_yield();
    puts("first: done yield");

    /*****************************************************************************/

    puts("first: sem_trywait");

    if (sem_trywait(&s1) < 0) {
        puts("first: sem_trywait FAILED");
    }

    puts("first: sem_trywait done");

    puts("first: sem_post");

    if (sem_post(&s1) < 0) {
        puts("first: sem_post FAILED");
    }

    puts("first: sem_post done");

    /*****************************************************************************/

    puts("first: sem_destroy");

    if (sem_destroy(&s1) < 0) {
        puts("first: sem_destroy FAILED");
    }

    puts("first: end");
}

static void *priority_sema_thread(void *name)
{
    msg_t msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];
    msg_init_queue(msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    sem_wait(&s1);
    printf("Thread '%s' woke up.\n", (const char *) name);
    return NULL;
}

char names[SEMAPHORE_TEST_THREADS][16];
void test2(void)
{
    puts("first: sem_init");

    if (sem_init(&s1, 0, 0) < 0) {
        puts("first: sem_init FAILED");
    }

    for (int i = 0; i < SEMAPHORE_TEST_THREADS; i++) {
        int priority = THREAD_PRIORITY_MAIN - (i + 3) % 10 + 1;

        snprintf(names[i], sizeof(names[i]), "priority %d", priority);
        printf("first: thread create: %d\n", priority);
        kernel_pid_t pid = thread_create(test2_thread_stack[i],
                                         sizeof(test2_thread_stack[i]),
                                         priority,
                                         CREATE_STACKTEST,
                                         priority_sema_thread,
                                         names[i],
                                         names[i]);

        if (pid == KERNEL_PID_UNDEF) {
            puts("first: thread create FAILED");
        }

        printf("first: thread created: %s (%d/%d)\n", names[i], i + 1, SEMAPHORE_TEST_THREADS);
    }

    puts("------------------------------------------");

    for (int i = 0; i < SEMAPHORE_TEST_THREADS; i++) {
        printf("post no. %d\n", i);
        sem_post(&s1);
        puts("Back in main thread.");
    }
}

static void *test3_one_two_thread(void *arg)
{
    msg_t msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];
    (void)arg;
    msg_init_queue(msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    sem_wait(&s1);
    puts("Thread 1 woke up after waiting for s1.");
    sem_wait(&s2);
    puts("Thread 1 woke up after waiting for s2.");
    return NULL;
}

static void *test3_two_one_thread(void *arg)
{
    msg_t msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];
    (void)arg;
    msg_init_queue(msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    sem_wait(&s2);
    puts("Thread 2 woke up after waiting for s2.");
    sem_wait(&s1);
    puts("Thread 2 woke up after waiting for s1.");
    return NULL;
}

void test3(void)
{
    puts("first: sem_init s1");
    if (sem_init(&s1, 0, 0) < 0) {
        puts("first: sem_init FAILED");
    }
    puts("first: sem_init s2");
    if (sem_init(&s2, 0, 0) < 0) {
        puts("first: sem_init FAILED");
    }
    puts("first: create thread 1");
    if (thread_create(test2_thread_stack[0], sizeof(test2_thread_stack[0]),
                      THREAD_PRIORITY_MAIN - 1, CREATE_STACKTEST,
                      test3_one_two_thread, NULL, "thread 1") < 0) {
        puts("first: thread create FAILED");
        return;
    }
    puts("first: create thread 2");
    if (thread_create(test2_thread_stack[1], sizeof(test2_thread_stack[1]),
                      THREAD_PRIORITY_MAIN - 1, CREATE_STACKTEST,
                      test3_two_one_thread, NULL, "thread 2") < 0) {
        puts("first: thread create FAILED");
        return;
    }
    puts("------------------------------------------");
    puts("post s1");
    sem_post(&s1);
    puts("post s2");
    sem_post(&s2);
    puts("post s2");
    sem_post(&s2);
    puts("post s1");
    sem_post(&s1);
}

void test4(void)
{
    struct timespec abs;
    char timestamp[TIMEX_MAX_STR_LEN];
    timex_t now, start, stop, exp = { 1, 0 };
    vtimer_now(&now);
    abs.tv_sec = now.seconds + 1;
    abs.tv_nsec = now.microseconds * 1000;
    puts("first: sem_init s1");
    if (sem_init(&s1, 0, 0) < 0) {
        puts("first: sem_init FAILED");
    }
    vtimer_now(&start);
    puts("first: wait 1 sec for s1");
    if (sem_timedwait(&s1, &abs) != 0) {
        if (errno != ETIMEDOUT) {
            printf("error waiting: %d\n", errno);
            return;
        }
        else {
            puts("first: timed out");
        }
    }
    vtimer_now(&stop);
    stop = timex_sub(stop, start);
    if (timex_cmp(stop, exp) < 0) {
        printf("first: waited only %s => FAILED\n",
               timex_to_str(stop, timestamp));
    }
    printf("first: waited %s\n", timex_to_str(stop, timestamp));
}

int main(void)
{
    msg_init_queue(main_msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    puts("######################### TEST1:");
    test1();
    puts("######################### TEST2:");
    test2();
    puts("######################### TEST3:");
    test3();
    puts("######################### TEST4:");
    test4();
    puts("######################### DONE");
    return 0;
}
