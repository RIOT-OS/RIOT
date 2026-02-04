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
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "pthread.h"

#define RET_EXIT ((void *) 1234)
#define RET_FAIL ((void *) 5678)

static void cleanup(void *arg)
{
    printf("Cleanup: <%s>\n", (const char *) arg);
}

static void *run(void *unused) {
    (void) unused;

    /* indentation for visibility */
    puts("<SCOPE 0>");
    pthread_cleanup_push(cleanup, "1");
        puts("<SCOPE 1>");
        pthread_cleanup_push(cleanup, "2");
            puts("<SCOPE 2>");
            pthread_cleanup_push(cleanup, "3");
                puts("<SCOPE 3>");
                pthread_cleanup_push(cleanup, "4");
                    puts("<SCOPE 4>");
                    pthread_cleanup_push(cleanup, "5");
                        puts("<SCOPE 5 />");
                    pthread_cleanup_pop(1);
                    puts("</SCOPE 4>");
                pthread_cleanup_pop(0); /* cleanup 4 should not be executed */
                puts("</SCOPE 3>");
            pthread_cleanup_pop(1);
            pthread_exit(RET_EXIT);
            puts("/<SCOPE 2>"); /* thread exited, should not be printed */
        pthread_cleanup_pop(0); /* should be printed nevertheless */
        puts("</SCOPE 1>");
    pthread_cleanup_pop(1);
    puts("</SCOPE 0>");

    return RET_FAIL;
}

int main(void) {
    puts("START");

    pthread_t th_id;
    pthread_create(&th_id, NULL, run, NULL);

    void *res;
    pthread_join(th_id, (void **) &res);

    printf("Result: %i\n", (int) (intptr_t) res);

    if (res == RET_EXIT) {
        puts("SUCCESS");
    }
    else {
        puts("FAILURE");
    }
    return 0;
}
