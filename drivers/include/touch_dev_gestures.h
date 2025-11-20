/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_touch_dev_gestures Touch device gesture recognition
 * @ingroup     drivers_misc
 *
 * @brief       Gesture recognition for touch devices
 * @{
 *
 * This driver implements a simple gesture recognition with a maximum of two
 * touches for touch devices that use the generic touch device API.
 *
 * The application that receives the events from the touch device via the
 * callback function registered with @ref touch_dev_set_touch_event_callback
 * must first create and initialize a touch device gesture context of type
 * @ref touch_dev_gesture_ctx_t. For each touch event received from the
 * touch device, it then calls @ref touch_dev_recognize_gesture function
 * with this context so that the gesture recognition fetches the data from
 * the touch device to detect the gestures, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * static void _touch_event_cb(void *arg)
 * {
 *     // indicate that a touch event occurred
 *     mutex_unlock(arg);
 * }
 *
 * void *_input_task(void *arg)
 * {
 *     ...
 *     mutex_t lock = MUTEX_INIT_LOCKED;
 *
 *     touch_dev_t *dev = touch_dev_reg_find_screen(0).dev;
 *     touch_dev_gesture_ctx_t ctx;
 *
 *     // set the event callback function and initialize the touch device gesture context
 *     touch_dev_set_touch_event_callback(dev, _touch_event_cb, &lock);
 *     touch_dev_init_gesture(dev, &ctx);
 *
 *     while (1) {
 *         // wait for the indication of a touch event
 *         mutex_lock(&lock);
 *
 *         // call the gesture recognition
 *         touch_t pos;
 *         touch_dev_gesture_t gesture = touch_dev_recognize_gesture(&ctx, &pos);
 *
 *         // process recognized gestures
 *         switch (gesture) {
 *             case TOUCH_DEV_GEST_SINGLE_TAP:
 *                 if ((pos.x == 50) && (pos.y == 75)) {
 *                 ...
 *             case TOUCH_DEV_GEST_DOUBLE_TAP:
 *                 ...
 *             ...
 *         }
 *     }
 *     return NULL;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note To use this event-driven approach the driver for the touch device
 *       has to report the following touch events by interrupt:
 *       - a new touch is detected,
 *       - a touch is released, and
 *       - regularly the current touch positions as long as there are touches.
 *
 * If the event-driven approach cannot be used because either the touch
 * device does not support all these touch events or the application wants
 * to use the touch device in polling mode, the application must call the
 * @ref touch_dev_recognize_gesture function with the gesture context
 * of the touch device at regular intervals, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * #ifndef TOUCH_DEV_POLLING_PERIOD
 * #define TOUCH_DEV_POLLING_PERIOD     50
 * #endif
 * ...
 *
 * void *_input_task(void *arg)
 * {
 *     ...
 *
 *     touch_dev_t *dev = touch_dev_reg_find_screen(0).dev;
 *     touch_dev_gesture_ctx_t ctx;
 *
 *     // initialize the touch device gesture context
 *     touch_dev_init_gesture(dev, &ctx);
 *
 *     while (1) {
 *         // call the gesture recognition
 *         touch_t pos;
 *         touch_dev_gesture_t gesture = touch_dev_recognize_gesture(&ctx, &pos);
 *
 *         // process recognized gestures
 *         switch (gesture) {
 *             case TOUCH_DEV_GEST_SINGLE_TAP:
 *                 if ((pos.x == 50) && (pos.y == 75)) {
 *                 ...
 *             case TOUCH_DEV_GEST_DOUBLE_TAP:
 *                 ...
 *             ...
 *         }
 *
 *         // wait the period time for polling
 *         ztimer_sleep(ZTIMER_MSEC, TOUCH_DEV_POLLING_PERIOD);
 *     }
 *     return NULL;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The following gestures are supported by the driver:
 *
 * - @ref TOUCH_DEV_GEST_SINGLE_TAP : a single tap with one touch at the given position
 * - @ref TOUCH_DEV_GEST_DOUBLE_TAP : a double tap with one touch at the given position
 * - @ref TOUCH_DEV_GEST_PRESSED : a long press with one touch at the given position
 * - @ref TOUCH_DEV_GEST_RELEASED : one touch released after a long press at given position
 * - @ref TOUCH_DEV_GEST_MOVE : moving while pressed with current position
 * - @ref TOUCH_DEV_GEST_SWIPE_LEFT : swiping left with one touch
 * - @ref TOUCH_DEV_GEST_SWIPE_RIGHT : swiping right with one touch
 * - @ref TOUCH_DEV_GEST_SWIPE_UP : swiping up with one touch
 * - @ref TOUCH_DEV_GEST_SWIPE_DOWN : swiping down with one touch
 * - @ref TOUCH_DEV_GEST_ZOOM_IN : zooming in (spreading) with two touches
 * - @ref TOUCH_DEV_GEST_ZOOM_OUT : zooming out (pinching) with two touches
 *
 * @note
 * - For technical reasons, a double-tap event is always preceded by a
 *   single-tap event, i.e. for a double-tap, the application always
 *   receives a single-tap event first and then a double-tap event if
 *   the second tap follows.
 * - Zooming gestures are only available if the touch device supports two
 *   touches.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "touch_dev.h"

/**
 * @brief   Maximum number of touches supported by gesture recognition
 */
#define TOUCH_DEV_TOUCHES_MAX_NUMOF     2

/**
 * @brief   Minimum distance in one direction to recognize a swipe gesture
 */
#ifndef CONFIG_TOUCH_DEV_SWIPE_TRESH
#define CONFIG_TOUCH_DEV_SWIPE_TRESH    5
#endif

/**
 * @brief   Minimum touch time in milliseconds to detect a long press gesture
 */
#ifndef CONFIG_TOUCH_DEV_PRESS_TIME_MS
#define CONFIG_TOUCH_DEV_PRESS_TIME_MS  600
#endif

/**
 * @brief   Maximum time in milliseconds between two taps to detect a double tap
 */
#ifndef CONFIG_TOUCH_DEV_DOUBLE_TIME_MS
#define CONFIG_TOUCH_DEV_DOUBLE_TIME_MS 400
#endif

/**
 * @brief   Touch device states used for gesture recognition
 */
typedef enum {
    TOUCH_DEV_STATE_RELEASED,           /**< no touches detected, default state */
    TOUCH_DEV_STATE_TAPPED_SINGLE,      /**< a single touch is detected */
    TOUCH_DEV_STATE_TAPPED_MULTIPLE,    /**< a second touch is detected */
    TOUCH_DEV_STATE_PRESSED,            /**< a long press is detected */
    TOUCH_DEV_STATE_WAIT_FOR_RELEASE,   /**< gesture detected, waiting for releasing touches */
} touch_dev_state_t;

/**
 * @brief   Touch gesture events
 */
typedef enum {
    TOUCH_DEV_GEST_NONE,         /**< No gesture recognized */
    TOUCH_DEV_GEST_SINGLE_TAP,   /**< Single tap recognized at the given position */
    TOUCH_DEV_GEST_DOUBLE_TAP,   /**< Double tap recognized at the given position */
    TOUCH_DEV_GEST_PRESSED,      /**< Long press recognized at the given position */
    TOUCH_DEV_GEST_RELEASED,     /**< Release after a long press at given position */
    TOUCH_DEV_GEST_MOVE,         /**< Moving while pressed recognized, current position is given */
    TOUCH_DEV_GEST_SWIPE_LEFT,   /**< Swipe left recognized, no position is given */
    TOUCH_DEV_GEST_SWIPE_RIGHT,  /**< Swipe right recognized, no position is given */
    TOUCH_DEV_GEST_SWIPE_UP,     /**< Swipe up recognized, no position is given */
    TOUCH_DEV_GEST_SWIPE_DOWN,   /**< Swipe down recognized, no position is given */
    TOUCH_DEV_GEST_ZOOM_IN,      /**< Zoom in (spread) recognized, no position is given */
    TOUCH_DEV_GEST_ZOOM_OUT,     /**< Zoom out (pinch) recognized, no position is given */
} touch_dev_gesture_t;

/**
 * @brief    Context information for a touch device needed for gesture recognition
 */
typedef struct {
    touch_dev_t *dev;                           /**< Pointer to the touch device */
    uint32_t t_changed;                         /**< Time of last state change in ms */
    uint32_t t_prev_tap;                        /**< Time of previous tap */
    touch_t prev[TOUCH_DEV_TOUCHES_MAX_NUMOF];  /**< Previous set of touches */
    uint8_t prev_num;                           /**< Previous number of touches */
    touch_dev_state_t state;                    /**< State of touch device */
} touch_dev_gesture_ctx_t;

/**
 * @brief   Initialize gesture recognition
 *
 * @param[in] dev       Pointer to the touch device
 * @param[in] ctx       Pointer to the context information for the touch device
 */
void touch_dev_init_gesture(touch_dev_t *dev,
                            touch_dev_gesture_ctx_t *ctx);

/**
 * @brief    Recognize gestures by handling next touch device event
 *
 * @param[in]  ctx      Pointer to the context information for the touch device
 * @param[out] pos      Position of the gesture if interested in it (can be NULL)
 *
 * return the gesture of type @ref touch_dev_gesture_t if one was
 *        recognized or @ref TOUCH_DEV_GEST_NONE
 */
touch_dev_gesture_t touch_dev_recognize_gesture(touch_dev_gesture_ctx_t *ctx,
                                                touch_t *pos);

#ifdef __cplusplus
}
#endif

/** @} */
