/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca95xx
 * @{
 *
 * @file
 * @brief       PCA95xx I2C GPIO expander device driver
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include "assert.h"
#include "debug.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#include "pca95xx.h"
#include "pca95xx_params.h"
#include "pca95xx_regs.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define I2C_SPEED I2C_SPEED_FAST

#define I2C (dev->params.i2c)
#define ADDR (dev->params.addr)

static inline uint8_t _get_reg(uint8_t pin, pca95xx_flags_t flags, uint8_t reg)
{
    /* Devices with >8 bits have twice as many registers */
    if (flags & PCA95XX_16PIN_DEV) {
        reg <<= 1;
    }

    /* Offset reg for high pins */
    if (pin < 8) {
        return reg;
    }
    else {
        return reg + 1;
    }
}

static inline uint8_t _pin_bit(uint8_t pin)
{
    if (pin < 8) {
        return pin;
    }
    else {
        return pin - 8;
    }
}

int pca95xx_init(pca95xx_t *dev, const pca95xx_params_t *params)
{
    uint8_t reg[2];
    uint8_t reg_addr;

    assert(dev && params);

    dev->params = *params;

    /* Acquire test */
    i2c_acquire(I2C);
    if (i2c_init_master(I2C, I2C_SPEED) < 0) {
        i2c_release(I2C);
        DEBUG("[pca95xx] init - error: unable to initialize I2C bus\n");
        return PCA95XX_NOI2C;
    }

    /* Register read/write test */
    reg_addr = _get_reg(dev->params.pin,
                        dev->params.flags, PCA95XX_INPUT_INVERT);
    i2c_read_reg(I2C, ADDR, reg_addr, &reg[0]);

    /* Toggle value */
    reg[1] = reg[0] ^ (1 << _pin_bit(dev->params.pin));

    i2c_write_reg(I2C, ADDR, reg_addr, reg[1]);
    i2c_read_reg(I2C, ADR, reg_addr, &reg[0]);

    /* Write should have actually written the register */
    if (reg[0] != reg[1]) {
        i2c_release(I2C);
        DEBUG("[pca95xx] init - error: unable to set reg (reg=%x)\n", regs[1]);
        return ADS101X_NODEV;
    }

    /* Set register back if it was already at its intended value */
    /* Also ensures that invert register is initialized properly */
    if ((dev->params.flags & PCA95XX_INPUT_INVERT)
        != (reg[0] & PCA95XX_INPUT_INVERT) {

        reg[0] ^= (1 << _pin_bit(dev->params.pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg[0]);
    }

    i2c_release(I2C);

    /* Finish initialize by setting pin to initial value */
    void pca95xx_write(dev, dev->params.pin,
                       (dev->params.flags & PCA95XX_INIT_SET));

    return PCA95XX_OK;
}

int pca95xx_int_init(pca95xx_int_t *dev, const pca95xx_int_params_t *params)
{
    assert(dev && params);

    dev->params = *params;
    dev->cb = NULL;
    dev->arg = NULL;

    /* Acquire test */
    i2c_acquire(I2C);
    if (i2c_init_master(I2C, I2C_SPEED) < 0) {
        i2c_release(I2C);
        DEBUG("[pca95xx] init - error: unable to initialize I2C bus\n");
        return PCA95XX_NOI2C;
    }
    i2c_release(I2C);

    return PCA95XX_OK;
}

int pca95xx_enable_int(pca95xx_int_t *dev, pca95xx_int_cb_t cb, void *arg)
{
    if (dev->params.int_pin == GPIO_UNDEF) {
        return PCA95XX_OK;
    }

    /* Enable interrupt */
    dev->arg = arg;
    dev->cb = cb;
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb, arg);

    return PCA95XX_OK;
}

int pca95xx_read(const pca95xx_t *dev, uint8_t pin)
{
    uint8_t reg;
    uint8_t reg_addr;

    i2c_acquire(I2C);

    /* Get input register */
    reg_addr = _get_reg(pin, dev->params.flags, PCA95XX_INPUT_ADDR);
    i2c_read_reg(I2C, ADDR, reg_addr, &reg);

    i2c_release(I2C);

    /* Get input bit */
    reg &= (1 << _pin_bit(pin));
    reg >>= _pin_bit(pin);

    return reg;
}

void pca95xx_set(const pca95xx_t *dev, uint8_t pin)
{
    uint8_t reg;
    uint8_t reg_addr;

    i2c_acquire(I2C);

    if (dev->params.flags & PCA95XX_HIGH_DRIVE) {
        /* Set pin high */
        reg_addr = _get_reg(pin, dev->params.flags, PCA95XX_OUTPUT_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg |= (1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);

        /* Set pin to output */
        reg_addr = _get_reg(pin, dev->params.flags, PCA95XX_CONFIG_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg &= ~(1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);
    }
    else {
        /* Set pin to input */
        reg_addr = _get_reg(pin, dev->params.flags, PCA95XX_CONFIG_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg |= (1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);
    }

    i2c_release(I2C);
}

void pca95xx_clear(const pca95xx_t *dev, uint8_t pin)
{
    uint8_t reg;
    uint8_t reg_addr;

    i2c_acquire(I2C);

    if (dev->params.flags & PCA95XX_LOW_DRIVE) {
        /* Set pin low */
        reg_addr = _get_reg(pin, dev->params.flags, PCA95XX_OUTPUT_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg &= ~(1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);

        /* Set pin to output */
        reg_addr = _get_reg(pin, dev->params.flags, PCA95XX_CONFIG_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg &= ~(1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);
    }
    else {
        /* Set pin to input */
        reg_addr = _get_reg(pin, dev->params.flags, PCA95XX_CONFIG_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg |= (1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);
    }

    i2c_release(I2C);
}

void pca95xx_toggle(const pca95xx_t *dev, uint8_t pin)
{
    if (pca95xx_read(dev, pin)) {
        pca95xx_clear(dev, pin);
    }
    else {
        pca95xx_set(dev, pin);
    }
}

void pca95xx_write(const pca95xx_t *dev, uint8_t pin, uint8_t value)
{
    if (value) {
        pca95xx_set(dev, pin);
    }
    else {
        pca95xx_clear(dev, pin);
    }
}
