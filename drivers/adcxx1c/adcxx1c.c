/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_adcxx1c
 * @{
 *
 * @file
 * @brief       ADCXX1C ADC device driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include "adcxx1c.h"
#include "adcxx1c_params.h"
#include "adcxx1c_regs.h"

#include "periph/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define I2C (dev->params.i2c)
#define ADDR (dev->params.addr)

/* Configuration register test value
 * value 0x20: cycle time = Tconvert x 64 */
#define CONF_TEST_VALUE (0x20)

int adcxx1c_init(adcxx1c_t *dev, const adcxx1c_params_t *params)
{
    assert(dev && params);

    dev->params = *params;
    dev->cb = NULL;

    i2c_acquire(I2C);
    uint8_t reg = 0;

    /* Test communication write and read configuration register */
    i2c_write_reg(I2C, ADDR, ADCXX1C_CONF_ADDR, CONF_TEST_VALUE, 0);
    i2c_read_reg(I2C, ADDR, ADCXX1C_CONF_ADDR, &reg, 0);

    if (reg != CONF_TEST_VALUE) {
        i2c_release(I2C);
        DEBUG("[adcxx1c] init - error: unable to communicate with the device (reg=%x)\n", reg);
        return ADCXX1C_NODEV;
    }

    reg = dev->params.cycle << 5;
    i2c_write_reg(I2C, ADDR, ADCXX1C_CONF_ADDR, reg, 0);
    i2c_release(I2C);

    adcxx1c_set_alert_parameters(dev, dev->params.low_limit,
                                 dev->params.high_limit,
                                 dev->params.hysteresis);

    return ADCXX1C_OK;
}

int adcxx1c_read_raw(const adcxx1c_t *dev, int16_t *raw)
{
    uint8_t buf[2];
    int status;

    i2c_acquire(I2C);
    status = i2c_read_regs(I2C, ADDR, ADCXX1C_CONV_RES_ADDR, buf, 2, 0);
    i2c_release(I2C);
    if (status < 0) {
        return ADCXX1C_NOI2C;
    }

    *raw = ((buf[0] & 0x0F) << 8 | buf[1]) >> (12 - dev->params.bits);

    return ADCXX1C_OK;
}

static void _alert_cb(void *arg)
{
    adcxx1c_t *dev = arg;

    if (dev->cb) {
        dev->cb(dev->arg);
    }
}

int adcxx1c_enable_alert(adcxx1c_t *dev, adcxx1c_cb_t cb, void *arg)
{
    uint8_t reg;

    i2c_acquire(I2C);
    i2c_read_reg(I2C, ADDR, ADCXX1C_CONF_ADDR, &reg, 0);
    reg |= (dev->params.alert_pin != GPIO_UNDEF ? ADCXX1C_CONF_ALERT_PIN_EN : 0)
            | ADCXX1C_CONF_ALERT_FLAG_EN;
    i2c_write_reg(I2C, ADDR, ADCXX1C_CONF_ADDR, reg, 0);
    i2c_release(I2C);

    if (dev->params.alert_pin != GPIO_UNDEF) {
        dev->cb = cb;
        dev->arg = arg;
        /* alert active low */
        gpio_init_int(dev->params.alert_pin, GPIO_IN, GPIO_FALLING, _alert_cb, dev);
    }

    return ADCXX1C_OK;
}

int adcxx1c_set_alert_parameters(const adcxx1c_t *dev, int16_t low_limit,
                                 int16_t high_limit, int16_t hysteresis)
{
    uint8_t buf[2];

    i2c_acquire(I2C);

    low_limit <<= (12 - dev->params.bits);
    buf[0] = low_limit >> 8;
    buf[1] = low_limit & 0xFF;
    i2c_write_regs(I2C, ADDR, ADCXX1C_LOW_LIMIT_ADDR, buf, 2, 0);

    high_limit <<= (12 - dev->params.bits);
    buf[0] = high_limit >> 8;
    buf[1] = high_limit & 0xFF;
    i2c_write_regs(I2C, ADDR, ADCXX1C_HIGH_LIMIT_ADDR, buf, 2, 0);

    hysteresis <<= (12 - dev->params.bits);
    buf[0] = hysteresis >> 8;
    buf[1] = hysteresis & 0xFF;
    i2c_write_regs(I2C, ADDR, ADCXX1C_HYSTERESIS_ADDR, buf, 2, 0);

    i2c_release(I2C);

    return ADCXX1C_OK;
}
