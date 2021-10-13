/*
 * Copyright (C) 2021 Inria
 *               2021 Freie Universität Berlin
 *               2021 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   Application for testing thread_yield_higher() with IRQs disabled
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "irq.h"
#include "thread.h"

static char t2_stack[THREAD_STACKSIZE_DEFAULT];

static void *second_thread(void *arg)
{
    puts("2. second thread scheduled");
    return arg;
}

int main(void)
{
    thread_create(
        t2_stack, sizeof(t2_stack),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
        second_thread, NULL, "nr2");

    irq_disable();
    thread_yield_higher();
    puts("1. post yield");
    irq_enable();
    puts("3. post irq enable");

    puts("if above messages are in order, this test succeeded.");

    return 0;
}
