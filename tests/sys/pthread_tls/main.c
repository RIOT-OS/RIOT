/*
 * SPDX-FileCopyrightText: 2014 Hamburg University of Applied Sciences (HAW)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief pthread tls test application
 *
 * @author Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "pthread.h"

#define NUMBER_OF_TLS (20)

void *run(void *parameter)
{
    pthread_key_t aKeys[NUMBER_OF_TLS];
    int aTLS_values[NUMBER_OF_TLS];

    (void)parameter;

    printf("\n-= TEST 1 - create %d tls with sequential values 0...%d =-\n",
           NUMBER_OF_TLS, NUMBER_OF_TLS - 1);

    for (int i = 0; i < NUMBER_OF_TLS; ++i) {
        aTLS_values[i] = i;
        pthread_key_create(&(aKeys[i]), NULL);
        pthread_setspecific(aKeys[i], &aTLS_values[i]);
    }

    printf("now rise sequential by one values 1...%d\n", NUMBER_OF_TLS);

    for (int i = 0; i < NUMBER_OF_TLS; ++i) {
        aTLS_values[i]++;
    }

    printf("pick deliberate storage (key[3]:%" PRIuPTR ") and change the value\n",
           (uintptr_t)aKeys[3]);
    void *val = pthread_getspecific(aKeys[3]);
    *((int *)val) = 42;

    puts("show tls values:");

    for (int i = 0; i < NUMBER_OF_TLS; ++i) {
        void *val = pthread_getspecific(aKeys[i]);
        int x = *(int *)val;
        printf("key[%d]: %" PRIuPTR ", val: %d\n", i, (uintptr_t)aKeys[i], x);
    }

    printf("\n -= TEST 2 - delete deliberate key (key[5]:%" PRIuPTR ") =-\n",
           (uintptr_t)aKeys[5]);
    pthread_key_delete(aKeys[5]);

    puts("show tls values:");

    for (int i = 0; i < NUMBER_OF_TLS; ++i) {
        void *val = pthread_getspecific(aKeys[i]);

        if (val != NULL) {
            int x = *(int *)val;
            printf("key[%d]: %" PRIuPTR ", val: %d\n", i, (uintptr_t)aKeys[i], x);
        }
    }

    puts("");
    puts("-= TEST 3 - create new tls =-");
    int new_val = 99;
    pthread_key_t new_key;
    pthread_key_create(&new_key, NULL);
    pthread_setspecific(new_key, &new_val);

    printf("added new tls, key: %" PRIuPTR ", val: %d\n", (uintptr_t)new_key, new_val);
    printf("show tls values:\n");

    for (int i = 0; i < NUMBER_OF_TLS; ++i) {
        void *val = pthread_getspecific(aKeys[i]);

        if (val != NULL) {
            int x = *(int *)val;
            printf("key[%d]: %" PRIuPTR ", val: %d\n", i, (uintptr_t)aKeys[i], x);
        }
    }

    puts("");
    puts("-= TEST 4 - delete all keys =-");

    for (int i = 0; i < NUMBER_OF_TLS; ++i) {
        pthread_key_delete(aKeys[i]);
    }

    printf("show tls values:\n");

    for (int i = 0; i < NUMBER_OF_TLS; ++i) {
        void *val = pthread_getspecific(aKeys[i]);

        if (val != NULL) {
            int x = *(int *)val;
            printf("key[%d]: %" PRIuPTR ", val: %d\n", i, (uintptr_t)aKeys[i], x);
        }
    }

    puts("");
    puts("-= TEST 5 - try delete non-existing key =-");
    printf("try to delete returns: %d\n",
           pthread_key_delete((pthread_key_t)99));

    puts("");
    puts("-= TEST 6 - add key and delete without a tls =-");

    pthread_key_create(&new_key, NULL);
    printf("created key: %" PRIuPTR "\n", (uintptr_t)new_key);
    printf("try to delete returns: %d\n", pthread_key_delete(new_key));

    puts("");
    puts("-= TEST 7 - add key without tls =-");
    pthread_key_create(&new_key, NULL);
    printf("created key: %" PRIuPTR "\n", (uintptr_t)new_key);
    void* test_7_val = pthread_getspecific(new_key);
    printf("test_7_val: %p\n", test_7_val);

    return NULL;
}

int main(void)
{
    puts("START");
    pthread_t th_id;
    pthread_attr_t th_attr;

    pthread_attr_init(&th_attr);
    pthread_create(&th_id, &th_attr, run, NULL);

    size_t res;
    pthread_join(th_id, (void **) &res);
    puts("tls tests finished.");

    if (res == 0) {
        puts("SUCCESS");
    }
    else {
        puts("FAILURE");
    }
    return 0;
}
