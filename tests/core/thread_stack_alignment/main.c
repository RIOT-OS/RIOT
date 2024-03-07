/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
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
 * @brief Thread stack alignment test application
 *
 * @author Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* work around bug with LLVM: <stdatomic.h> needs <stdint.h> included first,
 * so do not sort this one alphabetically */
#include <stdatomic.h>

#include "architecture.h"
#include "irq.h"
#include "sched.h"
#include "thread.h"
#include "thread_arch.h"

#define PI_ROUNDED  3.14159
#define _QUOTE(x)   #x
#define QUOTE(x)    _QUOTE(x)
#define ALIGNMENT   128
#define STACKSIZE   (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF \
        + ALIGNMENT)

static char alignas(ALIGNMENT) stack[STACKSIZE + ALIGNMENT];
static atomic_bool test_failed = ATOMIC_VAR_INIT(false);

static void *thread_func(void *arg)
{
    (void)arg;
    static const double pi_const = PI_ROUNDED;
    static const char pi_str[] = QUOTE(PI_ROUNDED);

    /* Force compiler to not optimize out the heavy lifting by loading the
     * value of pi with a "volatile" read. The compiler must assume that the
     * contents of pi are only known at runtime */
    double pi = (volatile const double)pi_const;

    char buf[16] = "";
    /* Since snprintf() has a variable number of arguments, arguments are
     * typically passed via stack even if the calling convention would pass
     * arguments via registers most of the time. And typically double has
     * the highest alignment requirement, so this is likely to run into
     * issues when the stack is not aligned as the target arch requires it */
    snprintf(buf, sizeof(buf) - 1, "%1.5f", pi);

    if (0 != memcmp(pi_str, buf, sizeof(pi_str))) {
        atomic_store(&test_failed, true);
        puts("FAILED");
        printf("Got \"%s\", expected \"%s\"\n", buf, pi_str);
        return NULL;
    }

    puts("OK");
    return NULL;
}

int main(void)
{
    bool failed = 0;
    printf("Testing with a stack sized %u and an alignment up to %u\n",
           (unsigned)STACKSIZE, (unsigned)ALIGNMENT);
    for (size_t i = 0; i < ALIGNMENT; i++) {
        atomic_store(&test_failed, false);
        printf("Testing for alignment %" PRIuSIZE ": ", i);
        kernel_pid_t p;
        p = thread_create(stack + i, STACKSIZE, THREAD_PRIORITY_MAIN - 1,
                          0,
                          thread_func, NULL, "test");
        /* we expect that the new thread is scheduled to directly after it is
         * created and this will only continue one the thread has terminated.
         * But let's better be safe than sorry */
        while (thread_get(p) != NULL) {
            thread_yield();
        }

        if (atomic_load(&test_failed)) {
            failed = true;
        }
    }

    if (failed) {
        puts("TEST FAILED");
    }
    else {
        puts("TEST PASSED");
    }
    return 0;
}
