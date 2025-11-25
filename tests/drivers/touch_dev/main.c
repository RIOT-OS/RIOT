/*
 * Copyright (C) 2020 Inria
 *               2023 Gunar Schorcht
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
 * @brief       Generic touch device test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdio.h>
#include <stdbool.h>

#include "mutex.h"
#include "ztimer.h"

#include "touch_dev.h"

#if IS_USED(MODULE_STMPE811)
#include "stmpe811.h"
#include "test_utils/expect.h"
#endif

#ifndef TOUCH_DEV_POLLING_PERIOD
#define TOUCH_DEV_POLLING_PERIOD     50
#endif

#if !IS_ACTIVE(TOUCH_DEV_POLLING_MODE)
static void _touch_event_cb(void *arg)
{
    mutex_unlock(arg);
}
#endif

int main(void)
{
    /* Use the first screen */
    touch_dev_reg_t *touch_dev = touch_dev_reg_find_screen(0);
    if (!touch_dev) {
        puts("No screen found!");
        return -1;
    }

#if !IS_ACTIVE(TOUCH_DEV_POLLING_MODE)
    mutex_t lock = MUTEX_INIT_LOCKED;

    touch_dev_set_touch_event_callback(touch_dev->dev, _touch_event_cb, &lock);
#endif

#if IS_USED(MODULE_STMPE811)
    uint16_t xmax = touch_dev_width(touch_dev->dev);
    uint16_t ymax = touch_dev_height(touch_dev->dev);

    stmpe811_t *stmpe811 = (stmpe811_t *)touch_dev->dev;
    expect(xmax == stmpe811->params.xmax);
    expect(ymax == stmpe811->params.ymax);
#endif

    uint8_t last_touches = touch_dev_touches(touch_dev->dev, NULL, 1);

    while (1) {
#if IS_ACTIVE(TOUCH_DEV_POLLING_MODE)
        ztimer_sleep(ZTIMER_MSEC, TOUCH_DEV_POLLING_PERIOD);
#else
        /* wait for event */
        mutex_lock(&lock);
#endif
        touch_t touches[1];
        uint8_t current_touches = touch_dev_touches(touch_dev->dev, touches, 1);

        if (current_touches != last_touches) {
            if (current_touches == 0) {
                puts("Event: released!");
            }
            if (current_touches > 0) {
                puts("Event: pressed!");
            }
            last_touches = current_touches;
        }

        /* Display touch position if pressed */
        if (current_touches == 1) {
            printf("X: %i, Y:%i\n", touches[0].x, touches[0].y);
        }
    }

    return 0;
}
