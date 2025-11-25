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
#include "touch_dev_gestures.h"

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
    touch_dev_reg_t *touch_reg = touch_dev_reg_find_screen(0);
    if (!touch_reg) {
        puts("No screen found!");
        return -1;
    }

    touch_dev_t *dev = touch_reg->dev;
    touch_dev_gesture_ctx_t ctx;

    touch_dev_init_gesture(dev, &ctx);

#if !IS_ACTIVE(TOUCH_DEV_POLLING_MODE)
    mutex_t lock = MUTEX_INIT_LOCKED;
    touch_dev_set_touch_event_callback(dev, _touch_event_cb, &lock);
#endif

    while (1) {

#if IS_ACTIVE(TOUCH_DEV_POLLING_MODE)
        ztimer_sleep(ZTIMER_MSEC, TOUCH_DEV_POLLING_PERIOD);
#else
        /* wait for event */
        mutex_lock(&lock);
#endif

        touch_t pos;
        touch_dev_gesture_t gesture = touch_dev_recognize_gesture(&ctx, &pos);

        switch (gesture) {
        case TOUCH_DEV_GEST_SINGLE_TAP:
            printf("Single Tap X: %u, Y:%u\n", pos.x, pos.y);
            break;
        case TOUCH_DEV_GEST_DOUBLE_TAP:
            printf("Double Tap X: %u, Y:%u\n", pos.x, pos.y);
            break;
        case TOUCH_DEV_GEST_PRESSED:
            printf("Pressed    X: %u, Y:%u\n", pos.x, pos.y);
            break;
        case TOUCH_DEV_GEST_RELEASED:
            printf("Released   X: %u, Y:%u\n", pos.x, pos.y);
            break;
        case TOUCH_DEV_GEST_MOVE:
            printf("Moving     X: %u, Y:%u\n", pos.x, pos.y);
            break;
        case TOUCH_DEV_GEST_SWIPE_LEFT:
            printf("Swipe left\n");
            break;
        case TOUCH_DEV_GEST_SWIPE_RIGHT:
            printf("Swipe right\n");
            break;
        case TOUCH_DEV_GEST_SWIPE_UP:
            printf("Swipe up\n");
            break;
        case TOUCH_DEV_GEST_SWIPE_DOWN:
            printf("Swipe down\n");
            break;
        case TOUCH_DEV_GEST_ZOOM_IN:
            printf("Zoom in (spread)\n");
            break;
        case TOUCH_DEV_GEST_ZOOM_OUT:
            printf("Zoom out (pinch)\n");
            break;
        default:
            break;
        }
    }

    return 0;
}
