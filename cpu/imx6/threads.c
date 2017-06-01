/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_imx6
 * @{
 *
 * @file        threads.c
 * @brief       Implementation of the kernels thread interface
 *
 * TODO needs to be revised
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon George <l.george@fu-berlin.de>
 *
 * @}
 */

void cpu_switch_context_exit(void)
{
    /* TODO */
}

int thread_yield(void)
{
    /* TODO */
    return 0;
}

char *thread_stack_init(void *task_func, void *stack_start, int stack_size)
{
    /* TODO */
    return 0;
}

unsigned int atomic_set_return(unsigned int *p, unsigned int uiVal)
{
    /* TODO */
    return 0;
}
