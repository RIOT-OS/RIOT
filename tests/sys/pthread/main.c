/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief pthread test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "pthread.h"

#define FACTORIAL_PARAM     (6U)
#define FACTORIAL_EXPECTED  (720U)

void *run(void *parameter) {
    size_t n = (size_t) parameter;
    size_t factorial = 1;

    printf("pthread: parameter = %" PRIuSIZE "\n", n);

    if (n > 0) {
        for (size_t i = 1; i <= n; i++) {
            factorial *= i;
        }
    }

    printf("pthread: factorial = %" PRIuSIZE "\n",factorial);
    pthread_exit((void *)factorial);

    return NULL;
}

int main(void) {
    pthread_t th_id;
    pthread_attr_t th_attr;

    size_t arg = FACTORIAL_PARAM;
    printf("main: parameter = %u\n", (unsigned int) arg);

    pthread_attr_init(&th_attr);
    pthread_create(&th_id, &th_attr, run, (void *) arg);
    size_t res;
    pthread_join(th_id, (void **) &res);
    printf("main: factorial = %u\n", (unsigned int) res);

    if (res == FACTORIAL_EXPECTED) {
        puts("SUCCESS");
    }
    else {
        puts("FAILURE");
    }

    return 0;
}
