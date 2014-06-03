/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief pthread TLS test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "pthread.h"

#define NUM_CHILDREN 4

static pthread_key_t id_key;

static void cleanup(void *p)
{
    printf("Cleanup: %u\n", (int) (uintptr_t) p);
}

static void *run(void *parameter)
{
    pthread_setspecific(id_key, parameter);

    printf("pthread: parameter = %u\n", (int) (uintptr_t) pthread_getspecific(id_key));

    return pthread_getspecific(id_key);
}

int main(void)
{
    puts("Start.");

    pthread_key_create(&id_key, cleanup);

    pthread_t th_id[NUM_CHILDREN];
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pthread_create(&th_id[i], NULL, run, (void *) (intptr_t) i);
    }
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        void *res;
        pthread_join(th_id[i], (void **) &res);
        printf("Child %u returned with value %u\n", i, (int) (uintptr_t) res);
    }

    puts("Done.");
    return 0;
}
