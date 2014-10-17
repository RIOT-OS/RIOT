/*
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

char snd_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];

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

    thread_create(snd_thread_stack, sizeof(snd_thread_stack), PRIORITY_MAIN,
                  CREATE_WOUT_YIELD, snd_thread, NULL, "snd");

    puts("yield 1");
    thread_yield();
    puts("yield 2");
    thread_yield();
    puts("done");

    return 0;
}
