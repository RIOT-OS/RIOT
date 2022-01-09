/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ft5x06
 * @{
 *
 * @file
 * @brief       Device driver implementation for the FT5x06 touch driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#include "ztimer.h"

#include "ft5x06.h"
#include "ft5x06_constants.h"
#include "ft5x06_params.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define FT5X06_BUS              (dev->params.i2c)
#define FT5X06_ADDR             (dev->params.addr)

#define FT5X06_RESET_DELAY_MS   (200)

int ft5x06_init(ft5x06_t *dev, const ft5x06_params_t *params, ft5x06_event_cb_t cb, void *arg)
{
    dev->params = *params;

    /* Wait at least 200ms after power up before accessing registers */
    ztimer_sleep(ZTIMER_MSEC, FT5X06_RESET_DELAY_MS);

    i2c_acquire(FT5X06_BUS);

    uint8_t vendor_id = 0;
    if (i2c_read_reg(FT5X06_BUS, FT5X06_ADDR, FT5X06_G_VENDOR_ID_REG, &vendor_id, 0) != 0) {
        i2c_release(FT5X06_BUS);
        return -EPROTO;
    }

    if (vendor_id != FT5X06_VENDOR_ID) {
        DEBUG("[ft5x06] init: invalid vendor ID: '0x%02x' (expected: 0x%02x)\n",
                vendor_id, FT5X06_VENDOR_ID);
        i2c_release(FT5X06_BUS);
        return -ENODEV;
    }

    /* Auto-calibrate if needed */
    if (IS_ACTIVE(FT5X06_AUTO_CALIB_NEEDED)) {
        DEBUG("[ft5x06] init: enable device auto-calibration\n");
        i2c_write_reg(FT5X06_BUS, FT5X06_ADDR, FT5X06_G_AUTO_CLB_MODE_REG, 0, 0);
    }

    /* Configure interrupt */
    if (gpio_is_valid(dev->params.int_pin)) {
        DEBUG("[ft5x06] init: configuring touchscreen interrupt\n");
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, cb, arg);
        i2c_write_reg(FT5X06_BUS, FT5X06_ADDR, FT5X06_G_MODE_REG, FT5X06_G_MODE_INTERRUPT_TRIGGER & 0x01, 0);
    }

    i2c_release(FT5X06_BUS);

    return 0;
}

static const uint8_t touch_reg_map[FT5X06_TOUCHES_COUNT_MAX] = {
    FT5X06_TOUCH1_XH_REG,
    FT5X06_TOUCH2_XH_REG,
#if FT5X06_TOUCHES_COUNT_MAX > 2
    FT5X06_TOUCH3_XH_REG,
    FT5X06_TOUCH4_XH_REG,
    FT5X06_TOUCH5_XH_REG,
#endif
};

int ft5x06_read_touch_positions(const ft5x06_t *dev, ft5x06_touch_position_t *positions, size_t len)
{
    i2c_acquire(FT5X06_BUS);
    for (uint8_t touch = 0; touch < len; touch++) {
        uint8_t regs[4];
        uint16_t pos_x = 0, pos_y = 0;
        i2c_read_regs(FT5X06_BUS, FT5X06_ADDR, touch_reg_map[touch], &regs, 4, 0);
        pos_x = (uint16_t)((regs[1] & FT5X06_TOUCH_POS_LSB_MASK) | (uint16_t)(regs[0] & FT5X06_TOUCH_POS_MSB_MASK) << 8);
        pos_y = (uint16_t)((regs[3] & FT5X06_TOUCH_POS_LSB_MASK) | (uint16_t)(regs[2] & FT5X06_TOUCH_POS_MSB_MASK) << 8);
        /* X and Y positions are swapped compared to the display */
        positions[touch].x = pos_y;
        positions[touch].y = pos_x;
    }
    i2c_release(FT5X06_BUS);

    return 0;
}

int ft5x06_read_touch_count(const ft5x06_t *dev, uint8_t *count)
{
    i2c_acquire(FT5X06_BUS);
    i2c_read_reg(FT5X06_BUS, FT5X06_ADDR, FT5X06_TD_STATUS_REG, count, 0);
    i2c_release(FT5X06_BUS);
    *count &= FT5X06_TD_STATUS_MASK;

    if (*count > FT5X06_TOUCHES_COUNT_MAX) {
        *count = 0;
    }

    return 0;
}

int ft5x06_read_touch_gesture(const ft5x06_t *dev, ft5x06_touch_gesture_t *gesture)
{
    uint8_t gesture_id = 0;
    i2c_acquire(FT5X06_BUS);
    i2c_read_reg(FT5X06_BUS, FT5X06_ADDR, FT5X06_GESTURE_ID_REG, &gesture_id, 0);
    i2c_release(FT5X06_BUS);

    DEBUG("[ft5x06] read gesture_id '0x%02X'\n", gesture_id);

    switch (gesture_id) {
        case FT5X06_GESTURE_ID_MOVE_UP:
            *gesture = FT5X06_TOUCH_MOVE_UP;
            break;
        case FT5X06_GESTURE_ID_MOVE_LEFT:
            *gesture = FT5X06_TOUCH_MOVE_LEFT;
            break;
        case FT5X06_GESTURE_ID_MOVE_DOWN:
            *gesture = FT5X06_TOUCH_MOVE_DOWN;
            break;
        case FT5X06_GESTURE_ID_MOVE_RIGHT:
            *gesture = FT5X06_TOUCH_MOVE_RIGHT;
            break;
        case FT5X06_GESTURE_ID_ZOOM_IN:
            *gesture = FT5X06_TOUCH_ZOOM_IN;
            break;
        case FT5X06_GESTURE_ID_ZOOM_OUT:
            *gesture = FT5X06_TOUCH_ZOOM_OUT;
            break;
        default: /* Fallback to None */
            *gesture = FT5X06_TOUCH_NO_GESTURE;
            break;
    }

    return 0;
}
