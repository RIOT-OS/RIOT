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
#include <thread.h>
#include <flags.h>
#include <kernel.h>

char t2_stack[KERNEL_CONF_STACKSIZE_MAIN];

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
            PRIORITY_MAIN - 1, CREATE_WOUT_YIELD | CREATE_STACKTEST,
            second_thread, NULL, "nr2");
    puts("first thread\n");
    return 0;
}
