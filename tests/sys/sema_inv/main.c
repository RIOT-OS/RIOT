/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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
 * @brief   Inverse Semaphore Test Application
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include <stdio.h>

#include "sema_inv.h"
#include "ztimer.h"

char t1_stack[THREAD_STACKSIZE_SMALL];
char t2_stack[THREAD_STACKSIZE_SMALL];
char t3_stack[THREAD_STACKSIZE_SMALL];

struct thread_ctx {
    sema_inv_t *sync;
    unsigned id;
};

static void *thread_count(void *arg)
{
    struct thread_ctx *ctx = arg;

    printf("THREAD %u start\n", ctx->id);

    ztimer_sleep(ZTIMER_MSEC, 5);

    if (sema_inv_post(ctx->sync)) {
        printf("THREAD %u woke main thread\n", ctx->id);
    }

    return arg;
}

static void *thread_bit(void *arg)
{
    struct thread_ctx *ctx = arg;

    printf("THREAD %u start\n", ctx->id);

    ztimer_sleep(ZTIMER_MSEC, 5);

    if (sema_inv_post_mask(ctx->sync, 1 << ctx->id)) {
        printf("THREAD %u woke main thread\n", ctx->id);
    }

    return arg;
}

static void test_counter_mode(void)
{
    sema_inv_t sync;

    struct thread_ctx ctx[3] = {
        { .sync = &sync, .id = 1 },
        { .sync = &sync, .id = 2 },
        { .sync = &sync, .id = 3 },
    };

    puts("counter mode");
    sema_inv_init(&sync, 3);

    thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 1,
                  0, thread_count, &ctx[0], "nr1");
    thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN + 1,
                  0, thread_count, &ctx[1], "nr2");
    thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN + 1,
                  0, thread_count, &ctx[2], "nr3");

    sema_inv_wait(&sync);
    puts("thread synced");

    /* wait for all threads to terminate, we are going to reuse the stack */
    ztimer_sleep(ZTIMER_MSEC, 50);
}

static void test_mask_mode(void)
{
    sema_inv_t sync;

    struct thread_ctx ctx[3] = {
        { .sync = &sync, .id = 1 },
        { .sync = &sync, .id = 2 },
        { .sync = &sync, .id = 3 },
    };

    puts("mask mode");
    sema_inv_init(&sync, 0xE);

    thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 1,
                  0, thread_bit, &ctx[0], "nr1");
    thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN + 1,
                  0, thread_bit, &ctx[1], "nr2");
    thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN + 1,
                  0, thread_bit, &ctx[2], "nr3");

    sema_inv_wait(&sync);
    puts("thread synced");

    /* wait for all threads to terminate, we are going to reuse the stack */
    ztimer_sleep(ZTIMER_MSEC, 50);
}

int main(void)
{
    test_counter_mode();
    test_mask_mode();

    puts("SUCCESS");

    return 0;
}
