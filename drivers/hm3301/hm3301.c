/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hm3301
 * @{
 *
 * @file
 * @brief       Device driver implementation for the HM3301 Sensor Driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "hm3301.h"
#include "hm3301_constants.h"
#include "hm3301_params.h"
#include "timex.h"

#if IS_USED(MODULE_ZTIMER_USEC)
#include "ztimer.h"
#elif IS_USED(MODULE_XTIMER)
#include "xtimer.h"
#endif

#define ENABLE_DEBUG    0
#include "debug.h"

/* pull reset pin low for ~10 us */
#define HM3301_RESET_TIME_US       (10)

int _set_i2c_mode(hm3301_t *dev)
{
    if (i2c_acquire(dev->params.i2c) != 0) {
        DEBUG("[hm3301]: could not acquire I2C bus %d\n", dev->params.i2c);
        return -1;
    }
    uint8_t cmd = HM3301_CMD_I2C_MODE;
    int ret = i2c_write_bytes(dev->params.i2c, HM3301_I2C_ADDRESS, &cmd, 1, 0);
    i2c_release(dev->params.i2c);

    return ret;
}

int hm3301_init(hm3301_t *dev, const hm3301_params_t *params)
{
    assert(dev && params);
    memset(dev, 0, sizeof(hm3301_t));
    dev->params = *params;

    if (gpio_is_valid(dev->params.reset_pin)) {
        if (gpio_init(dev->params.reset_pin, GPIO_OUT)) {
            DEBUG_PUTS("[hm3301]: failed to init reset pin");
            return -EIO;
        }
    }

    if (gpio_is_valid(dev->params.set_pin)) {
        if (gpio_init(dev->params.set_pin, GPIO_OUT)) {
            DEBUG_PUTS("[hm3301]: failed to init set pin");
            return -EIO;
        }
        gpio_set(dev->params.set_pin);
    }

    DEBUG_PUTS("[hm3301]: reset device if reset_pin");
    hm3301_reset(dev);

    if (_set_i2c_mode(dev)) {
        DEBUG_PUTS("[hm3301]: failed set i2c mode");
        return -EPROTO;
    }

    return 0;
}

int hm3301_read(hm3301_t *dev, hm3301_data_t* data)
{
    if (i2c_acquire(dev->params.i2c) != 0) {
        DEBUG("[hm3301]: could not acquire I2C bus %d\n", dev->params.i2c);
        return -EPROTO;
    }

    uint8_t buf[HM3301_DATA_LENGTH] = {0};
    if (i2c_read_bytes(dev->params.i2c, HM3301_I2C_ADDRESS, buf, HM3301_DATA_LENGTH, 0)) {
        return -EPROTO;
    }

    /* calculate crc */
    uint8_t crc = 0;
    for (uint8_t i = 0; i < HM3301_DATA_LENGTH - 1; i++) {
        crc += buf[i];
    }

    if (crc != buf[HM3301_DATA_LENGTH - 1]) {
        DEBUG("crc mismatch expected %02x got %02x\n", buf[HM3301_DATA_LENGTH - 1], crc)
    }

    i2c_release(dev->params.i2c);

    hm3301_data_t tmp = {
        .mc_pm_1    = (buf[ 4] << 8) | buf[ 5],
        .mc_pm_2p5  = (buf[ 6] << 8) | buf[ 7],
        .mc_pm_10   = (buf[ 8] << 8) | buf[ 9],
        .amc_pm_1   = (buf[10] << 8) | buf[11],
        .amc_pm_2p5 = (buf[12] << 8) | buf[13],
        .amc_pm_10  = (buf[14] << 8) | buf[15],
        .nc_pm_0p3  = (buf[16] << 8) | buf[17],
        .nc_pm_0p5  = (buf[18] << 8) | buf[19],
        .nc_pm_1    = (buf[20] << 8) | buf[21],
        .nc_pm_2p5  = (buf[22] << 8) | buf[23],
        .nc_pm_5    = (buf[24] << 8) | buf[25],
        .nc_pm_10   = (buf[26] << 8) | buf[27],
    };
    memcpy(data, &tmp, sizeof(hm3301_data_t));

    return 0;
}

void hm3301_reset(hm3301_t *dev)
{
    if (gpio_is_valid(dev->params.reset_pin)) {
        gpio_clear(dev->params.reset_pin);
#if IS_USED(MODULE_ZTIMER_USEC)
        ztimer_sleep(ZTIMER_USEC, HM3301_RESET_TIME_US);
#elif IS_USED(MODULE_XTIMER)
        xtimer_sleep(HM3301_RESET_TIME_US);
#else
        /* each loop iteration is at least 3 instructions, so this tries
           to approximate the target time based on CLOCK_CORECLOCK, but
           a precise time is not needed here */
        for (uint32_t i = 0;
             i < HM3301_RESET_TIME_US * (CLOCK_CORECLOCK / US_PER_SEC / 3);
             i++) {
            /* Make sure for loop is not optimized out */
            __asm__ ("");
        }
#endif
        gpio_set(dev->params.reset_pin);
    }
}

void hm3301_sleep(hm3301_t *dev)
{
    if (gpio_is_valid(dev->params.set_pin)) {
        gpio_clear(dev->params.set_pin);
    }
}

void hm3301_wakeup(hm3301_t *dev)
{
    if (gpio_is_valid(dev->params.set_pin)) {
        gpio_set(dev->params.set_pin);
    }
}
