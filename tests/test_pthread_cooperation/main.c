/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief pthread test application
 *
 * @author Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 12

pthread_t ths[NUM_THREADS];

pthread_mutex_t mtx;
volatile int storage = 1;

void *run(void *parameter)
{
    int arg = (int) parameter;
    printf("My arg: %d\n", arg);

    int err = pthread_mutex_lock(&mtx);

    if (err != 0) {
        printf("[!!!] pthread_mutex_lock failed with %d\n", err);
        return NULL;
    }

    storage *= arg;
    printf("val = %d\n", storage);
    pthread_mutex_unlock(&mtx);

    return NULL;
}

int main(void)
{
    pthread_attr_t th_attr;
    pthread_attr_init(&th_attr);
    pthread_mutex_init(&mtx, NULL);

    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("Creating thread with arg %d\n", (i + 1));
        pthread_create(&ths[i], &th_attr, run, (void *)(i + 1));
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("join\n");
        pthread_join(ths[i], NULL);
    }

    printf("Factorial: %d\n", storage);

    if (storage != 479001600) {
        puts("[!!!] Error, expected: 12!= 479001600.");
    }

    pthread_mutex_destroy(&mtx);
    pthread_attr_destroy(&th_attr);

    puts("finished");
    return 0;
}

