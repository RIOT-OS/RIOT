/*
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
 * Copyright (C) 2013 René Kijewski <rene.kijewski@fu-berlin.de>
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

#include "thread.h"
#include "semaphore.h"

#define SEMAPHORE_TEST_THREADS 10
char test1_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];
char test2_thread_stack[SEMAPHORE_TEST_THREADS][KERNEL_CONF_STACKSIZE_MAIN];

sem_t s;

static void *test1_second_thread(void *arg)
{
    (void) arg;
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
                                     PRIORITY_MAIN - 1,
                                     CREATE_STACKTEST | CREATE_WOUT_YIELD,
                                     test1_second_thread,
                                     NULL,
                                     "second");

    if (pid < 0) {
        puts("first: thread create failed");
    }

    puts("first: thread created");

    puts("first: sem_getvalue");
    int val;

    if (sem_getvalue(&s, &val) != 0 || val != 0) {
        puts("first: sem_getvalue failed");
    }

    puts("first: sem_getvalue != 0");

    puts("first: do yield");
    thread_yield();
    puts("first: done yield");

    /*****************************************************************************/

    puts("first: sem_trywait");

    if (sem_trywait(&s) != -1) {
        puts("first: sem_trywait failed");
    }

    puts("first: sem_trywait done");

    puts("first: sem_post");

    if (sem_post(&s) != 1) {
        puts("first: sem_post failed");
    }

    puts("first: sem_post done");

    /*****************************************************************************/

    puts("first: sem_destroy");

    if (sem_destroy(&s) != 0) {
        puts("first: sem_destroy failed");
    }

    puts("first: end");
}

static void *priority_sema_thread(void *arg)
{
    (void) arg;
    sem_wait(&s);
    printf("Thread '%s' woke up.\n", thread_getname(thread_getpid()));
    return NULL;
}

char names[SEMAPHORE_TEST_THREADS][16];
void test2(void)
{
    puts("first: sem_init");

    if (sem_init(&s, 0, 0) != 0) {
        puts("first: sem_init failed");
    }

    for (int i = 0; i < SEMAPHORE_TEST_THREADS; i++) {
        int priority = PRIORITY_MAIN - (i + 3) % 10 + 1;

        snprintf(names[i], sizeof(names[i]), "priority %d", priority);
        printf("first: thread create: %d\n", priority);
        kernel_pid_t pid = thread_create(test2_thread_stack[i],
                                         sizeof(test2_thread_stack[i]),
                                         priority,
                                         CREATE_STACKTEST,
                                         priority_sema_thread,
                                         NULL,
                                         names[i]);

        if (pid == KERNEL_PID_UNDEF) {
            puts("first: thread create failed");
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
    puts("#########################");
    test1();
    puts("#########################");
    test2();
    puts("#########################");
    return 0;
}
