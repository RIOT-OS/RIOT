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
#define STEPS_PER_SET 10

__attribute__((unused))
static void bad_wait(uint32_t us)
{
    /* keep the CPU busy waiting for some time to pass simulate working */
    ztimer_spin(ZTIMER_USEC, us);
}

static void (* const do_work)(uint32_t us) = bad_wait;

__attribute__((unused))
static void nice_wait(uint32_t us)
{
    /* be nice give the CPU some time to do other things or rest */
    ztimer_sleep(ZTIMER_USEC, us);
}

__attribute__((unused))
static void yield_wait(uint32_t unused)
{
    (void) unused;
    /* do not wait just yield */
    thread_yield();
}

__attribute__((unused))
static void no_wait(uint32_t unused)
{
    (void) unused;
    /* do not wait */
}

/* worker_config is a small configuration structure for the thread_worker */
struct worker_config {
    void (*waitfn)(uint32_t);  /**< the resting strategy */
    uint32_t workload;         /**< the amount of work to do per set */
};

/*
 * the following are threads that count and wait with different strategies and
 * print their current count in steps.
 * the ration of active (doing hard work like checking the timer)
 * to passive (wait to be informed when a certain time is there) waiting
 * is determined by there value given to the thread.
 * no_wait and yield_wait threads are restless an therefore never pause.
 */

void * thread_worker(void * d)
{
    nice_wait(200 *  US_PER_MS);  /* always be nice at start */
#ifdef DEVELHELP
    const char *name = thread_get_active()->name;
#else
    int16_t pid = thread_getpid();
#endif

    uint32_t w = 0;
    struct worker_config *wc =  d;
    /* Each set consists of STEPS_PER_SET steps which are divided into work (busy waiting)
     * and resting.
     * E.g. if there are 10 steps per set, the maximum workload is 10, which means no rest.
     * If the given value is out of range work ratio is set to half of STEPS_PER_SET */
    uint32_t work = wc->workload;
    if (work > STEPS_PER_SET) {
        work = STEPS_PER_SET / 2;
    }
    uint32_t rest = (STEPS_PER_SET - work);
    uint32_t step = 0;

    /* work some time and rest */
    for (;;) {
        if (w - step >= PRINT_STEPS) {
#ifdef DEVELHELP
            printf("%s: %" PRIu32 ", %" PRIu32 "\n", name, w, work);
#else
            printf("T-Pid %i:%" PRIu32 ", %" PRIu32 "\n", pid, w, work);
#endif
            step = w;
        }
        do_work(work * WORK_SCALE);
        w += work;
        wc->waitfn(rest * WORK_SCALE);
    }
}
/*
 * nice_wait ->  a thread does nice breaks giving other threads time to do something
 * bad_wait ->   a thread that waits by spinning (intensely looking at the clock)
 * yield_wait -> a restless thread that yields before continuing with the next work package
 * no_wait ->    a restless thread always working until it is preempted
 */
/* yield_wait and nice_wait threads are able to work in "parallel" without sched_round_robin */

#ifndef  THREAD_1
#define  THREAD_1 {no_wait, 5}
#endif

#ifndef  THREAD_2
#define  THREAD_2 {no_wait, 5}
#endif

#ifndef  THREAD_3
#define  THREAD_3 {no_wait, 5}
#endif

/*a TINY Stack should be enough*/
#ifndef WORKER_STACKSIZE
#define WORKER_STACKSIZE (THREAD_STACKSIZE_TINY+THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

int main(void)
{
    {
        static char stack[WORKER_STACKSIZE];
        static struct worker_config wc = THREAD_1;   /* 0-10 workness */
        thread_create(stack, sizeof(stack), 7, 0,
                      thread_worker, &wc, "T1");
    }
    {
        static char stack[WORKER_STACKSIZE];
        static struct worker_config wc = THREAD_2;   /* 0-10 workness */
        thread_create(stack, sizeof(stack), 7, 0,
                      thread_worker, &wc, "T2");
    }
    {
        static char stack[WORKER_STACKSIZE];
        static struct worker_config wc = THREAD_3;   /* 0-10 workness */
        thread_create(stack, sizeof(stack), 7, 0,
                      thread_worker, &wc, "T3");
    }
}
