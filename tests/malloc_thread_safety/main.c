/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for checking whether malloc is thread-safe
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdint.h>
/* keep stdatomic.h after stdint.h for buggy toolchains */
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#include "architecture.h"
#include "clist.h"
#include "sched.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "xtimer.h"

#ifndef NO_MALLINFO
#include <malloc.h>
#endif

static char WORD_ALIGNED t1_stack[THREAD_STACKSIZE_SMALL];
static char WORD_ALIGNED t2_stack[THREAD_STACKSIZE_SMALL];
static atomic_uint_least8_t is_running = ATOMIC_VAR_INIT(1);

void * t1_t2_func(void *arg)
{
    (void)arg;
    while (atomic_load(&is_running)) {
        int *chunk1 = malloc(sizeof(int) * 1);
        int *chunk2 = malloc(sizeof(int) * 2);
        int *chunk3 = malloc(sizeof(int) * 4);
        int *chunk4 = malloc(sizeof(int) * 8);
        expect(chunk1 && chunk2 && chunk3 && chunk4);
        free(chunk1);
        free(chunk2);
        free(chunk3);
        free(chunk4);
    }

    return NULL;
}

static void evil_schedule_hack_dont_do_this_at_home(uint8_t prio)
{
    extern clist_node_t sched_runqueues[SCHED_PRIO_LEVELS];
    clist_lpoprpush(&sched_runqueues[prio]);
}

int main(void)
{
    kernel_pid_t t1, t2;
    puts(
        "Test Application for multithreaded use of malloc()\n"
        "==================================================\n"
        "\n"
        "This test will run duelling threads allocating and freeing memory.\n"
        "The running thread is interrupted every millisecond and the other\n"
        "threads get's scheduled. Eventually, this should yield to memory\n"
        "corruption unless proper guards are in place preventing them. After\n"
        "ca. two seconds without crash, the test is considered as passing.\n"
    );

#ifndef NO_MALLINFO
    struct mallinfo pre = mallinfo();
#else
    puts("WARNING: Use of mallinfo() disabled.\n");
#endif

    t1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_STACKTEST, t1_t2_func, NULL, "t1");
    t2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_STACKTEST, t1_t2_func, NULL, "t2");
    expect((t1 != KERNEL_PID_UNDEF) && (t2 != KERNEL_PID_UNDEF));

    for (uint16_t i = 0; i < 2 * MS_PER_SEC; i++) {
        xtimer_usleep(US_PER_MS);
        /* shuffle t1 and t2 in their run queue. This should eventually hit
         * during a call to malloc() or free() and disclose any missing
         * guards */
        evil_schedule_hack_dont_do_this_at_home(THREAD_PRIORITY_MAIN + 1);
    }

    /* Don't keep threads spinning */
    atomic_store(&is_running, 0);
    /* Give threads time to terminate */
    xtimer_usleep(10 * US_PER_MS);

#ifndef NO_MALLINFO
    struct mallinfo post = mallinfo();

    /* RIOT's board or arch support hopefully doesn't use malloc, so there
     * should be zero bytes allocated prior to the first call to malloc() in
     * this test. But let's be forgiving and just expect that the number of
     * allocated bytes before and after the test is equal.
     */
    expect(pre.uordblks == post.uordblks);
#endif
    puts("TEST PASSED");

    return 0;
}
