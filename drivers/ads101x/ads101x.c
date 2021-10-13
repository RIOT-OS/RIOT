/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ads101x
 * @{
 *
 * @file
 * @brief       ADS101x/111x ADC device driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include "assert.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include "ads101x.h"
#include "ads101x_params.h"
#include "ads101x_regs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef ADS101X_READ_DELAY
#define ADS101X_READ_DELAY (8 * US_PER_MS)    /* Compatible with 128SPS */
#endif

#define DEV (dev->params.i2c)
#define ADDR (dev->params.addr)

static int _ads101x_init_test(i2c_t i2c, uint8_t addr);

int ads101x_init(ads101x_t *dev, const ads101x_params_t *params)
{
    assert(dev && params);

    dev->params = *params;

    return _ads101x_init_test(DEV, ADDR);
}

int ads101x_alert_init(ads101x_alert_t *dev,
                       const ads101x_alert_params_t *params)
{
    assert(dev && params);

    dev->params = *params;
    dev->cb = NULL;
    dev->arg = NULL;

    /* Set up alerts */
    ads101x_set_alert_parameters(dev, dev->params.low_limit,
                                 dev->params.high_limit);

    return _ads101x_init_test(DEV, ADDR);
}

static int _ads101x_init_test(i2c_t i2c, uint8_t addr)
{
    uint8_t regs[2];

    i2c_acquire(i2c);

    /* Register read test */
    if (i2c_read_regs(i2c, addr, ADS101X_CONF_ADDR, &regs, 2, 0x0) < 0) {
        DEBUG("[ads101x] init - error: unable to read reg %x\n",
              ADS101X_CONF_ADDR);
        i2c_release(i2c);
        return ADS101X_NODEV;
    }

    regs[1] = (regs[1] & ~ADS101X_DATAR_MASK) | ADS101X_DATAR_3300;

    /* Register write test */
    if (i2c_write_regs(i2c, addr, ADS101X_CONF_ADDR, &regs, 2, 0x0) < 0) {
        DEBUG("[ads101x] init - error: unable to write reg %x\n",
              ADS101X_CONF_ADDR);
        i2c_release(i2c);
        return ADS101X_NODEV;
    }

    i2c_read_regs(i2c, addr, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    i2c_release(i2c);

    /* Write should have actually written the register */
    if ((regs[1] & ADS101X_DATAR_MASK) != ADS101X_DATAR_3300) {
        DEBUG("[ads101x] init - error: unable to set reg (reg=%x)\n", regs[1]);
        return ADS101X_NODEV;
    }

    return ADS101X_OK;
}

int ads101x_set_mux_gain(const ads101x_t *dev, uint8_t mux_gain)
{
    uint8_t regs[2];

    i2c_acquire(DEV);

    i2c_read_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    /* Zero mux and gain */
    regs[0] &= ~ADS101X_MUX_MASK;
    regs[0] &= ~ADS101X_PGA_MASK;

    /* Write mux and gain */
    regs[0] |= mux_gain;

    i2c_write_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    i2c_release(DEV);

    return ADS101X_OK;
}

int ads101x_read_raw(const ads101x_t *dev, int16_t *raw)
{
    uint8_t regs[2];

    i2c_acquire(DEV);

    /* Read control register */
    i2c_read_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    /* Tell the ADC to acquire a single-shot sample */
    regs[0] |= ADS101X_CONF_OS_CONV;
    i2c_write_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    /* Wait for the sample to be acquired */
    xtimer_usleep(ADS101X_READ_DELAY);

    /* Read the sample */
    if (i2c_read_regs(DEV, ADDR, ADS101X_CONV_RES_ADDR, &regs, 2, 0x0) < 0) {
        i2c_release(DEV);
        return ADS101X_NODATA;
    }

    i2c_release(DEV);

    /* If all okay, change raw value */
    *raw = (int16_t)(regs[0] << 8) | (int16_t)(regs[1]);

    return ADS101X_OK;
}

int ads101x_enable_alert(ads101x_alert_t *dev,
                         ads101x_alert_cb_t cb, void *arg)
{
    uint8_t regs[2];

    if (!gpio_is_valid(dev->params.alert_pin)) {
        return ADS101X_OK;
    }

    /* Read control register */
    i2c_acquire(DEV);
    i2c_read_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    /* Enable alert comparator */
    regs[1] &= ~ADS101X_CONF_COMP_DIS;
    i2c_write_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    i2c_release(DEV);

    /* Enable interrupt */
    dev->arg = arg;
    dev->cb = cb;
    gpio_init_int(dev->params.alert_pin, GPIO_IN, GPIO_FALLING, cb, arg);

    return ADS101X_OK;
}

int ads101x_set_alert_parameters(const ads101x_alert_t *dev,
                                 int16_t low_limit, int16_t high_limit)
{
    uint8_t regs[2];

    i2c_acquire(DEV);

    /* Set up low_limit */
    regs[0] = (uint8_t)(low_limit >> 8);
    regs[1] = (uint8_t)low_limit;
    i2c_write_regs(DEV, ADDR, ADS101X_LOW_LIMIT_ADDR, &regs, 2, 0x0);

    /* Set up high_limit */
    regs[0] = (uint8_t)(high_limit >> 8);
    regs[1] = (uint8_t)high_limit;
    i2c_write_regs(DEV, ADDR, ADS101X_HIGH_LIMIT_ADDR, &regs, 2, 0x0);

    /* Read control register */
    i2c_read_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    /* Set up window mode */
    if (low_limit != 0) {
        /* Enable window mode */
        regs[1] |= ADS101X_CONF_COMP_MODE_WIND;
    }
    else {
        /* Disable window mode */
        regs[1] &= ~ADS101X_CONF_COMP_MODE_WIND;
    }
    i2c_write_regs(DEV, ADDR, ADS101X_CONF_ADDR, &regs, 2, 0x0);

    i2c_release(DEV);

    return ADS101X_OK;
}
