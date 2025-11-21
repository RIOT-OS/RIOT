/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

void * t1_t2_malloc_func(void *arg)
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

void * t1_t2_realloc_func(void *arg)
{
    (void)arg;
    while (atomic_load(&is_running)) {
        int *chunk = realloc(NULL, sizeof(int) * 1);
        expect(chunk);
        /* cppcheck-suppress memleakOnRealloc
         * no need to free data on allocation failure, as expect() terminates then anyway */
        chunk = realloc(chunk, sizeof(int) * 2);
        expect(chunk);
        /* cppcheck-suppress memleakOnRealloc
         * no need to free data on allocation failure, as expect() terminates then anyway */
        chunk = realloc(chunk, sizeof(int) * 4);
        expect(chunk);
        /* cppcheck-suppress memleakOnRealloc
         * no need to free data on allocation failure, as expect() terminates then anyway */
        chunk = realloc(chunk, sizeof(int) * 8);
        expect(chunk);
        free(chunk);
    }

    return NULL;
}

int main(void)
{
    kernel_pid_t t1, t2;
    int failed = 0;
    puts(
        "Test Application for multithreaded use of malloc()\n"
        "==================================================\n"
        "\n"
        "This test will run duelling threads allocating and freeing memory.\n"
        "The running thread is interrupted every millisecond and the other\n"
        "threads gets scheduled. Eventually, this should yield to memory\n"
        "corruption unless proper guards are in place preventing them. After\n"
        "ca. two seconds without crash, the test is considered as passing.\n"
    );

#ifndef NO_MALLINFO
    /* in case the malloc implementation dynamically allocates management structures,
     * do one malloc() / free() to obtain the baseline for mallinfo()
     */
    free(malloc(sizeof(int)));
    struct mallinfo pre = mallinfo();
#else
    puts("WARNING: Use of mallinfo() disabled.\n");
#endif

    void *(* const funcs[])(void *) = { t1_t2_malloc_func, t1_t2_realloc_func };
    const char *tests[] = { "malloc()/free()", "realloc()/free()" };

    for (size_t i = 0; i < ARRAY_SIZE(funcs); i++) {
        printf("Testing: %s\n", tests[i]);
        t1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN + 1,
                           0, funcs[i], NULL, "t1");
        t2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN + 1,
                           0, funcs[i], NULL, "t2");
        expect((t1 != KERNEL_PID_UNDEF) && (t2 != KERNEL_PID_UNDEF));

        for (uint16_t i = 0; i < 2 * MS_PER_SEC; i++) {
            xtimer_usleep(US_PER_MS);
            /* shuffle t1 and t2 in their run queue. This should eventually hit
             * during a call to malloc() or free() and disclose any missing
             * guards */
            sched_runq_advance(THREAD_PRIORITY_MAIN + 1);
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
        if (pre.uordblks != post.uordblks) {
            failed = 1;
            puts("Not all blocks were correctly freed!");
            printf("mallinfo().uordblks before test: %u, after test: %u\n",
                   (unsigned)pre.uordblks, (unsigned)post.uordblks);
        }
#endif
    }

    /* cppcheck-suppress knownConditionTrueFalse
     * The actual test is that this application doesn't crash / hang due to memory corruptions.
     * But if there is mallinfo() provided, we can also test for memory leaks in the malloc()
     * implementation pretty much for free. Even if this implementation most likely comes from
     * the standard C lib, it is still good to be at least aware of bugs in the used toolchain */
    if (failed) {
        puts("TEST FAILED");
    }
    else {
        puts("TEST PASSED");
    }

    return 0;
}
