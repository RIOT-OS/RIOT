/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_touch_dev_gestures
 * @{
 *
 * @file
 * @brief       Gesture recognition for touch devices
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>

#include "macros/utils.h"
#include "ztimer.h"

#include "touch_dev_gestures.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static touch_dev_gesture_t _handle_swipe(const touch_t *prev,
                                         const touch_t *curr)
{
    DEBUG("[touch_dev] @%" PRIu32 " handle swipe (%u, %u) -> (%u, %u): ",
          ztimer_now(ZTIMER_MSEC), prev[0].x, prev[0].y, curr[0].x, curr[0].y);

    assert(curr[0].x != UINT16_MAX);
    assert(curr[0].x != UINT16_MAX);
    assert(prev[0].x != UINT16_MAX);
    assert(prev[0].x != UINT16_MAX);

    uint16_t x_diff = MAX(curr[0].x, prev[0].x) - MIN(curr[0].x, prev[0].x);
    uint16_t y_diff = MAX(curr[0].y, prev[0].y) - MIN(curr[0].y, prev[0].y);

    if ((x_diff > y_diff) && (x_diff > CONFIG_TOUCH_DEV_SWIPE_TRESH)) {
        /* movement in x direction is dominant */
        if (curr[0].x > prev[0].x) {
            DEBUG("swipe right\n");
            return TOUCH_DEV_GEST_SWIPE_RIGHT;
        }
        else {
            DEBUG("swipe left\n");
            return TOUCH_DEV_GEST_SWIPE_LEFT;
        }
    }
    else if (y_diff > CONFIG_TOUCH_DEV_SWIPE_TRESH) {
        /* movement in y direction is dominant */
        if (curr[0].y > prev[0].y) {
            DEBUG("swipe down\n");
            return TOUCH_DEV_GEST_SWIPE_DOWN;
        }
        else {
            DEBUG("swipe up\n");
            return TOUCH_DEV_GEST_SWIPE_UP;
        }
    }

    DEBUG("none\n");
    return TOUCH_DEV_GEST_NONE;
}

static touch_dev_gesture_t _handle_zoom(const touch_t *prev,
                                        const touch_t *curr)
{
    DEBUG("[touch_dev] @%" PRIu32 " handle zoom  "
          "(%u, %u)(%u, %u) -> (%u, %u)(%u, %u): ",
          ztimer_now(ZTIMER_MSEC),
          prev[0].x, prev[0].y, prev[1].x, prev[1].y,
          curr[0].x, curr[0].y, curr[1].x, curr[1].y);

    assert(curr[0].x != UINT16_MAX);
    assert(curr[0].x != UINT16_MAX);
    assert(curr[1].x != UINT16_MAX);
    assert(curr[1].x != UINT16_MAX);
    assert(prev[0].x != UINT16_MAX);
    assert(prev[0].x != UINT16_MAX);
    assert(prev[1].x != UINT16_MAX);
    assert(prev[1].x != UINT16_MAX);

    uint32_t prev_x_diff = MAX(prev[0].x, prev[1].x) - MIN(prev[0].x, prev[1].x);
    uint32_t prev_y_diff = MAX(prev[0].y, prev[1].y) - MIN(prev[0].y, prev[1].y);
    uint32_t curr_x_diff = MAX(curr[0].x, curr[1].x) - MIN(curr[0].x, curr[1].x);
    uint32_t curr_y_diff = MAX(curr[0].y, curr[1].y) - MIN(curr[0].y, curr[1].y);

    uint32_t prev_diff_sqr = (prev_x_diff * prev_x_diff) + (prev_y_diff * prev_y_diff);
    uint32_t curr_diff_sqr = (curr_x_diff * curr_x_diff) + (curr_y_diff * curr_y_diff);

    if (curr_diff_sqr > prev_diff_sqr) {
        DEBUG("zoom in\n");
        return TOUCH_DEV_GEST_ZOOM_IN;
    }
    else {
        DEBUG("zoom out\n");
        return TOUCH_DEV_GEST_ZOOM_OUT;
    }
}

