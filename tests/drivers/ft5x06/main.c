/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the ft5x06 touch driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "mutex.h"

#include "ft5x06.h"
#include "ft5x06_params.h"

static void _touch_event_cb(void *arg)
{
    mutex_unlock(arg);
}

static ft5x06_touch_position_t positions[FT5X06_TOUCHES_COUNT_MAX];

int main(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    ft5x06_t dev;

    puts("FT5x06 test application\n");

    printf("+------------Initializing------------+\n");
    int ret = ft5x06_init(&dev, &ft5x06_params[0], _touch_event_cb, &lock);
    if (ret != 0) {
        puts("[Error] Initialization failed");
        return 1;
    }

    puts("Initialization successful");

    uint8_t current_touch_count = 0;
    ft5x06_read_touch_count(&dev, &current_touch_count);
    uint8_t last_touch_count = current_touch_count;

    while (1) {

        /* wait for touch event */
        mutex_lock(&lock);

        ft5x06_read_touch_count(&dev, &current_touch_count);

        if (current_touch_count != last_touch_count) {
            if (current_touch_count) {
                printf("%d touch detected\n", current_touch_count);
            }
            if (!current_touch_count) {
                puts("Released!");
            }
            last_touch_count = current_touch_count;
        }

        ft5x06_touch_gesture_t gesture;
        ft5x06_read_touch_gesture(&dev, &gesture);
        if (gesture != FT5X06_TOUCH_NO_GESTURE) {
            printf("Gesture detected: %d\n", gesture);
        }

        /* Display touch positions if there are some */
        if (current_touch_count > 0) {
            ft5x06_read_touch_positions(&dev, positions, current_touch_count);
            for (uint8_t touch = 0; touch < current_touch_count; touch++) {
                printf("Touch %d - X: %i, Y:%i\n",
                        touch + 1, positions[touch].x, positions[touch].y);
            }
        }
    }

    return 0;
}
