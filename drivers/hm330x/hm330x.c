/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hm330x
 * @{
 *
 * @file
 * @brief       Device driver implementation for the HM330X Sensor Driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "hm330x.h"
#include "hm330x_constants.h"
#include "hm330x_params.h"
#include "clk.h"
#include "timex.h"

#if IS_USED(MODULE_ZTIMER_USEC)
#include "ztimer.h"
#elif IS_USED(MODULE_XTIMER)
#include "xtimer.h"
#endif

#define ENABLE_DEBUG    0
#include "debug.h"

/* pull reset pin low for ~10 us */
#define HM330X_RESET_TIME_US       (10)

int _set_i2c_mode(hm330x_t *dev)
{
    i2c_acquire(dev->params.i2c);
    uint8_t cmd = HM330X_CMD_I2C_MODE;
    int ret = i2c_write_bytes(dev->params.i2c, HM330X_I2C_ADDRESS, &cmd, 1, 0);

    i2c_release(dev->params.i2c);

    return ret;
}

int hm330x_init(hm330x_t *dev, const hm330x_params_t *params)
{
    assert(dev && params);
    memset(dev, 0, sizeof(hm330x_t));
    dev->params = *params;

    if (gpio_is_valid(dev->params.reset_pin)) {
        if (gpio_init(dev->params.reset_pin, GPIO_OUT)) {
            DEBUG_PUTS("[hm330x]: failed to init reset pin");
            return -EIO;
        }
    }

    if (gpio_is_valid(dev->params.set_pin)) {
        if (gpio_init(dev->params.set_pin, GPIO_OUT)) {
            DEBUG_PUTS("[hm330x]: failed to init set pin");
            return -EIO;
        }
        gpio_set(dev->params.set_pin);
    }

    DEBUG_PUTS("[hm330x]: reset device if reset_pin");
    hm330x_reset(dev);

    if (_set_i2c_mode(dev)) {
        DEBUG_PUTS("[hm330x]: failed set i2c mode");
        return -EPROTO;
    }

    return 0;
}

int hm330x_read(hm330x_t *dev, hm330x_data_t *data)
{
    i2c_acquire(dev->params.i2c);

    uint8_t buf[HM330X_DATA_LENGTH] = { 0 };

    if (i2c_read_bytes(dev->params.i2c, HM330X_I2C_ADDRESS, buf, HM330X_DATA_LENGTH, 0)) {
        return -EPROTO;
    }

    /* calculate crc */
    uint8_t crc = 0;

    for (uint8_t i = 0; i < HM330X_DATA_LENGTH - 1; i++) {
        crc += buf[i];
    }

    if (crc != buf[HM330X_DATA_LENGTH - 1]) {
        DEBUG("crc mismatch expected %02x got %02x\n", buf[HM330X_DATA_LENGTH - 1], crc);
    }

    i2c_release(dev->params.i2c);

    hm330x_data_t tmp = {
        .mc_pm_1 = (buf[4] << 8) | buf[5],
        .mc_pm_2p5 = (buf[6] << 8) | buf[7],
        .mc_pm_10 = (buf[8] << 8) | buf[9],
        .amc_pm_1 = (buf[10] << 8) | buf[11],
        .amc_pm_2p5 = (buf[12] << 8) | buf[13],
        .amc_pm_10 = (buf[14] << 8) | buf[15],
#if IS_USED(MODULE_HM3302)
        .nc_pm_0p3 = (buf[16] << 8) | buf[17],
        .nc_pm_0p5 = (buf[18] << 8) | buf[19],
        .nc_pm_1 = (buf[20] << 8) | buf[21],
        .nc_pm_2p5 = (buf[22] << 8) | buf[23],
        .nc_pm_5 = (buf[24] << 8) | buf[25],
        .nc_pm_10 = (buf[26] << 8) | buf[27],
#endif
    };

    memcpy(data, &tmp, sizeof(hm330x_data_t));

    return 0;
}

void hm330x_reset(hm330x_t *dev)
{
    if (gpio_is_valid(dev->params.reset_pin)) {
        gpio_clear(dev->params.reset_pin);
#if IS_USED(MODULE_ZTIMER_USEC)
        ztimer_sleep(ZTIMER_USEC, HM330X_RESET_TIME_US);
#elif IS_USED(MODULE_XTIMER)
        xtimer_sleep(HM330X_RESET_TIME_US);
#else
        /* each loop iteration is at least 3 instructions, so this tries
           to approximate the target time based on coreclk(), but
           a precise time is not needed here */
        for (uint32_t i = 0;
             i < HM330X_RESET_TIME_US * (coreclk() / US_PER_SEC / 3);
             i++) {
            /* Make sure for loop is not optimized out */
            __asm__ ("");
        }
#endif
        gpio_set(dev->params.reset_pin);
    }
}

void hm330x_sleep(hm330x_t *dev)
{
    if (gpio_is_valid(dev->params.set_pin)) {
        gpio_clear(dev->params.set_pin);
    }
}

void hm330x_wakeup(hm330x_t *dev)
{
    if (gpio_is_valid(dev->params.set_pin)) {
        gpio_set(dev->params.set_pin);
    }
}
