/*
 * Copyright (C) 2020 Freie Universität Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       testing sema
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <errno.h>
#include <stdio.h>
#include <limits.h>

#include "msg.h"
#include "sema.h"
#include "thread.h"

#define TEST_TIME_US 1000
#define TEST_ITERATIONS 4

static char stack[THREAD_STACKSIZE_SMALL];
static sema_t test_sema;
static sema_t test_sema2;

static void *second_thread(void *arg)
{
    int *thread_success = arg;

    sema_post(&test_sema);
    sema_post(&test_sema);
    if (sema_try_wait(&test_sema2) != 0) {
        printf("THREAD ERROR: sema_try_wait()");
        *thread_success = -1;
    }
    if (sema_wait_timed(&test_sema, TEST_TIME_US) != 0) {
        printf("THREAD ERROR: sema_wait_timed()");
        *thread_success = -1;
    }
    if (sema_wait_timed(&test_sema2, TEST_TIME_US) != 0) {
        printf("THREAD ERROR: sema_wait_timed()");
        *thread_success = -1;
    }
    if (sema_wait(&test_sema2) != -ECANCELED) {
        printf("THREAD ERROR: sema_wait()");
        *thread_success = -1;
    }
    if (*thread_success != -1) {
        *thread_success = 1;
    }

    return NULL;
}

int main(void)
{
    int thread_success = 0;

    sema_create(&test_sema, 0);
    sema_create(&test_sema2, 1);
    if (sema_try_wait(&test_sema) != -EAGAIN) {
        printf("MAIN ERROR: sema_try_wait()");
        return 1;
    }

    if (sema_wait_timed(&test_sema, TEST_TIME_US) != -ETIMEDOUT) {
        printf("MAIN ERROR: sema_wait_timed()");
        return 1;
    }
    if (sema_wait_timed_ztimer(&test_sema, ZTIMER_USEC, TEST_TIME_US)
        != -ETIMEDOUT) {
        printf("MAIN_ERROR: sema_wait_timed_ztimer()");
        return 1;
    }

    thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_WOUT_YIELD,
                  second_thread,
                  &thread_success,
                  "second_thread");

    if (sema_wait(&test_sema) != 0) {
        printf("MAIN ERROR: sema_wait()");
        return 1;
    }
    sema_post(&test_sema2);

    sema_destroy(&test_sema);
    sema_destroy(&test_sema2);
    sema_post(&test_sema2);
    if (sema_wait_timed(&test_sema, TEST_TIME_US) != -ECANCELED) {
        printf("MAIN ERROR: sema_wait_timed()");
        return 1;
    }
    if (sema_wait(&test_sema) != -ECANCELED) {
        printf("MAIN ERROR: sema_wait()");
        return 1;
    }
    if (sema_try_wait(&test_sema) != -ECANCELED) {
        printf("MAIN ERROR: sema_wait()");
        return 1;
    }
    /* UINT_MAX test */
    test_sema.value = UINT_MAX;
    if (sema_post(&test_sema) != -EOVERFLOW) {
        printf("MAIN ERROR: sema_post()");
        return 1;
    }

    if (thread_success == 1) {
        printf("SUCCESS\n");

    }
    else {
        printf("FAILURE\n");
    }
    return 0;
}
