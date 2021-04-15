/*
 * Copyright (C) 2020 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "thread.h"
#include "sched.h"

#include "ztimer.h"
#include "timex.h"

#include "sched_round_robin.h"

#define PRINT_STEPS 10
#define WORK_SCALE  1000

static void bad_wait(uint32_t us)
{
    /*keep the CPU busy waiting for some time to pass simulate working*/
    ztimer_spin(ZTIMER_USEC, us);
}__attribute__((unused))

static void nice_wait(uint32_t us)
{
    /*be nice give the CPU some time to do other things or rest*/
    ztimer_sleep(ZTIMER_USEC, us);
}__attribute__((unused))

static void yield_wait(uint32_t unused)
{
    (void) unused;
    /* do not wait just yield */
    thread_yield();
}__attribute__((unused))

static void no_wait(uint32_t unused)
{
    (void) unused;
    /* do not wait */
}__attribute__((unused))

/*
 * the following are threads that count and wait with different strategies and
 * print their current count in steps.
 * the ration of active (doing hard work like checking the timer)
 * to passive (wait to be informed when a certain time is there) waiting
 * is determined by there value given to the thread.
 * the restless threads do never pause.
 */

struct worker_config {
    void (*waitfn) (unsigned);
    uint32_t work;
};

void * thread_worker(void * d)
{
    nice_wait(200 *  US_PER_MS);  /*always be nice at start*/
#ifdef DEVELHELP
    const char *name = thread_get_active()->name;
#else
    int16_t pid = thread_getpid();
#endif

    uint32_t w = 0;
    struct worker_config *wc =  d;
    /* this is doing 10 Steps which divided into work (busy waiting)
     * and not work of these 10 steps up to 10 might be work but not more
     * if the given value is out of range work ratio is set to 5 */
    uint32_t work = wc->work;
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
        wc->waitfn(rest * WORK_SCALE);
    }
}

/* no_wait ->    a restless thread always working until it is suspended
 * yield_wait -> a restless thread that yields before continuing with the next work package
 * bad_wait ->   a thread that does pause very intensely
 * nice_wait ->  a thread does nice breaks giving other threads time to do something
 */
/* yield_wait and nice_wait threads are able to work in "parallel" without sched_round_robin*/

#ifndef  THREAD_1
#define  THREAD_1 {no_wait, 5}
#endif

#ifndef  THREAD_2
#define  THREAD_2 {no_wait, 5}
#endif

#ifndef  THREAD_3
#define  THREAD_3 {no_wait, 5}
#endif

int main(void)
{
    {
        static char stack[THREAD_STACKSIZE_DEFAULT];
        static struct worker_config wc = THREAD_1;   /* 0-10 workness*/
        thread_create(stack, sizeof(stack), 7, THREAD_CREATE_STACKTEST,
                      thread_worker, &wc, "T1");
    }
    {
        static char stack[THREAD_STACKSIZE_DEFAULT];
        static struct worker_config wc = THREAD_2;   /* 0-10 workness*/
        thread_create(stack, sizeof(stack), 7, THREAD_CREATE_STACKTEST,
                      thread_worker, &wc, "T2");
    }
    {
        static char stack[THREAD_STACKSIZE_DEFAULT];
        static struct worker_config wc = THREAD_3;   /* 0-10 workness*/
        thread_create(stack, sizeof(stack), 7, THREAD_CREATE_STACKTEST,
                      thread_worker, &wc, "T3");
    }
}
