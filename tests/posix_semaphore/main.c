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
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "thread.h"
#include "semaphore.h"

#define SEMAPHORE_MSG_QUEUE_SIZE        (8)
#define SEMAPHORE_TEST_THREADS          (5)
static char test1_thread_stack[THREAD_STACKSIZE_MAIN];
static char test2_thread_stack[SEMAPHORE_TEST_THREADS][THREAD_STACKSIZE_MAIN];
static msg_t main_msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];
static msg_t test1_msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];

static sem_t s;

static void *test1_second_thread(void *arg)
{
    (void) arg;
    msg_init_queue(test1_msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    puts("second: sem_trywait");

    if (sem_trywait(&s) == 0) {
        puts("second: sem_trywait failed");
    }

    puts("second: sem_trywait done with == 0");

    puts("second: wait for post");

    if (sem_wait(&s) != 1) {
        puts("second: sem_wait failed");
    }

    puts("second: sem was posted");

    puts("second: end");
    return NULL;
}

static void test1(void)
{
    puts("first: sem_init");

    if (sem_init(&s, 0, 0) != 0) {
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

    if (sem_getvalue(&s, &val) != 0 || val != 0) {
        puts("first: sem_getvalue FAILED");
    }

    puts("first: sem_getvalue != 0");

    puts("first: do yield");
    thread_yield();
    puts("first: done yield");

    /*****************************************************************************/

    puts("first: sem_trywait");

    if (sem_trywait(&s) != -1) {
        puts("first: sem_trywait FAILED");
    }

    puts("first: sem_trywait done");

    puts("first: sem_post");

    if (sem_post(&s) != 1) {
        puts("first: sem_post FAILED");
    }

    puts("first: sem_post done");

    /*****************************************************************************/

    puts("first: sem_destroy");

    if (sem_destroy(&s) != 0) {
        puts("first: sem_destroy FAILED");
    }

    puts("first: end");
}

static void *priority_sema_thread(void *name)
{
    msg_t msg_queue[SEMAPHORE_MSG_QUEUE_SIZE];
    msg_init_queue(msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    sem_wait(&s);
    printf("Thread '%s' woke up.\n", (const char *) name);
    return NULL;
}

char names[SEMAPHORE_TEST_THREADS][16];
void test2(void)
{
    puts("first: sem_init");

    if (sem_init(&s, 0, 0) != 0) {
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
        sem_post(&s);
        puts("Back in main thread.");
    }
}

int main(void)
{
    msg_init_queue(main_msg_queue, SEMAPHORE_MSG_QUEUE_SIZE);
    puts("######################### TEST1:");
    test1();
    puts("######################### TEST2:");
    test2();
    puts("######################### DONE");
    return 0;
}
