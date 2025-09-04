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
#define ADS1X1X_DEFAULT_CONFIG 0x8385

static inline int _read_reg(const i2c_t i2c, const uint8_t addr, uint8_t reg_addr,
                            uint16_t *reg_value)
{
    if (i2c_read_regs(i2c, addr, reg_addr, reg_value, sizeof(*reg_value), 0) < 0) {
        return ADS1X1X_NOI2C;
    }
    *reg_value = ntohs(*reg_value);
    return ADS1X1X_OK;
}

static inline int _read_conv_reg(const i2c_t i2c, const uint8_t addr, uint16_t *reg)
{
    return _read_reg(i2c, addr, ADS1X1X_CONV_RES_ADDR, reg);
}

static inline int _read_config_reg(const i2c_t i2c, const uint8_t addr, uint16_t *reg)
{
    return _read_reg(i2c, addr, ADS1X1X_CONF_ADDR, reg);
}

static inline int _write_reg(const i2c_t i2c, const uint8_t addr, uint8_t reg_addr,
                             uint16_t reg_value)
{
    reg_value = htons(reg_value);
    if (i2c_write_regs(i2c, addr, reg_addr, &reg_value, sizeof(reg_value), 0) < 0) {
        return ADS1X1X_NOI2C;
    }
    return ADS1X1X_OK;
}

static inline int _write_config_reg(const i2c_t i2c, const uint8_t addr, uint16_t value)
{
    return _write_reg(i2c, addr, ADS1X1X_CONF_ADDR, value);
}

static inline int _write_low_limit_reg(const i2c_t i2c, const uint8_t addr, uint16_t value)
{
    return _write_reg(i2c, addr, ADS1X1X_LOW_LIMIT_ADDR, value);
}

static inline int _write_high_limit_reg(const i2c_t i2c, const uint8_t addr, uint16_t value)
{
    return _write_reg(i2c, addr, ADS1X1X_HIGH_LIMIT_ADDR, value);
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
    uint8_t lsb = params->dr | ADS1X1X_PARAM_COMP_MODE | ADS1X1X_PARAM_COMP_POLARITY |
                  ADS1X1X_PARAM_COMP_LATCH | ADS1X1X_PARAM_COMP_QUEUE;

    return (msb << 8) | lsb;
}

