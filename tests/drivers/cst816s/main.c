/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the CST816S touch screen driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "cst816s_params.h"
#include "cst816s.h"
#include "thread.h"
#include "thread_flags.h"

#define CST816S_THREAD_FLAG     (1 << 8)
#define CST816S_NUM_TOUCHES     5

static void _cb(void *arg)
{
    kernel_pid_t *pid = arg;
    thread_flags_set((thread_t *)sched_threads[*pid], CST816S_THREAD_FLAG);
}

static void _dump_cst816s(cst816s_t *dev)
{
    puts("Reading data:");
    cst816s_touch_data_t touches;
    if (cst816s_read(dev, &touches) == 0) {
        printf("Touch at %03u, %03u with gesture type \"%s\"\n", touches.x,
               touches.y, cst816s_gesture_str[touches.gesture]);
    }
    else {
        puts("Device not responding");
    }
}

int main(void)
{
    cst816s_t dev;

    kernel_pid_t pid = thread_getpid();

    puts("CST816S test application\n");
    cst816s_init(&dev, &cst816s_params[0], _cb, &pid);

    while (1) {
        thread_flags_t flags = thread_flags_wait_any(CST816S_THREAD_FLAG);
        if (flags & CST816S_THREAD_FLAG) {
            _dump_cst816s(&dev);
        }
    }
    return 0;
}
