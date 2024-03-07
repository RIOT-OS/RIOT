/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Freie Universität Berlin
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Module creating a dummy thread
 *
 * This module can be used to mess up the number of threads a bit, e.g., for
 * testing test scripts.
 *
 * This module can be used by manually adding it to the command line when
 * building, e.g.,
 *
 *     USEMODULE+=dummy_thread make -C tests/core/rmutex
 *     make -C tests/core/rmutex test
 *
 * Note how the output of the test changes compared to a build without
 * dummy_thread.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "thread.h"

static char _dummy_stack[THREAD_STACKSIZE_IDLE];

static void *_dummy_thread(void *arg)
{
    (void)arg;
    while (1) {
        thread_sleep();
    }

    return NULL;
}

void dummy_thread_create(void)
{
    thread_create(_dummy_stack, sizeof(_dummy_stack),
                  THREAD_PRIORITY_IDLE,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_SLEEPING,
                  _dummy_thread, NULL, "dummy");
}
