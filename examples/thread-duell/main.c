#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <thread.h>
#include <sched.h>

#include <math.h>

#include "xtimer.h"

#define PRINT_STEPS 10
#define WORK_SCALE  1000

void bad_wait(uint64_t us)
{
    /*keep the cpu busy waiting for some time to pass simulate working*/
    uint64_t until = xtimer_now_usec64() + us;

    while (xtimer_now_usec64() < until) {
        /*do nothing*/
    }
}

void nice_wait(uint64_t us)
{
    /*be nice give the core some time to do other things or rest*/
    xtimer_usleep64(us);
}

void * thread_nicebreaks(void * d)
{
    xtimer_usleep64(200 * 1000);  /*allways be nice at start*/
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
            printf(
#ifdef DEVELHELP
                "%s: "
#else
                "T-Pid %i:"
#endif
                "%u, %d\n",
#ifdef DEVELHELP
                name,
#else
                pid,
#endif
                w, work);
            step = w;
        }
        bad_wait(work * WORK_SCALE);
        w += work;
        nice_wait(rest * WORK_SCALE);
    }
}


void * thread_bad(void * d)
{
    xtimer_usleep64(200 * 1000);  /*allways be nice at start*/
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
            printf(
#ifdef DEVELHELP
                "%s: "
#else
                "T-Pid %i:"
#endif
                "%u, %d\n",
#ifdef DEVELHELP
                name,
#else
                pid,
#endif
                w, work);
            step = w;
        }
        bad_wait(work * WORK_SCALE);
        w += work;
        bad_wait(rest * WORK_SCALE);
    }
}

void * thread_restless_yielding(void * d)
{
    xtimer_usleep64(200 * 1000);  /*allways be nice at start*/
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
            printf(
#ifdef DEVELHELP
                "%s: "
#else
                "T-Pid %i:"
#endif
                "%u, %d\n",
#ifdef DEVELHELP
                name,
#else
                pid,
#endif
                w, work);
            step = w;
        }
        bad_wait(work * WORK_SCALE);
        w += work;
        thread_yield();
    }
}


void * thread_restless(void * d)
{
    xtimer_usleep64(200 * 1000);  /*allways be nice at start*/
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
            printf(
#ifdef DEVELHELP
                "%s: "
#else
                "T-Pid %i:"
#endif
                "%u, %d\n",
#ifdef DEVELHELP
                name,
#else
                pid,
#endif
                w, work);
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
