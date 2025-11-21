/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief ps schedstatistics test app
 *
 * @author Vincent Dupont <vincent@otakeys.com>
 * @author Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include <shell.h>
#include <thread.h>
#include <ztimer.h>

#include "test_utils/interactive_sync.h"

#define NB_THREADS  (5U)

static char stacks[NB_THREADS][THREAD_STACKSIZE_DEFAULT];
static kernel_pid_t pids[NB_THREADS];

static void *_thread_fn(void *arg)
{
    int next = ((uintptr_t)arg + 1) % NB_THREADS;

    printf("Creating thread #%" PRIuPTR ", next=%d\n", (uintptr_t)arg, next);

    while (1) {
        msg_t m1, m2;
        msg_receive(&m1);
        /* generate different loads per thead */
        for (int i = 0; i < (100 * (next + 1)); ++i) {
            volatile uint32_t now = ztimer_now(ZTIMER_USEC);
            (void)now;
        }
        ztimer_sleep(ZTIMER_USEC, 1000); /* Sleep for a bit */
        msg_send(&m2, pids[next]);
    }

    return NULL;
}

int main(void)
{
    test_utils_interactive_sync();

    for (uintptr_t i = 0; i < NB_THREADS; ++i) {
        pids[i] = thread_create(stacks[i], sizeof(stacks[i]),
                                THREAD_PRIORITY_MAIN - 1,
                                0,
                                _thread_fn, (void *)i, "thread");
    }
    /* sleep for a second, so that `ps` shows some % on idle at the beginning */
    ztimer_sleep(ZTIMER_SEC, 1);

    msg_t msg;
    msg_send(&msg, pids[0]);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
}
