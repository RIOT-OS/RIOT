/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Thread test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "flags.h"
#include "kernel.h"

char second_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];
char third_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];

void *fourth_thread(void *arg)
{
    (void) arg;
    puts("4th: starting");
    puts("4th: exiting");
    return NULL;
}

void *third_thread(void *arg)
{
    (void) arg;
    puts("3rd: starting");
    puts("3rd: exiting");
    return NULL;
}

void *second_thread(void *arg)
{
    (void) arg;
    puts("2nd: starting");

    if ((thread_create(
             third_thread_stack,
             sizeof(third_thread_stack),
             PRIORITY_MAIN - 2,
             CREATE_STACKTEST,
             third_thread,
             NULL,
             "nr3")
        ) == -1) {
        puts("2nd: Error creating 3rd thread.");
    }

    /* thread should have returned already */

    if ((thread_create(
             third_thread_stack,
             sizeof(third_thread_stack),
             PRIORITY_MAIN - 1,
             CREATE_WOUT_YIELD | CREATE_STACKTEST,
             fourth_thread,
             NULL,
             "nr4")
        ) == -1) {
        puts("2nd: Error creating 4rd thread.");
    }

    puts("2nd: exiting");
    return NULL;
}

int main(void)
{
    puts("main: starting");

    if ((thread_create(
             second_thread_stack,
             sizeof(second_thread_stack),
             PRIORITY_MAIN - 1,
             CREATE_WOUT_YIELD | CREATE_STACKTEST,
             second_thread,
             NULL,
             "nr2")
        ) == -1) {
        puts("main: Error creating 3rd thread.");
    }

    puts("main: exiting");
    return 0;
}
