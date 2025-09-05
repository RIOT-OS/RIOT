/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <assert.h>

#include "adcxx1c.h"
#include "adcxx1c_params.h"
#include "adcxx1c_regs.h"

#include "periph/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV (dev->params.i2c)
#define ADDR (dev->params.addr)

/* Configuration register test value
 * value 0x20: cycle time = Tconvert x 64 */
#define CONF_TEST_VALUE (0x20)

int adcxx1c_init(adcxx1c_t *dev, const adcxx1c_params_t *params)
{
    assert(dev && params);
    int status;

    dev->params = *params;
    dev->cb = NULL;

    i2c_acquire(DEV);
    uint8_t reg = 0;

    /* Test communication write and read configuration register */
    status = i2c_write_reg(DEV, ADDR, ADCXX1C_CONF_ADDR, CONF_TEST_VALUE, 0);
    status += i2c_read_reg(DEV, ADDR, ADCXX1C_CONF_ADDR, &reg, 0);

    if (status < 0 || reg != CONF_TEST_VALUE) {
        i2c_release(DEV);
        DEBUG("[adcxx1c] init - error: unable to communicate with the device "
              "(reg=%x)\n", reg);
        return ADCXX1C_NODEV;
    }

    reg = dev->params.cycle << 5;
    status = i2c_write_reg(DEV, ADDR, ADCXX1C_CONF_ADDR, reg, 0);
    i2c_release(DEV);
    if (status < 0) {
        DEBUG("[adcxx1c] init - error: unable to communicate with the device "
              "(err=%x)\n", status);
        return ADCXX1C_NOI2C;
    }

    return adcxx1c_set_alert_parameters(dev, dev->params.low_limit,
                                        dev->params.high_limit,
                                        dev->params.hysteresis);
}

int adcxx1c_read_raw(const adcxx1c_t *dev, int16_t *raw)
{
    uint8_t buf[2];
    int status;

    i2c_acquire(DEV);
    status = i2c_read_regs(DEV, ADDR, ADCXX1C_CONV_RES_ADDR, buf, 2, 0);
    i2c_release(DEV);
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
    int status;

    i2c_acquire(DEV);
    i2c_read_reg(DEV, ADDR, ADCXX1C_CONF_ADDR, &reg, 0);
    reg |= (gpio_is_valid(dev->params.alert_pin) ? ADCXX1C_CONF_ALERT_PIN_EN : 0)
            | ADCXX1C_CONF_ALERT_FLAG_EN;
    status = i2c_write_reg(DEV, ADDR, ADCXX1C_CONF_ADDR, reg, 0);
    i2c_release(DEV);
    if (status < 0) {
        DEBUG("[adcxx1c] enable_alert - error: unable to communicate with the "
              "device (err=%d)\n", status);
        return ADCXX1C_NOI2C;
    }

    if (gpio_is_valid(dev->params.alert_pin)) {
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
    int status;

    i2c_acquire(DEV);

    low_limit <<= (12 - dev->params.bits);
    buf[0] = low_limit >> 8;
    buf[1] = low_limit & 0xFF;
    status = i2c_write_regs(DEV, ADDR, ADCXX1C_LOW_LIMIT_ADDR, buf, 2, 0);
    if (status < 0) {
        i2c_release(DEV);
        DEBUG("[adcxx1c] set_alert (low limit) - error: unable to communicate "
              "with the device (err=%d)\n", status);
        return ADCXX1C_NOI2C;
    }

    high_limit <<= (12 - dev->params.bits);
    buf[0] = high_limit >> 8;
    buf[1] = high_limit & 0xFF;
    status = i2c_write_regs(DEV, ADDR, ADCXX1C_HIGH_LIMIT_ADDR, buf, 2, 0);
    if (status < 0) {
        i2c_release(DEV);
        DEBUG("[adcxx1c] set_alert (high limit) - error: unable to communicate "
              "with the device (err=%d)\n", status);
        return ADCXX1C_NOI2C;
    }

    hysteresis <<= (12 - dev->params.bits);
    buf[0] = hysteresis >> 8;
    buf[1] = hysteresis & 0xFF;
    status = i2c_write_regs(DEV, ADDR, ADCXX1C_HYSTERESIS_ADDR, buf, 2, 0);
    if (status < 0) {
        i2c_release(DEV);
        DEBUG("[adcxx1c] set_alert (hysteresis) - error: unable to communicate "
              "with the device (err=%d)\n", status);
        return ADCXX1C_NOI2C;
    }

    i2c_release(DEV);

    return ADCXX1C_OK;
}

int adcxx1c_get_and_clear_alert(const adcxx1c_t *dev)
{
    int status;
    uint8_t alert;

    i2c_acquire(DEV);

    status = i2c_read_reg(DEV, ADDR, ADCXX1C_ALERT_STATUS_ADDR, &alert, 0);
    if (status < 0) {
        i2c_release(DEV);
        DEBUG("[adcxx1c] get_and_clear_alert - error: unable to read reg (%d)\n", status);
        return ADCXX1C_NOI2C;
    }

    status = i2c_write_reg(DEV, ADDR, ADCXX1C_ALERT_STATUS_ADDR, alert, 0);
    if (status < 0) {
        i2c_release(DEV);
        DEBUG("[adcxx1c] get_and_clear_alert - error: unable to write reg (%d)\n", status);
        return ADCXX1C_NOI2C;
    }

    i2c_release(DEV);

    return alert;
}
