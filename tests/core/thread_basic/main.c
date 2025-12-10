/*
 * SPDX-FileCopyrightText: 2013 INRIA
 * SPDX-License-Identifier: LGPL-2.1-only
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

char t2_stack[THREAD_STACKSIZE_MAIN];

void *second_thread(void *arg)
{
    (void) arg;
    puts("second thread\n");
    return NULL;
}

int main(void)
{
    (void) thread_create(
            t2_stack, sizeof(t2_stack),
            THREAD_PRIORITY_MAIN - 1,
            THREAD_CREATE_WOUT_YIELD,
            second_thread, NULL, "nr2");
    puts("first thread\n");
    return 0;
}
