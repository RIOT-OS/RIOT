/*
 * Copyright (C) 2013 INRIA
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
 * @brief Thread test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"

#include "test_utils/interactive_sync.h"

char t2_stack[THREAD_STACKSIZE_MAIN];

void *second_thread(void *arg)
{
    (void) arg;
    puts("second thread\n");
    return NULL;
}

int main(void)
{
    test_utils_interactive_sync();

    (void) thread_create(
            t2_stack, sizeof(t2_stack),
            THREAD_PRIORITY_MAIN - 1,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            second_thread, NULL, "nr2");
    puts("first thread\n");
    return 0;
}
