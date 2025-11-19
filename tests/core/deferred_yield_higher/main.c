/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2021 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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
        THREAD_CREATE_WOUT_YIELD,
        second_thread, NULL, "nr2");

    irq_disable();
    thread_yield_higher();
    puts("1. post yield");
    irq_enable();
    puts("3. post irq enable");

    puts("if above messages are in order, this test succeeded.");

    return 0;
}
