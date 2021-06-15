/*
 * Copyright (C) 2020 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <thread.h>
#include <sched.h>

#include <math.h>

#include "xtimer.h"

#include "sched_round_robin.h"

#define PRINT_STEPS 10
#define WORK_SCALE  1000

static void bad_wait(uint32_t us)
{
    uint32_t start = xtimer_now_usec();
    /*keep the CPU busy waiting for some time to pass simulate working*/
    while ((xtimer_now_usec() - start) < us){
        /*do nothing*/
    }
}

static void nice_wait(uint32_t us)
{
    /*be nice give the CPU some time to do other things or rest*/
    xtimer_usleep(us);
}

/*
 * the following are threads that count and wait with different strategies and
 * print their current count in steps.
 * the ration of active (doing hard work like checking the timer)
 * to passive (wait to be informed when a certain time is there) waiting
 * is determined by there value given to the thread.
 * the restless threads do never pause.
 */

void * thread_nicebreaks(void * d)
{
    xtimer_usleep(200 * 1000);  /*always be nice at start*/
#ifdef DEVELHELP
    const char *name = thread_get_active()->name;
#else
    int16_t pid = thread_getpid();
#endif

    uint32_t w = 0;
    uint32_t work = *((uint32_t *) d);
    if (work > 10) {
        work = 5;
    }
    uint32_t rest = (10 - work);
    uint32_t step = 0;

    /*work some time and rest*/
    for (;;) {
        if (w - step >= PRINT_STEPS) {
#ifdef DEVELHELP
            printf("%s: %" PRIu32 ", %" PRIu32 "\n", name, w, work);
#else
            printf("T-Pid %i:%" PRIu32 ", %" PRIu32 "\n", pid, w, work);
#endif
            step = w;
        }
        bad_wait(work * WORK_SCALE);
        w += work;
        nice_wait(rest * WORK_SCALE);
    }
}

void * thread_bad(void * d)
{
    xtimer_usleep(200 * 1000);  /*always be nice at start*/
#ifdef DEVELHELP
    const char *name = thread_get_active()->name;
#else
    int16_t pid = thread_getpid();
#endif

    uint32_t w = 0;
    uint32_t work = *((uint32_t *) d);
    if (work > 10) {
        work = 5;
    }
    uint32_t rest = (10 - work);
    uint32_t step = 0;

    /*work some time yield rest blocking*/
    for (;;) {
        if (w - step >= PRINT_STEPS) {
#ifdef DEVELHELP
            printf("%s: %" PRIu32 ", %" PRIu32 "\n", name, w, work);
#else
            printf("T-Pid %i:%" PRIu32 ", %" PRIu32 "\n", pid, w, work);
#endif
            step = w;
        }
        bad_wait(work * WORK_SCALE);
        w += work;
        bad_wait(rest * WORK_SCALE);
    }
}

void * thread_restless_yielding(void * d)
{
    xtimer_usleep(200 * 1000);  /*always be nice at start*/
#ifdef DEVELHELP
    const char *name = thread_get_active()->name;
#else
    int16_t pid = thread_getpid();
#endif

    uint32_t w = 0;
    uint32_t work = *((uint32_t *) d);
    if (work > 10) {
        work = 5;
    }
    /*uint32_t rest = (10 - work);*/
    uint32_t step = 0;

    /*work for some time and yield*/
    for (;;) {
        if (w - step >= PRINT_STEPS) {
#ifdef DEVELHELP
            printf("%s: %" PRIu32 ", %" PRIu32 "\n", name, w, work);
#else
            printf("T-Pid %i:%" PRIu32 ", %" PRIu32 "\n", pid, w, work);
#endif
            step = w;
        }
        bad_wait(work * WORK_SCALE);
        w += work;
        thread_yield();
    }
}

void * thread_restless(void * d)
{
    xtimer_usleep(200 * 1000);  /*always be nice at start*/
#ifdef DEVELHELP
    const char * name = thread_get_active()->name;
#else
    int16_t pid = thread_getpid();
#endif

    uint32_t w = 0;
    uint32_t work = *((uint32_t *) d);
    if (work > 10) {
        work = 5;
    }
    /*uint32_t rest = (10 - work);*/
    uint32_t step = 0;

    /*continue working*/
    for (;;) {
        if (w - step >= PRINT_STEPS) {
#ifdef DEVELHELP
            printf("%s: %" PRIu32 ", %" PRIu32 "\n", name, w, work);
#else
            printf("T-Pid %i:%" PRIu32 ", %" PRIu32 "\n", pid, w, work);
#endif
            step = w;
        }
        bad_wait(work * WORK_SCALE);
        w += work;
    }
}

int main(void)
{
    {
        static char stack[THREAD_STACKSIZE_DEFAULT];
        static uint32_t workness = 3;   /* 0-10 workness*/
        thread_create(stack, sizeof(stack), 7, THREAD_CREATE_STACKTEST,
                      thread_restless, &workness, "T1");
    }
    {
        static char stack[THREAD_STACKSIZE_DEFAULT];
        static uint32_t workness = 5;   /* 0-10 workness*/
        thread_create(stack, sizeof(stack), 7, THREAD_CREATE_STACKTEST,
                      thread_restless, &workness, "T2");
    }
    {
        static char stack[THREAD_STACKSIZE_DEFAULT];
        static uint32_t workness = 5;   /* 0-10 workness*/
        thread_create(stack, sizeof(stack), 7, THREAD_CREATE_STACKTEST,
                      thread_restless, &workness, "T3");
    }
}
