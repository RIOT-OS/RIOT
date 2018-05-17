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
#define DFLAGS (dev->params.dflags)
#define PFLAGS (dev->pflags)

static inline uint8_t _get_reg(uint8_t pin, pca95xx_dflags_t flags, uint8_t reg)
{
    /* Devices with >8 bits have twice as many registers */
    if (flags & (1 << PCA95XX_16PIN_DEV)) {
        reg <<= 1;
    }

    /* Offset reg for high pins */
    if (pin >= 8) {
        reg++;
    }

    return reg;
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

static inline uint8_t _get_pflag(uint8_t pin, uint32_t pflags, uint8_t flag)
{
    /* Shift to the correct entry */
    pflags >>= 2*pin;

    if (pflags & (1 << flag)) {
        return 1;
    }
    else {
        return 0;
    }
}

static inline void _set_pflag(uint8_t pin, uint32_t *pflags,
                              uint8_t flag, uint8_t val)
{
    if (val) {
        *pflags |= (1 << (2*pin + flag));
    }
    else {
        *pflags &= ~(1 << (2*pin + flag));
    }
}

static inline uint8_t _get_int_pin(uint8_t int_info)
{
    /* INT pin is lowest nibble of int_info */
    return (int_info & 0x0F);
}

static inline int8_t _get_int_num(uint8_t pin, uint8_t int_info[])
{
    uint8_t num;

    for (num = 0; num < PCA95XX_MAX_INTS; num++) {
        if (_get_int_pin(int_info[num]) == pin) {
            return num;
        }
    }

    /* No entry with that pin */
    return -1;
}

static void _isr_cb_mux(void *arg)
{
    pca95xx_t *dev = (pca95xx_t *)arg;
    uint8_t reg_addr;
    uint8_t reg[2];
    uint8_t num;
    uint8_t pin;
    uint8_t pin_val;

    i2c_acquire(I2C);

    /* Get input register */
    reg_addr = _get_reg(0, DFLAGS, PCA95XX_INPUT_ADDR);
    i2c_read_reg(I2C, ADDR, reg_addr, &reg[0]);

    if (DFLAGS & (1 << PCA95XX_16PIN_DEV)) {
        reg_addr++;
        i2c_read_reg(I2C, ADDR, reg_addr, &reg[1]);
    }
    else {
        reg[1] = 0;
    }

    i2c_release(I2C);

    for (num = 0; num < PCA95XX_MAX_INTS; num++) {
        /* No callback registered */
        if (dev->cbs[num].cb == NULL) {
            continue;
        }

        /* Get pin number and value */
        pin = _get_int_pin(dev->int_info[num]);
        if (pin < 8) {
            pin_val = reg[0] >> pin;
        }
        else {
            pin_val = reg[1] >> (pin - 8);
        }
        pin_val &= 0x01;

        /* Pin has not changed */
        if (pin_val == _get_pflag(pin, PFLAGS, PCA95XX_INT_LASTVAL)) {
            continue;
        }

        /* Update pin value */
        _set_pflag(pin, &(PFLAGS), PCA95XX_INT_LASTVAL, pin_val);

        /* Interrupt is not enabled */
        if (!(dev->int_info[num] & PCA95XX_INT_EN)) {
            continue;
        }

        /* Interrupt set to trigger on this flank */
        if ((pin_val && (dev->int_info[num] & (1 << PCA95XX_INT_RISE)))
            || (!pin_val && (dev->int_info[num] & (1 << PCA95XX_INT_FALL)))) {

            /* Run callback */
            dev->cbs[num].cb(dev->cbs[num].arg);
        }
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

    /* Register read/write test (pin 0 polarity toggle) */
    reg_addr = _get_reg(0, DFLAGS, PCA95XX_POLAR_ADDR);
    DEBUG("[pca95xx] init - testing reg %i\n", reg_addr);

    i2c_read_reg(I2C, ADDR, reg_addr, &reg[0]);

    /* Toggle value */
    reg[1] = reg[0] ^ (1 << 0);

    i2c_write_reg(I2C, ADDR, reg_addr, reg[1]);
    i2c_read_reg(I2C, ADDR, reg_addr, &reg[0]);

    /* Write should have actually written the register */
    if (reg[0] != reg[1]) {
        i2c_release(I2C);
        DEBUG("[pca95xx] init - error: unable to set reg (reg=%x)\n", reg[1]);
        return PCA95XX_NODEV;
    }

    /* Set the register back */
    reg[1] = reg[0] ^ (1 << 0);

    i2c_write_reg(I2C, ADDR, reg_addr, reg[1]);

    i2c_release(I2C);

    /* Clear callbacks */
    for (uint8_t i = 0; i < PCA95XX_MAX_INTS; i++) {
        dev->cbs[i].cb = NULL;
        dev->cbs[i].arg = NULL;
    }

    /* Set up interrupt pin */
    if (dev->params.int_pin != GPIO_UNDEF) {
        DEBUG("[pca95xx] init - setting up int pin %i\n", dev->params.int_pin);

        gpio_init_int(dev->params.int_pin, GPIO_IN,
                      GPIO_FALLING, _isr_cb_mux, dev);
    }

    return PCA95XX_OK;
}

int pca95xx_init_pin(pca95xx_t *dev, uint8_t pin, gpio_mode_t mode)
{
    /* Note: treat using pull-up/down as the same as without */
    if (mode == GPIO_IN_PD
        || mode == GPIO_IN_PU
        || mode == GPIO_OD_PU) {

        DEBUG("[pca95xx] init_pin - warn: pretending to PU/PD pin %i\n", pin);
    }

    /* Enable low-drive */
    if (mode == GPIO_OUT
        || mode == GPIO_OD
        || mode == GPIO_OD_PU) {

        DEBUG("[pca95xx] init_pin - enabled low drive on pin %i\n", pin);

        _set_pflag(pin, &(PFLAGS), PCA95XX_LOW_DRIVE, 1);
    }
    else {
        DEBUG("[pca95xx] init_pin - disabled low drive on pin %i\n", pin);

        _set_pflag(pin, &(PFLAGS), PCA95XX_LOW_DRIVE, 0);
    }

    /* Enable high-drive */
    if (mode == GPIO_OUT) {
        DEBUG("[pca95xx] init_pin - enabled high drive on pin %i\n", pin);

        _set_pflag(pin, &(PFLAGS), PCA95XX_HIGH_DRIVE, 1);
    }
    else {
        DEBUG("[pca95xx] init_pin - disabled high drive on pin %i\n", pin);

        _set_pflag(pin, &(PFLAGS), PCA95XX_HIGH_DRIVE, 0);
    }

    return 0;
}

int pca95xx_init_int(pca95xx_t *dev, uint8_t pin, gpio_mode_t mode,
                     gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int8_t num;

    /* Check for interrupt service */
    if ((dev->params.int_pin != GPIO_UNDEF) || (PCA95XX_MAX_INTS == 0)) {
        DEBUG("[pca95xx] init_int - error: no interrupt service\n");
        return -1;
    }

    /* If pin is not registered, use a new array entry if one is available */
    num = _get_int_num(pin, dev->int_info);
    if (num < 0) {
        for (num = 0; num < PCA95XX_MAX_INTS; num++) {
            if (dev->cbs[num].cb == NULL) {
                break;
            }
        }

        /* No empty entries available for use */
        if (num == PCA95XX_MAX_INTS) {
            DEBUG("[pca95xx] init_int - error: interrupt array is full\n");
            return -1;
        }
    }

    /* Initialize the pin */
    pca95xx_init_pin(dev, pin, mode);

    /* Set up int_info pin */
    dev->int_info[num] = pin;

    /* Set up int_info falling flank */
    if ((flank == GPIO_FALLING) || (flank == GPIO_BOTH)) {
        dev->int_info[num] |= (1 << PCA95XX_INT_FALL);
    }

    /* Set up int_info rising flank */
    if ((flank == GPIO_RISING) || (flank == GPIO_BOTH)) {
        dev->int_info[num] |= (1 << PCA95XX_INT_RISE);
    }

    /* Set up int_info last value */
    if (pca95xx_read(dev, pin)) {
        dev->int_info[num] |= (1 << PCA95XX_INT_LASTVAL);
    }

    /* Force interrupt safe callback assign order */
    volatile gpio_cb_t vcb = cb;
    volatile void *varg = arg;
    dev->cbs[num].arg = (void *)varg;
    dev->cbs[num].cb = vcb;

    DEBUG("[pca95xx] init_int - interrupts initialized on pin %i\n", pin);

    return 0;
}

void pca95xx_irq_enable(pca95xx_t *dev, uint8_t pin)
{
    int8_t num = _get_int_num(pin, dev->int_info);

    /* No interrupt with that pin */
    if (num < 0) {
        DEBUG("[pca95xx] irq_enable - error: no interrupt on pin %i\n", pin);
        return;
    }

    DEBUG("[pca95xx] irq_enable - enabling interrupts on pin %i\n", pin);
    dev->int_info[num] |= (1 << PCA95XX_INT_EN);
}

void pca95xx_irq_disable(pca95xx_t *dev, uint8_t pin)
{
    int8_t num = _get_int_num(pin, dev->int_info);

    /* No interrupt with that pin */
    if (num < 0) {
        DEBUG("[pca95xx] irq_disable - error: no interrupt on pin %i\n", pin);
        return;
    }

    DEBUG("[pca95xx] irq_disable - disabling interrupts on pin %i\n", pin);
    dev->int_info[num] &= ~(1 << PCA95XX_INT_EN);
}

int pca95xx_read(const pca95xx_t *dev, uint8_t pin)
{
    uint8_t reg;
    uint8_t reg_addr;

    i2c_acquire(I2C);

    /* Get input register */
    reg_addr = _get_reg(pin, DFLAGS, PCA95XX_INPUT_ADDR);
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

    if (_get_pflag(pin, PFLAGS, PCA95XX_HIGH_DRIVE)) {
        DEBUG("[pca95xx] set - driving pin %i high\n", pin);

        /* Set pin high */
        reg_addr = _get_reg(pin, DFLAGS, PCA95XX_OUTPUT_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg |= (1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);

        /* Set pin to output */
        reg_addr = _get_reg(pin, DFLAGS, PCA95XX_CONFIG_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg &= ~(1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);
    }
    else {
        DEBUG("[pca95xx] set - setting pin %i to high-Z\n", pin);

        /* Set pin to input */
        reg_addr = _get_reg(pin, DFLAGS, PCA95XX_CONFIG_ADDR);
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

    if (_get_pflag(pin, PFLAGS, PCA95XX_LOW_DRIVE)) {
        DEBUG("[pca95xx] clear - driving pin %i low\n", pin);

        /* Set pin low */
        reg_addr = _get_reg(pin, DFLAGS, PCA95XX_OUTPUT_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg &= ~(1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);

        /* Set pin to output */
        reg_addr = _get_reg(pin, DFLAGS, PCA95XX_CONFIG_ADDR);
        i2c_read_reg(I2C, ADDR, reg_addr, &reg);

        reg &= ~(1 << _pin_bit(pin));

        i2c_write_reg(I2C, ADDR, reg_addr, reg);
    }
    else {
        DEBUG("[pca95xx] clear - setting pin %i to high-Z\n", pin);

        /* Set pin to input */
        reg_addr = _get_reg(pin, DFLAGS, PCA95XX_CONFIG_ADDR);
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
