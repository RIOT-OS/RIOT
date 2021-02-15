/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx150x
 * @{
 *
 * @file
 * @brief       SX150X driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#include <string.h>

#include "assert.h"
#include "sx150x.h"

#ifdef MODULE_SX1509
#include "sx1509_regs.h"
#else
#include "sx15078_regs.h"
#endif

#define ENABLE_DEBUG (1)
#include "debug.h"

#define CHECK(expr)         if (expr != SX150X_OK) { goto err; }

/* define device module specific configuration */
#if defined(MODULE_SX1507)
#define PIN_NUMOF           (4U)
#define MASK(pin)           (1 << pin)
#elif defined(MODULE_SX1508)
#define PIN_NUMOF           (8U)
#define MASK(pin)           (1 << pin)
#elif defined(MODULE_SX1509)
#define PIN_NUMOF           (16U)
#define MASK(pin)           (1 << (pin & 0x7))
#endif

static int _reg_read(const sx150x_t *dev, uint8_t reg, uint8_t *val)
{
    if (i2c_read_reg(dev->params.bus, dev->params.addr, reg, val, 0) != 0) {
        return SX150X_BUSERR;
    }
    return SX150X_OK;
}

static int _reg_write(const sx150x_t *dev, uint8_t reg, uint8_t val)
{
    if (i2c_write_reg(dev->params.bus, dev->params.addr, reg, val, 0) != 0) {
        return SX150X_BUSERR;
    }
    return SX150X_OK;
}

/** Write an up to 16 bit map value into a pair of (B, A) registers (or the
 * direct register on the smaller SX1509s)
 *
 * @param[in] reg The first (B) register of the pair
 * */
static int _reg_write_u16(const sx150x_t *dev, uint8_t reg, uint16_t val)
{
    uint8_t buf[] = {val >> 8, val & 0xff};
    int ret;

    if (PIN_NUMOF == 16) {
        ret = i2c_write_regs(dev->params.bus, dev->params.addr, reg, buf, 2, 0);
    } else {
        ret = i2c_write_regs(dev->params.bus, dev->params.addr, reg, buf + 1, 1, 0);
    }

    if (ret != 0) {
        return SX150X_BUSERR;
    }
    return SX150X_OK;
}

/* Read-modify-write function to set a given mask in a register */
static int reg_set(const sx150x_t *dev, uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    if (_reg_read(dev, reg, &tmp) != SX150X_OK) {
        return SX150X_BUSERR;
    }
    // DEBUG("SET, before 0x%02x\n", (int)tmp);
    tmp |= mask;
    // DEBUG("SET, after  0x%02x\n", (int)tmp);
    return _reg_write(dev, reg, tmp);
}

/* Read-modify-write function to clear a given mask in a register */
static int reg_clr(const sx150x_t *dev, uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    if (_reg_read(dev, reg, &tmp) != SX150X_OK) {
        return SX150X_BUSERR;
    }
    tmp &= ~(mask);
    return _reg_write(dev, reg, tmp);
}

int sx150x_init(sx150x_t *dev, const sx150x_params_t *params)
{
    assert(dev && params);

    int res = SX150X_OK;
    uint8_t tmp;

    dev->params = *params;

    DEBUG("[sx150x] init: initializing driver now\n");

    /* try to get access to the bus */
    if (i2c_acquire(dev->params.bus) != 0) {
        DEBUG("[sx150x] init: unable to acquire bus\n");
        res = SX150X_BUSERR;
        goto exit;
    }

    DEBUG("[sx150x] acquire ok\n");

    /* do SW reset and read input disable register for testing the connection */
    res = _reg_write(dev, REG_RESET, RESET_SEQ_1);
    if (res != SX150X_OK) {
        goto exit;
    }
    res = _reg_write(dev, REG_RESET, RESET_SEQ_2);
    if (res != SX150X_OK) {
        goto exit;
    }

    res = _reg_read(dev, REG_INPUT_DISABLE(0), &tmp);
    if ((res != SX150X_OK) || (tmp != 0)) {
        DEBUG("[sx150x] init: error - unable to read from device\n");
        res = SX150X_NODEV;
        goto exit;
    }

    /* Scale down PWM frequency; even the 120Hz don't appear flickery. */
    res = _reg_write(dev, REG_MISC, REG_MISC_MINFREQ);
    assert(res == SX150X_OK);


#if ENABLE_DEBUG
    if (res == SX150X_OK) {
        DEBUG("[sx150x] init: initialization successful\n");
    }
    else {
        DEBUG("[sx150x] init: initialization failed (%i)\n", res);
    }
#endif

exit:
    i2c_release(dev->params.bus);
    return res;
}

/* Set the pin in the leddriverenable register, write it, and start the clock
 * if not running according to the last leddriverenable state.
 *
 * The I2C bus must be acquired around this founction.
 * */
static int pwm_pin_on(sx150x_t *dev, unsigned pin)
{
    int res;

    if (dev->leddriverenable == 0) {
        res = _reg_write(dev, REG_CLOCK, REG_CLOCK_INTERNALONLY);
        if (res != SX150X_OK) {
            return res;
        }
    }

    dev->leddriverenable |= 1 << pin;

    return _reg_write_u16(dev, REG_LEDDRIVERENABlE(BOTH), dev->leddriverenable);
}

/* Clear the pin in the leddriverenable register, write it if necessary, and
 * stop the clock if no PWM pin is active any more
 *
 * The I2C bus must be acquired around this founction.
 * */
static int pwm_pin_off(sx150x_t *dev, unsigned pin)
{
    int res = SX150X_OK;
    uint16_t mask = 1 << pin;
    if ((dev->leddriverenable & mask) == 0) {
        return res;
    }

    dev->leddriverenable &= ~mask;

    res = _reg_write_u16(dev, REG_LEDDRIVERENABlE(BOTH), dev->leddriverenable);
    if (res != SX150X_OK) {
        return res;
    }

    if (dev->leddriverenable == 0) {
        res = _reg_write(dev, REG_CLOCK, 0);
    }
    return res;
}

/* Summary for those who don't want to spend hours in the data sheet:
 *
 * * Logarithmic mode doesn't give any better resolution
 * * PWM always assumes active-low LEDs (as polarity inversion only affects the
 *   data register interpretation); especially log mode only makes sense that
 *   way 'round
 * * Only in ON state, full resolution is available; per the assumption of
 *   active-low, IOnX is only relevant when the data=0; IOn=255 means
 *   always-low, IOn=0 means always-high.
 */
int sx150x_pwm_init(sx150x_t *dev, unsigned pin)
{
    int res;
    assert(dev && (pin < PIN_NUMOF));
    i2c_acquire(dev->params.bus);

    DEBUG("init pin %i for PWM\n", (int)pin);

    res = _reg_write(dev, REG_INPUT_DISABLE(pin), MASK(pin));
    if (res != SX150X_OK) {
        goto err;
    }

    /* Given the registers expect pull-down and don't work when this is not
     * set, it seems to be a suitable setting */
    res = _reg_write(dev, REG_OPENDRAIN(pin), MASK(pin));
    if (res != SX150X_OK) {
        goto err;
    }

    /* Initialize to PWM value 0 in analogy to the PWM peripheral */
    res = _reg_write(dev, reg_i_on(pin), 0);
    if (res != SX150X_OK) {
        goto err;
    }

    /* Only when pin is clear, the PWM's ON value is used */
    dev->data &= ~(1 << pin);
    res = _reg_write_u16(dev, REG_DATA(BOTH), dev->data);
    if (res != SX150X_OK) {
        goto err;
    }

    res = pwm_pin_on(dev, pin);
    if (res != SX150X_OK) {
        goto err;
    }

err:
    i2c_release(dev->params.bus);
    return res;
}

int sx150x_pwm_set(sx150x_t *dev, unsigned pin, uint8_t value)
{
    int res;
    assert(dev && (pin < PIN_NUMOF));
    i2c_acquire(dev->params.bus);

    res = _reg_write(dev, reg_i_on(pin), value);

    i2c_release(dev->params.bus);
    return res;
}

int sx150x_gpio_init(sx150x_t *dev, unsigned pin, gpio_mode_t mode)
{
    assert(dev && (pin < PIN_NUMOF));

    i2c_acquire(dev->params.bus);

    /* Only currently supported mode */
    assert(mode == GPIO_OUT);

    pwm_pin_off(dev, pin);

    CHECK(reg_set(dev, REG_INPUT_DISABLE(pin), MASK(pin)));
    CHECK(reg_clr(dev, REG_DIR(pin), MASK(pin)));


    DEBUG("[sx150x] gpio_init: done\n");
    i2c_release(dev->params.bus);
    return SX150X_OK;

err:
    i2c_release(dev->params.bus);
    return SX150X_BUSERR;
}

int sx150x_gpio_set(sx150x_t *dev, unsigned pin)
{
    assert(dev && (pin < PIN_NUMOF));

    i2c_acquire(dev->params.bus);
    dev->data |= (1 << pin);
    int res = _reg_write_u16(dev, REG_DATA(BOTH), dev->data);
    i2c_release(dev->params.bus);

    return res;
}

int sx150x_gpio_clear(sx150x_t *dev, unsigned pin)
{
    assert(dev && (pin < PIN_NUMOF));

    i2c_acquire(dev->params.bus);
    dev->data &= ~(1 << pin);
    int res = _reg_write_u16(dev, REG_DATA(BOTH), dev->data);
    i2c_release(dev->params.bus);

    return res;
}

int sx150x_gpio_toggle(sx150x_t *dev, unsigned pin)
{
    assert(dev && (pin < PIN_NUMOF));

    i2c_acquire(dev->params.bus);
    dev->data ^= 1 << pin;
    int res = _reg_write_u16(dev, REG_DATA(BOTH), dev->data);
    i2c_release(dev->params.bus);

    return res;
}
