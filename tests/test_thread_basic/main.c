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

#define STACK_SIZE  (KERNEL_CONF_STACKSIZE_DEFAULT + KERNEL_CONF_STACKSIZE_MAIN)

char t2_stack[STACK_SIZE];

void second_thread(void)
{
    puts("second thread\n");
}

int main(void)
{
    (void) thread_create(t2_stack, STACK_SIZE, PRIORITY_MAIN - 1, CREATE_WOUT_YIELD | CREATE_STACKTEST, second_thread, "nr2");
    puts("first thread\n");
    return 0;
}
