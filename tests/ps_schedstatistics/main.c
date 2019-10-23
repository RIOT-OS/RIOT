/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2017 HAW Hamburg
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
#include <xtimer.h>

#define NB_THREADS  (5U)

static char stacks[NB_THREADS][THREAD_STACKSIZE_DEFAULT];
static kernel_pid_t pids[NB_THREADS];

static void *_thread_fn(void *arg)
{
    int next = ((int)arg + 1) % NB_THREADS;

    printf("Creating thread #%d, next=%d\n", (int)arg, next);

    while (1) {
        msg_t m1, m2;
        msg_receive(&m1);
        /* generate different loads per thead */
        for (int i = 0; i < (10 * (next + 1)); ++i) {
            _xtimer_now64();
        }
        xtimer_usleep(XTIMER_BACKOFF * 3);
        msg_send(&m2, pids[next]);
    }

    return NULL;
}

int main(void)
{
    for (unsigned i = 0; i < NB_THREADS; ++i) {
        pids[i] = thread_create(stacks[i], sizeof(stacks[i]),
                                THREAD_PRIORITY_MAIN - 1,
                                THREAD_CREATE_STACKTEST,
                                _thread_fn, (void *)i, "thread");
    }
    /* sleep for a second, so that `ps` shows some % on idle at the beginning */
    xtimer_sleep(1);

    msg_t msg;
    msg_send(&msg, pids[0]);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
}