int ads1x1x_reset(i2c_t i2c)
{
    i2c_acquire(i2c);

    /* General call to reset the internal registers */
    uint8_t reset = 0x06;
    int res;
    if ((res = i2c_write_byte(i2c, 0x00, reset, 0)) < 0) {
        i2c_release(i2c);
        DEBUG("[ads1x1x] init - error: reset failed res < %d\n", res);
        return ADS1X1X_NOI2C;
    }

    /* Assert the config register has been correctly reset */
    uint16_t reg;
    res = _read_config_reg(i2c, 0, &reg);
    if (res < 0) {
        i2c_release(i2c);
        return res;
    }
    i2c_release(i2c);

    if (reg != ADS1X1X_DEFAULT_CONFIG) {
        DEBUG("[ads1x1x] init - error: reset failed (reg=%04x)\n", reg);
        return ADS1X1X_NODEV;
    }
    return ADS1X1X_OK;

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
    uint16_t reg, conf;

    i2c_acquire(i2c);

    /* Read the configuration register */
    int res = _read_config_reg(i2c, addr, &reg);
    if (res < 0) {
        DEBUG("[ads1x1x] init - error: read test failed (reg=%04x)\n", reg);
        i2c_release(i2c);
        return ADS1X1X_NOI2C;
    }

    /* Save the configuration register value */
    conf = reg;

    /* Write test value */
    if (_write_config_reg(i2c, addr, ADS1X1X_CONF_TEST_VALUE) < 0) {
        DEBUG("[ads1x1x] init - error: write test failed\n");
        i2c_release(i2c);
        return ADS1X1X_NODEV;
    }

    /* Read back the configuration register */
    res = _read_config_reg(i2c, addr, &reg);
    if (res < 0 || reg != ADS1X1X_CONF_TEST_VALUE) {
        DEBUG("[ads1x1x] init - error: read test failed (reg=%04x)\n", reg);
        i2c_release(i2c);
        return ADS1X1X_NOI2C;
    }

    /* Restore original configuration */
    if (_write_config_reg(i2c, addr, conf) < 0) {
        DEBUG("[ads1x1x] init - error: write restore failed\n");
        i2c_release(i2c);
        return ADS1X1X_NODEV;
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
    uint16_t conf = _build_config_reg(params);
    if (_write_config_reg(params->i2c, params->addr, conf) < 0) {
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

    uint16_t reg;
    i2c_acquire(DEV);

    /* Read current configuration */
    int res = _read_config_reg(DEV, ADDR, &reg);
    if (res < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    /* Update MUX bits */
    uint16_t conf = reg;
    conf &= ~(ADS1X1X_MUX_MASK << 8);   /* Clear MUX bits */
    conf |= (mux << 8);                 /* Set new MUX */

    /* Write back updated configuration */
    if (_write_config_reg(DEV, ADDR, conf) < 0) {
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
    int res = _read_config_reg(DEV, ADDR, &reg);
    if (res < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    /* Single-Shot mode */
    if ((reg & (ADS1X1X_MODE_MASK << 8)) == (ADS1X1X_MODE_SINGLE << 8)) {


        /* Tell the ADC to acquire a single-shot sample */
        reg |= (ADS1X1X_CONF_OS_CONV_MASK << 8);
        if (_write_config_reg(DEV, ADDR, reg) < 0) {
            i2c_release(DEV);
            return ADS1X1X_NOI2C;
        }

        /* Wait for the sample to be acquired */
        ztimer_sleep(ZTIMER_MSEC, ADS1X1X_READ_DELAY_MS);
    }

    /* Read the sample */
    res = _read_conv_reg(DEV, ADDR, &reg);
    if (res < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    i2c_release(DEV);

    *raw = reg;
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
    if (_write_low_limit_reg(params->i2c, params->addr, params->low_limit) < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    /* Set up high_limit */
    if (_write_high_limit_reg(params->i2c, params->addr, params->high_limit) < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    int res = _read_config_reg(params->i2c, params->addr, &reg);
    if (res < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    /* Clear alert bits */
    reg &= ~ADS1X1X_ALERT_MASK;

    /* Apply config */
    reg |= (params->comp_polarity | params->comp_latch | params->comp_queue);
    /* Set up window mode */
    if (params->low_limit != 0) {
        /* Enable window mode */
        reg |= ADS1X1X_COMP_MODE_WINDOW;
    }

    /* Write back updated configuration */
    if (_write_config_reg(params->i2c, params->addr, reg) < 0) {
        i2c_release(params->i2c);
        return ADS1X1X_NOI2C;
    }

    i2c_release(params->i2c);

    /* Update only after successful I2C write */
    dev->params = *params;
    return ADS1X1X_OK;
}

int ads1x1x_enable_alert(ads1x1x_alert_t *dev,
                         ads1x1x_alert_cb_t cb, void *arg, uint8_t nb_assert)
{
    assert(dev && cb);
    uint16_t reg;

    if (!gpio_is_valid(dev->params.alert_pin)) {
        return ADS1X1X_GPIO_ERROR;
    }

    /* Read control register */
    i2c_acquire(DEV);
    int res = _read_config_reg(DEV, ADDR, &reg);
    if (res < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    /* Enable alert comparator */
    reg &= ~ADS1X1X_COMP_QUEUE_MASK;

    switch (nb_assert) {
    case 1: reg |= ADS1X1X_COMP_QUEUE_1; break;
    case 2: reg |= ADS1X1X_COMP_QUEUE_2; break;
    case 4: reg |= ADS1X1X_COMP_QUEUE_4; break;
    default: return ADS1X1X_INVALID_ARG;
    }

    if (_write_config_reg(DEV, ADDR, reg) < 0) {
        i2c_release(DEV);
        return ADS1X1X_NOI2C;
    }

    i2c_release(DEV);

    /* Enable interrupt */
    dev->arg = arg;
    dev->cb = cb;
    if (gpio_init_int(dev->params.alert_pin, GPIO_IN, GPIO_FALLING, cb, arg) < 0) {
        return ADS1X1X_GPIO_ERROR;
    }

    return ADS1X1X_OK;
}
