/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mhz19
 * @{
 *
 * @file
 * @brief       Device driver implementation for MH-Z19 CO2 sensor.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include "mhz19.h"
#include "mhz19_params.h"
#include "ztimer.h"
#include "mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_MHZ19_PWM
int mhz19_init(mhz19_t *dev, const mhz19_params_t *params)
{
    int16_t res;

    dev->pin = params->pin;

    if (gpio_init(dev->pin, GPIO_IN_PD) != 0) {
        return MHZ19_ERR_INIT;
    }

    /* Take one measurement to make sure there's an mhz19 on the pin */
    if (mhz19_get_ppm(dev, &res) == MHZ19_ERR_TIMEOUT) {
        return MHZ19_ERR_TIMEOUT;
    }

    return MHZ19_OK;
}

int mhz19_get_ppm(mhz19_t *dev, int16_t *ppm)
{
    uint32_t start, middle, end, th, tl;
    /*
     * Per the docs, one sample should take 1004ms +-5%. Worst case is
     * that  we come in right after the rising edge of the current cycle,
     * so we want to wait two cycles plus some wiggle room at most for
     * a measurement.
     */
    int16_t timeout = 2200;

    DEBUG("%s: Waiting for high level to end\n", __func__);
    while (gpio_read(dev->pin) && timeout) {
        timeout--;
        ztimer_sleep(ZTIMER_MSEC, 1);
    }

    DEBUG("%s: Waiting for initial rising edge\n", __func__);
    while (!gpio_read(dev->pin) && timeout) {
        timeout--;
        ztimer_sleep(ZTIMER_MSEC, 1);
    }

    start = ztimer_now(ZTIMER_MSEC);
    DEBUG("%s: Waiting for falling edge\n", __func__);
    while (gpio_read(dev->pin) && timeout) {
        timeout--;
        ztimer_sleep(ZTIMER_MSEC, 1);
    }
    middle = ztimer_now(ZTIMER_MSEC);
    DEBUG("%s: Waiting for rising edge\n", __func__);
    while (!gpio_read(dev->pin) && timeout) {
        timeout--;
        ztimer_sleep(ZTIMER_MSEC, 1);
    }

    /* If we waited too long for flanks, something went wrong */
    if (!timeout) {
        DEBUG("%s: Measurement timed out\n", __func__);
        return MHZ19_ERR_TIMEOUT;
    }
    end = ztimer_now(ZTIMER_MSEC);

    th = (middle - start);
    tl = (end - middle);

    *ppm = (int16_t)(2000 * (th - 2) / (th + tl - 4));

    return MHZ19_OK;
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_MHZ19_PWM */
