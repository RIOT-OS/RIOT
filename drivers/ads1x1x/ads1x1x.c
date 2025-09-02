/*
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ads1x1x
 * @{
 *
 * @file
 * @brief       ADS101x/111x Analog-to-digital converter driver
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * @}
 */

#include <assert.h>

#include "ads1x1x_params.h"
#include "ads1x1x_internal.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "ztimer.h"
#include "byteorder.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#ifndef ADS1X1X_READ_DELAY_MS
#define ADS1X1X_READ_DELAY_MS           8   /* compatible with 128SPS */
#endif

#define DEV (dev->params.i2c)
#define ADDR (dev->params.addr)

#define ADS1X1X_CONF_TEST_VALUE (1)

static inline int _ads1x1x_get_pga_voltage(uint8_t pga)
{
    switch (pga) {
    case ADS1X1X_PGA_FSR_6V144: return 6144;
    case ADS1X1X_PGA_FSR_4V096: return 4096;
    case ADS1X1X_PGA_FSR_2V048: return 2048;
    case ADS1X1X_PGA_FSR_1V024: return 1024;
    case ADS1X1X_PGA_FSR_0V512: return 512;
    case ADS1X1X_PGA_FSR_0V256: return 256;
    default: return 0;
    }
}

/**
 * @brief Builds the configuration register value from the parameters
 *
 * @param[in] params  ADS1X1X parameters
 *
 * @return Configuration register value
 */
static uint16_t _build_config_reg(const ads1x1x_params_t *params)
{
    uint8_t msb = params->mux | params->pga | params->mode;
    uint8_t lsb = params->dr | params->comp_mode | params->comp_polarity | params->comp_latch |
                  params->comp_queue;

    return (msb << 8) | lsb;
}

/**
 * @brief Test the ADS1X1X I2C communication
 *
 * @param[in] i2c   I2C device
 * @param[in] addr  I2C address
 *
 * @return zero on success, non zero on error
 */
