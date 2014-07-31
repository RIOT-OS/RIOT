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
 * @brief pthread test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "pthread.h"

void *run(void *parameter) {
    size_t n = (size_t) parameter;
    size_t factorial = 1;

    printf("pthread: parameter = %u\n", (unsigned int) n);

    if (n > 0) {
        for (size_t i = 1; i <= n; i++) {
            factorial *= i;
        }
    }

    printf("pthread: factorial = %u\n", (unsigned int) factorial);
    pthread_exit((void *)factorial);

    return NULL;
}

int main(void) {
    pthread_t th_id;
    pthread_attr_t th_attr;

    size_t arg = 6;
    printf("main parameter = %u\n", (unsigned int) arg);

    pthread_attr_init(&th_attr);
    pthread_create(&th_id, &th_attr, run, (void *) arg);
    size_t res;
    pthread_join(th_id, (void **) &res);
    printf("main: factorial = %u\n", (unsigned int) res);
    puts("main: finished");
    return 0;
}

