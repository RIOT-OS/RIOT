/*
 * SPDX-FileCopyrightText: 2014 Oliver Hahm <oliver.hahm@inria.fr>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test thread_yield()
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include "thread.h"

char snd_thread_stack[THREAD_STACKSIZE_MAIN];

void *snd_thread(void *unused)
{
    (void) unused;
    puts("snd_thread running");
    return NULL;
}

int main(void)
{
    puts("The output should be: yield 1, snd_thread running, yield 2, done");
    puts("----------------------------------------------------------------");

    thread_create(snd_thread_stack, sizeof(snd_thread_stack), THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_WOUT_YIELD, snd_thread, NULL, "snd");

    puts("yield 1");
    thread_yield();
    puts("yield 2");
    thread_yield();
    puts("done");

    return 0;
}
