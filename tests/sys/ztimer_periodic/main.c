/*
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       ztimer sleep range test application
 *
 * @author      Snehal Verma <snehalverma10@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <semaphore.h>

#include "ztimer.h"
#include "timex.h"
#include "pthread.h"

#define RUNS                5U
#define SLEEP_TIMES_NUMOF   2U

#define SLEEP_TEST_THREADS          (3)
#define TEST_STACK_SIZE 1024 + 256

// In microseconds
static const uint32_t sleep_times[][2] = {
    {10000, 11000},
    {50000, 51000},
};

static const uint32_t t2_sleep_times[][2] = {
    {10, 1000},
    {10, 100000},
    {10, 40000}
};

static char test2_thread_stack[3][TEST_STACK_SIZE];

int test1(void)
{
    printf("Running test %u times with %u distinct sleep time ranges\n",
            RUNS, SLEEP_TIMES_NUMOF);

    uint32_t start_test = ztimer_now(ZTIMER_USEC);

    for (unsigned m = 0; m < RUNS; m++) {
        for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
            uint32_t start_sleep = ztimer_now(ZTIMER_USEC);
            ztimer_sleep_range(sleep_times[n][0], sleep_times[n][1]);
            uint32_t slept = ztimer_now(ZTIMER_USEC) - start_sleep;
            printf("Slept between %" PRIu32 " us and %" PRIu32 " us, actually slept %" PRIu32 " us\n",
                   sleep_times[n][0], sleep_times[n][1], slept);
        }
    }

    printf("Test ran for %" PRIu32 " us\n",
            ztimer_now(ZTIMER_USEC) - start_test);

    return 0;
}

static sem_t s1;
void *run(void *parameter)
{
    int id = *(int *)parameter;
    uint32_t start_sleep = ztimer_now(ZTIMER_USEC);
    ztimer_sleep_range(t2_sleep_times[id][0], t2_sleep_times[id][1]);
    uint32_t slept = ztimer_now(ZTIMER_USEC) - start_sleep;
    printf("THREAD %d: Slept between %" PRIu32 " us and %" PRIu32 " us, actually slept %" PRIu32 " us\n",
            id, t2_sleep_times[id][0], t2_sleep_times[id][1], slept);
    sem_post(&s1);
    return NULL;
}

int test2(void)
{
    if (sem_init(&s1, 0, 0) < 0) {
        puts("first: sem_init FAILED");
    }
    
    int args[] = {0, 1, 2};
    char names[][16] = {"test1", "test2", "test3"};
    for (int i = 0; i < SLEEP_TEST_THREADS; i++)
    {
        int priority = THREAD_PRIORITY_MAIN - (i + 3) % 10 + 1;
        //printf("CREATE LOOP %d\n", i);
        thread_create(test2_thread_stack[i],
                                         sizeof(test2_thread_stack[i]),
                                         priority,
                                         THREAD_CREATE_STACKTEST,
                                         run,
                                         &args[i],
                                         names[i]);
        printf("CREATED THREAD %d\n", i);
    }
    for (uint8_t i = 0; i < SLEEP_TEST_THREADS; i++) {
        // printf("post no. %d\n", i);
        sem_wait(&s1);
        // puts("Back in main thread.");
    }
    puts("FINISHED\n");

    return 0;
}

int main(void)
{
    // puts("######################### TEST1:");
    // test1();
    puts("######################### TEST2:\n\n");
    ztimer_acquire(ZTIMER_USEC);
    test2();
    ztimer_release(ZTIMER_USEC);
}
