/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_cst816s
 * @{
 *
 * @file
 * @brief       Device driver implementation for cst816s touch screen
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "log.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "ztimer.h"

#include "cst816s.h"
#include "cst816s_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

const char *cst816s_gesture_str[] = {
    [CST816S_GESTURE_NONE]         = "none",
    [CST816S_GESTURE_SLIDE_DOWN]   = "slide down",
    [CST816S_GESTURE_SLIDE_UP]     = "slide up",
    [CST816S_GESTURE_SLIDE_LEFT]   = "slide left",
    [CST816S_GESTURE_SLIDE_RIGHT]  = "slide right",
    [CST816S_GESTURE_SINGLE_CLICK] = "single click",
    [CST816S_GESTURE_DOUBLE_CLICK] = "double click",
    [CST816S_GESTURE_LONG_PRESS]   = "long press",
};

static void _gpio_irq(void *arg)
{
    cst816s_t *dev = arg;
    assert(dev);

    if (dev->cb) {
        dev->cb(dev->cb_arg);
    }
}

static void _cst816s_reset(const cst816s_t *dev)
{
    /* Reset, sleep durations based on
     * https://github.com/lupyuen/hynitron_i2c_cst0xxse/blob/master/cst0xx_core.c#L1078-L1085 */
    gpio_clear(dev->params->reset);
    ztimer_sleep(ZTIMER_MSEC, CST816S_RESET_DURATION_LOW_MS);
    gpio_set(dev->params->reset);
    ztimer_sleep(ZTIMER_MSEC, CST816S_RESET_DURATION_HIGH_MS);
}

int cst816s_read(const cst816s_t *dev, cst816s_touch_data_t *data)
{
    uint8_t buf[9]; /* 3 bytes "header" and 6 bytes touch info */

    i2c_acquire(dev->params->i2c_dev);
    int res = i2c_read_regs(dev->params->i2c_dev, dev->params->i2c_addr,
                            0, buf, sizeof(buf), 0);
    i2c_release(dev->params->i2c_dev);

    if (res < 0) {
        return res;
    }

    data->gesture = buf[1];
    data->action = buf[3] >> 6;
    data->x = ((buf[3] & 0x0f) << 8) | buf[4];
    data->y = ((buf[5] & 0x0f) << 8) | buf[6];

    return 0;
}

int cst816s_init(cst816s_t *dev, const cst816s_params_t *params,
                 cst816s_irq_cb_t cb, void *arg)
{
    assert(dev && params);
    dev->params = params;
    dev->cb = cb;
    dev->cb_arg = arg;

    if (gpio_is_valid(dev->params->reset)) {
        gpio_init(dev->params->reset, GPIO_OUT);
        _cst816s_reset(dev);
    }

    if (gpio_is_valid(dev->params->irq) && cb) {
        if (gpio_init_int(dev->params->irq, GPIO_IN,
                          dev->params->irq_flank,
                          _gpio_irq, dev) < 0) {
            return CST816S_ERR_IRQ;
        }
    }
    return CST816S_OK;
    /* The device will not respond until the first touch event */
}