static int _ads1x1x_init_test(i2c_t i2c, uint8_t addr)
{
    uint16_t reg;

    i2c_acquire(i2c);

    /* Register write test */
    reg = htons(ADS1X1X_CONF_TEST_VALUE);
    if (i2c_write_regs(i2c, addr, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0) {
        DEBUG("[ads1x1x] init - error: write test failed\n");
        i2c_release(i2c);
        return ADS1X1X_NODEV;
    }

    if (i2c_read_regs(i2c, addr, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0 ||
        ntohs(reg) != ADS1X1X_CONF_TEST_VALUE) {
        DEBUG("[ads1x1x] init - error: read test failed (reg=%04x)\n", ntohs(reg));
        i2c_release(i2c);
        return ADS1X1X_NOI2C;
    }

    i2c_release(i2c);
    return ADS1X1X_OK;
}

int ads1x1x_init(ads1x1x_t *dev, const ads1x1x_params_t *params)
{
    assert(dev && params);

    DEBUG("[ads1x1x] init - i2c=%d, addr=0x%02x\n", params->i2c, params->addr);

    int res = _ads1x1x_init_test(params->i2c, params->addr);
    if (res != ADS1X1X_OK) {
        return res;
    }

    return ads1x1x_set_parameters(dev, params);
}

int ads1x1x_set_parameters(ads1x1x_t *dev, const ads1x1x_params_t *params)
{
    assert(dev && params);

    i2c_acquire(params->i2c);
    /* Apply actual configuration */
    uint16_t conf = htons(_build_config_reg(params));
    if (i2c_write_regs(params->i2c, params->addr, ADS1X1X_CONF_ADDR, &conf, sizeof(conf), 0) < 0) {
        DEBUG("[ads1x1x] init - error: setting config failed\n");
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    i2c_release(params->i2c);
    dev->params = *params;
    return ADS1X1X_OK;
}

int ads1x1x_set_mux(ads1x1x_t *dev, uint8_t mux)
{
    assert(dev);

    i2c_acquire(DEV);

    /* Read current configuration */
    uint16_t reg;
    if (i2c_read_regs(DEV, ADDR, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    /* Update MUX bits */
    uint16_t conf = ntohs(reg);
    conf &= ~(ADS1X1X_MUX_MASK << 8);   /* Clear MUX bits */
    conf |= (mux << 8);            /* Set new MUX */

    /* Write back updated configuration */
    reg = htons(conf);

    if (i2c_write_regs(DEV, ADDR, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    i2c_release(DEV);

    /* Update only after successful I2C write */
    dev->params.mux = mux;
    return ADS1X1X_OK;
}

int ads1x1x_read_raw(const ads1x1x_t *dev, int16_t *raw)
{
    assert(dev && raw);

    uint16_t reg;

    i2c_acquire(DEV);

    /* Read config register */
    if (i2c_read_regs(DEV, ADDR, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    reg = ntohs(reg);

    /* Single-Shot mode */
    if ((reg & (ADS1X1X_MODE_MASK << 8)) == (ADS1X1X_MODE_SINGLE << 8)) {
        
        /* Tell the ADC to acquire a single-shot sample */
        reg |= ADS1X1X_CONF_OS_CONV << 8;
        if (i2c_write_regs(DEV, ADDR, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0) {
            i2c_release(DEV);
            return ADS1X1X_NOI2C;
        }

        /* Wait for the sample to be acquired */
        ztimer_sleep(ZTIMER_MSEC, ADS1X1X_READ_DELAY_MS);
    }

    /* Read the sample */
    if (i2c_read_regs(DEV, ADDR, ADS1X1X_CONV_RES_ADDR, &reg, sizeof(reg), 0) < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    i2c_release(DEV);

    /* Combine bytes into a single value */
    *raw = ntohs(reg);
    return ADS1X1X_OK;
}

int ads1x1x_convert_to_mv(ads1x1x_t *dev, int16_t raw)
{
    assert(dev);
    return ((int32_t)raw * _ads1x1x_get_pga_voltage(dev->params.pga))
           / (1 << (ADS1X1X_BITS_RES - 1));
}

int ads1x1x_alert_init(ads1x1x_alert_t *dev,
                       const ads1x1x_alert_params_t *params)
{
    assert(dev && params);

    int res = _ads1x1x_init_test(params->i2c, params->addr);
    if (res != ADS1X1X_OK) {
        return res;
    }

    /* Set up alerts */
    return ads1x1x_set_alert_parameters(dev, params);
}

int ads1x1x_set_alert_parameters(ads1x1x_alert_t *dev, const ads1x1x_alert_params_t *params)
{
    assert(dev && params);
    uint16_t reg;

    i2c_acquire(params->i2c);

    /* Set up low_limit */
    reg = htons((uint16_t)params->low_limit);
    if (i2c_write_regs(params->i2c, params->addr, ADS1X1X_LOW_LIMIT_ADDR, &reg, sizeof(reg),
                       0) < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    /* Set up high_limit */
    reg = htons((uint16_t)params->high_limit);
    if (i2c_write_regs(params->i2c, params->addr, ADS1X1X_HIGH_LIMIT_ADDR, &reg, sizeof(reg),
                       0) < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    /* Read control register */
    if (i2c_read_regs(params->i2c, params->addr, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }
    reg = ntohs(reg);

    /* Set up window mode */
    reg &= ~ADS1X1X_COMP_MODE_MASK;
    if (params->low_limit != 0) {
        /* Enable window mode */
        reg |= ADS1X1X_COMP_MODE_WINDOW;
    }

    /* Write back updated configuration */
    reg = htons(reg);
    if (i2c_write_regs(params->i2c, params->addr, ADS1X1X_CONF_ADDR, &reg, sizeof(reg), 0) < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    i2c_release(params->i2c);

    /* Update only after successful I2C write */
    dev->params = *params;
    return ADS1X1X_OK;
}

int ads1x1x_enable_alert(ads1x1x_alert_t *dev,
                         ads1x1x_alert_cb_t cb, void *arg)
{
    uint8_t regs[2];

    if (!gpio_is_valid(dev->params.alert_pin)) {
        return ADS1X1X_OK;
    }

    /* Read control register */
    i2c_acquire(DEV);
    i2c_read_regs(DEV, ADDR, ADS1X1X_CONF_ADDR, &regs, 2, 0x0);

    /* Enable alert comparator */
    regs[1] &= ~ADS1X1X_CONF_COMP_DIS;
    i2c_write_regs(DEV, ADDR, ADS1X1X_CONF_ADDR, &regs, 2, 0x0);

    i2c_release(DEV);

    /* Enable interrupt */
    dev->arg = arg;
    dev->cb = cb;
    gpio_init_int(dev->params.alert_pin, GPIO_IN, GPIO_FALLING, cb, arg);

    return ADS1X1X_OK;
}