touch_dev_gesture_t touch_dev_recognize_gesture(touch_dev_gesture_ctx_t *ctx,
                                                touch_t *pos)
{
    assert(ctx);

    touch_dev_gesture_t gesture = TOUCH_DEV_GEST_NONE;

    uint8_t max_num = touch_dev_max_numof(ctx->dev);

    /* a maximum of 2 touches are supported */
    max_num = (max_num > TOUCH_DEV_TOUCHES_MAX_NUMOF) ? TOUCH_DEV_TOUCHES_MAX_NUMOF
                                                      : max_num;

    touch_t curr[TOUCH_DEV_TOUCHES_MAX_NUMOF];
    uint8_t curr_num = touch_dev_touches(ctx->dev, curr, max_num);

    /* a maximum of TOUCH_DEV_TOUCHES_MAX_NUMOF touches are supported */
    curr_num = (curr_num > TOUCH_DEV_TOUCHES_MAX_NUMOF) ? 2 : curr_num;

    uint32_t t_now = ztimer_now(ZTIMER_MSEC);

    switch (ctx->state) {
    case TOUCH_DEV_STATE_RELEASED:
        if (curr_num > ctx->prev_num) {
            ctx->state = (curr_num == 1) ? TOUCH_DEV_STATE_TAPPED_SINGLE
                                         : TOUCH_DEV_STATE_TAPPED_MULTIPLE;
            ctx->t_changed = t_now;
            ctx->prev[0] = curr[0];
            ctx->prev[1] = (curr_num == 2) ? curr[1] : TOUCH_DEV_VALUE_INVALID;
            DEBUG("[touch_dev] @%" PRIu32 " tapped, count %u -> %u\n",
                  t_now, ctx->prev_num, curr_num);
        }
        break;

    case TOUCH_DEV_STATE_TAPPED_SINGLE:
        if (curr_num < ctx->prev_num) {
            if (pos) {
                *pos = ctx->prev[0];
            }
            ctx->state = TOUCH_DEV_STATE_RELEASED;
            ctx->t_changed = t_now;
            ctx->prev[0] = TOUCH_DEV_VALUE_INVALID;
            if ((t_now - ctx->t_prev_tap) < CONFIG_TOUCH_DEV_DOUBLE_TIME_MS) {
                DEBUG("[touch_dev] @%" PRIu32 " double tap, count %u -> %u\n",
                      t_now, ctx->prev_num, curr_num);
                gesture = TOUCH_DEV_GEST_DOUBLE_TAP;
                ctx->t_prev_tap = 0;
            }
            else {
                DEBUG("[touch_dev] @%" PRIu32 " single tap, count %u -> %u\n",
                      t_now, ctx->prev_num, curr_num);
                gesture = TOUCH_DEV_GEST_SINGLE_TAP;
                ctx->t_prev_tap = t_now;
            }
        }
        else if (curr_num > ctx->prev_num) {
            ctx->state = TOUCH_DEV_STATE_TAPPED_MULTIPLE;
            ctx->t_changed = t_now;
            ctx->prev[1] = curr[1];
            DEBUG("[touch_dev] @%" PRIu32 " tapped, count %u -> %u\n",
                  t_now, ctx->prev_num, curr_num);
        }
        else if ((t_now - ctx->t_changed) > CONFIG_TOUCH_DEV_PRESS_TIME_MS) {
            if (pos) {
                *pos = curr[0];
            }
            ctx->state = TOUCH_DEV_STATE_PRESSED;
            ctx->t_changed = t_now;
            DEBUG("[touch_dev] @%" PRIu32 " pressed, count %u\n",
                  t_now, curr_num);
            gesture = TOUCH_DEV_GEST_PRESSED;
        }
        else {
            gesture = _handle_swipe(ctx->prev, curr);
            if (gesture != TOUCH_DEV_GEST_NONE) {
                ctx->state = TOUCH_DEV_STATE_WAIT_FOR_RELEASE;
                ctx->t_changed = t_now;
            }
        }
        break;

    case TOUCH_DEV_STATE_TAPPED_MULTIPLE:
        if (curr_num < ctx->prev_num) {
            ctx->state = (curr_num == 1) ? TOUCH_DEV_STATE_WAIT_FOR_RELEASE
                                         : TOUCH_DEV_STATE_RELEASED;
            ctx->t_changed = t_now;
            ctx->prev[0] = (curr_num == 1) ? curr[0] : TOUCH_DEV_VALUE_INVALID;
            ctx->prev[1] = TOUCH_DEV_VALUE_INVALID;
            DEBUG("[touch_dev] @%" PRIu32 " released, count %u -> %u\n",
                  t_now, ctx->prev_num, curr_num);
        }
        else {
            gesture = _handle_zoom(ctx->prev, curr);
            ctx->prev[0] = curr[0];
            ctx->prev[1] = curr[1];
        }
        break;

    case TOUCH_DEV_STATE_PRESSED:
        if (curr_num == 0) {
            if (pos) {
                *pos = ctx->prev[0];
            }
            gesture = TOUCH_DEV_GEST_RELEASED;
            ctx->state = TOUCH_DEV_STATE_RELEASED;
            ctx->t_changed = t_now;
            ctx->prev[0] = TOUCH_DEV_VALUE_INVALID;
            ctx->prev[1] = TOUCH_DEV_VALUE_INVALID;
            DEBUG("[touch_dev] @%" PRIu32 " released, count %u -> %u\n",
                  t_now, ctx->prev_num, curr_num);
        }
        else {
            DEBUG("[touch_dev] @%" PRIu32 " move %u (%u, %u)\n",
                  t_now, curr_num, curr[0].x, curr[0].y);
            if (pos) {
                *pos = curr[0];
            }
            ctx->prev[0] = curr[0];
            gesture = TOUCH_DEV_GEST_MOVE;
        }
        break;

    case TOUCH_DEV_STATE_WAIT_FOR_RELEASE:
        if (curr_num == 0) {
            ctx->state = TOUCH_DEV_STATE_RELEASED;
            ctx->t_changed = t_now;
            ctx->prev[0] = TOUCH_DEV_VALUE_INVALID;
            ctx->prev[1] = TOUCH_DEV_VALUE_INVALID;
            DEBUG("[touch_dev] @%" PRIu32 " released, count %u -> %u\n",
                  t_now, ctx->prev_num, curr_num);
        }
        else {
            DEBUG("[touch_dev] @%" PRIu32 " wait for release, count %u\n",
                  t_now, curr_num);
        }
        break;
    }

    ctx->prev_num = curr_num;

    return gesture;
}

void touch_dev_init_gesture(touch_dev_t *dev,
                            touch_dev_gesture_ctx_t *ctx)
{
    assert(dev);
    assert(ctx);

    ctx->dev = dev;

    ctx->state = TOUCH_DEV_STATE_RELEASED;
    ctx->t_changed = UINT32_MAX;

    ctx->t_prev_tap = 0;
    ctx->prev_num = 0;

    ctx->prev[0] = TOUCH_DEV_VALUE_INVALID;
    ctx->prev[1] = TOUCH_DEV_VALUE_INVALID;
}
