/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief pthread_barrier test
 *
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "pthread.h"
#include "random.h"
#include "ztimer.h"

#define NUM_CHILDREN 4
#define NUM_ITERATIONS 5

#define RAND_SEED 0xC0FFEE

static pthread_barrier_t barrier;

static void *run(void *id_)
{
    int id = (intptr_t) id_ + 1;
    printf("Start %i\n", id);

    for (int i = 1; i <= NUM_ITERATIONS; ++i) {
        if (id == NUM_CHILDREN) {
            puts("\n======================================\n");
        }
        pthread_barrier_wait(&barrier);

        uint32_t timeout_us = random_uint32() % 2500000;
        printf("Child %i sleeps for %8" PRIu32 " us.\n", id, timeout_us);
        ztimer_sleep(ZTIMER_USEC, timeout_us);
    }

    printf("Done %i\n", id);
    return NULL;
}

int main(void)
{
    random_init(RAND_SEED);

    printf("NUM_CHILDREN: %d, NUM_ITERATIONS: %d\n", NUM_CHILDREN,
           NUM_ITERATIONS);
    pthread_barrier_init(&barrier, NULL, NUM_CHILDREN);

    pthread_t children[NUM_CHILDREN];
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pthread_create(&children[i], NULL, run, (void *) (intptr_t) i);
    }

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        void *void_;
        pthread_join(children[i], &void_);
    }

    pthread_barrier_destroy(&barrier);
    puts("");
    puts("SUCCESS");
    return 0;
}
