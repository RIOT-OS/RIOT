/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca9685
 * @brief       Device driver for the PCA9685 I2C PWM controller
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "pca9685_regs.h"
#include "pca9685.h"

#include "irq.h"
#include "log.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    do { \
        if (!(cond)) { \
            DEBUG("[pca9685] %s: %s\n", \
                  __func__, "parameter condition (" #cond ") not fulfilled"); \
            assert(cond); \
        } \
    } while(0)

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[pca9685] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.i2c_dev, dev->params.i2c_addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond)
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */
#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[pca9685] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.i2c_dev, dev->params.i2c_addr, ## __VA_ARGS__)

#define EXEC_RET(f) \
    do { \
        int _r; \
        if ((_r = f) != PCA9685_OK) { \
            DEBUG("[pca9685] %s: error code %d\n", __func__, _r); \
            return _r; \
        } \
    } while(0)

#define EXEC_RET_CODE(f, c) \
    do { \
        int _r; \
        if ((_r = f) != PCA9685_OK) { \
            DEBUG("[pca9685] %s: error code %d\n", __func__, _r); \
            return c; \
        } \
    } while(0)

#define EXEC(f) \
    do { \
        int _r; \
        if ((_r = f) != PCA9685_OK) { \
            DEBUG("[pca9685] %s: error code %d\n", __func__, _r); \
            return; \
        } \
    } while(0)

/** Forward declaration of functions for internal use */
static int _is_available(const pca9685_t *dev);
static int _init(pca9685_t *dev);

static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit);

static int _read(const pca9685_t *dev, uint8_t reg, uint8_t *data, uint32_t len);
static int _write(const pca9685_t *dev, uint8_t reg, const uint8_t *data, uint32_t len);
static int _update(const pca9685_t *dev, uint8_t reg, uint8_t mask, uint8_t data);

inline static int _write_word(const pca9685_t *dev, uint8_t reg, uint16_t word);

int pca9685_init(pca9685_t *dev, const pca9685_params_t *params)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);
    ASSERT_PARAM(params->ext_freq <= 50000000);

    dev->powered_on = false;
    dev->params = *params;

    DEBUG_DEV("params=%p", dev, params);

    if (dev->params.oe_pin != GPIO_UNDEF) {
        /* init the pin an disable outputs first */
        gpio_init(dev->params.oe_pin, GPIO_OUT);
        gpio_set(dev->params.oe_pin);
    }

    /* test whether PWM device is available */
    EXEC_RET(_is_available(dev));

    /* init the PWM device */
    EXEC_RET(_init(dev));

    return PCA9685_OK;
}

uint32_t pca9685_pwm_init(pca9685_t *dev, pwm_mode_t mode, uint32_t freq,
                                                           uint16_t res)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(freq >= 24 && freq <= 1526);
    ASSERT_PARAM(res >= 2 && res <= 4096);
    ASSERT_PARAM(mode == PWM_LEFT || mode == PWM_CENTER || mode == PWM_RIGHT);

    DEBUG_DEV("mode=%u freq=%"PRIu32" res=%u", dev, mode, freq, res);

    dev->params.mode = mode;
    dev->params.freq = freq;
    dev->params.res = res;

    /* prescale can only be set while in sleep mode (powered off) */
    if (dev->powered_on) {
        pca9685_pwm_poweroff(dev);
    }

    /* prescale = round(clk / (PCA9685_RESOLUTION * freq)) - 1; */
    uint32_t div = PCA9685_RESOLUTION * freq;
    uint8_t byte = ((dev->params.ext_freq ? dev->params.ext_freq
                                          : PCA9685_OSC_FREQ) + div/2) / div - 1;

    EXEC_RET(_write(dev, PCA9685_REG_PRE_SCALE, &byte, 1));

    if (!dev->powered_on) {
        pca9685_pwm_poweron(dev);
    }

    return freq;
}

void pca9685_pwm_set(pca9685_t *dev, uint8_t chn, uint16_t val)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(chn <= PCA9685_CHANNEL_NUM);

    DEBUG_DEV("chn=%u val=%u", dev, chn, val);

    /* limit val to resolution */
    val = (val >= dev->params.res) ?  dev->params.res : val;

    uint16_t on;
    uint16_t off;

    if (val == 0) {
        /* full off */
        on = 0;
        off = PCA9685_LED_OFF;
    }
    else if (val == dev->params.res) {
        /* full on */
        on = PCA9685_LED_ON;
        off = 0;
    }
    else {
        /* duty = scale(2^12) / resolution * value */
        uint32_t duty = PCA9685_RESOLUTION * val / dev->params.res;
        switch (dev->params.mode) {
            case PWM_LEFT: on = 0;
                           off = on + duty;
                           break;
            case PWM_CENTER: on = (PCA9685_RESOLUTION - duty) >> 1;
                             off = on + duty;
                             break;
            case PWM_RIGHT: off = PCA9685_RESOLUTION - 1;
                            on = off - duty;
                            break;
            default: return;
        }
    }

    DEBUG_DEV("on=%u off=%u", dev, on, off);

    if (chn == PCA9685_CHANNEL_NUM) {
        EXEC(_write_word(dev, PCA9685_REG_ALL_LED_ON, on));
        EXEC(_write_word(dev, PCA9685_REG_ALL_LED_OFF, off));
    }
    else {
        EXEC(_write_word(dev, PCA9685_REG_LED_ON(chn), on));
        EXEC(_write_word(dev, PCA9685_REG_LED_OFF(chn), off));
    }
}

void pca9685_pwm_poweron(pca9685_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t byte;
    /* read MODE1 register */
    EXEC(_read(dev, PCA9685_REG_MODE1, &byte, 1));

    /* check if RESTART bit is 1 */
    if (byte & PCA9685_MODE1_RESTART) {
        /* clear the SLEEP bit */
        byte &= ~PCA9685_MODE1_SLEEP;
        EXEC(_write(dev, PCA9685_REG_MODE1, &byte, 1));
        /* allow 500 us for oscilator to stabilize */
        xtimer_usleep(500);
        /* clear the RESTART bit to start all PWM channels*/
        EXEC(_update(dev, PCA9685_REG_MODE1, PCA9685_MODE1_RESTART, 1));
    }
    else {
        EXEC(_update(dev, PCA9685_REG_MODE1, PCA9685_MODE1_SLEEP, 0));
        /* allow 500 us for oscilator to stabilize */
        xtimer_usleep(500);
        /* clear the RESTART bit to start all PWM channels*/
        EXEC(_update(dev, PCA9685_REG_MODE1, PCA9685_MODE1_RESTART, 1));
    }

    if (dev->params.oe_pin != GPIO_UNDEF) {
        gpio_clear(dev->params.oe_pin);
    }

    dev->powered_on = true;
}

void pca9685_pwm_poweroff(pca9685_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->params.oe_pin != GPIO_UNDEF) {
        gpio_set(dev->params.oe_pin);
    }

    /* set sleep mode */
    EXEC(_update(dev, PCA9685_REG_MODE1, PCA9685_MODE1_SLEEP, 1));

    dev->powered_on = false;
}

/** Functions for internal use only */

static int _is_available(const pca9685_t *dev)
{
    uint8_t byte;

    /* simply tests to read */
    return _read(dev, PCA9685_REG_MODE1, &byte, 1);
}

static int _init(pca9685_t *dev)
{
    /* set Auto-Increment flag */
    EXEC_RET(_update(dev, PCA9685_REG_MODE1, PCA9685_MODE1_AI, 1));

    /* switch off all channels */
    EXEC_RET(_write_word(dev, PCA9685_REG_ALL_LED_OFF, PCA9685_ALL_LED_OFF));

    /* set Auto-Increment flag */
    uint8_t byte = 0;
    _set_reg_bit(&byte, PCA9685_MODE2_INVERT, dev->params.inv);
    _set_reg_bit(&byte, PCA9685_MODE2_OUTDRV, dev->params.out_drv);
    _set_reg_bit(&byte, PCA9685_MODE2_OUTNE, dev->params.out_ne);
    EXEC_RET(_write(dev, PCA9685_REG_MODE2, &byte, 1));

    /* set Sleep mode, Auto-Increment, Restart, All call and External Clock */
    byte = 0;
    _set_reg_bit(&byte, PCA9685_MODE1_AI, 1);
    _set_reg_bit(&byte, PCA9685_MODE1_SLEEP, 1);
    _set_reg_bit(&byte, PCA9685_MODE1_RESTART, 1);
    _set_reg_bit(&byte, PCA9685_MODE1_ALLCALL, 1);
    EXEC_RET(_write(dev, PCA9685_REG_MODE1, &byte, 1));

    /* must be done only in sleep mode */
    _set_reg_bit(&byte, PCA9685_MODE1_EXTCLK, dev->params.ext_freq ? 1 : 0);
    EXEC_RET(_write(dev, PCA9685_REG_MODE1, &byte, 1));

    return PCA9685_OK;
}

static int _read(const pca9685_t *dev, uint8_t reg, uint8_t *data, uint32_t len)
{
    DEBUG_DEV("reg=%02x data=%p len=%"PRIu32"", dev, reg, data, len);

    /* acquire the I2C device */
    if (i2c_acquire(dev->params.i2c_dev)) {
        DEBUG_DEV("could not acquire I2C bus", dev);
        return -PCA9685_ERROR_I2C;
    }

    if (i2c_read_regs(dev->params.i2c_dev,
                      dev->params.i2c_addr, reg, data, len, 0) != 0) {
        i2c_release(dev->params.i2c_dev);
        return -PCA9685_ERROR_I2C;
    }

    /* release the I2C device */
    i2c_release(dev->params.i2c_dev);

    return PCA9685_OK;
}

static int _write(const pca9685_t *dev, uint8_t reg, const uint8_t *data, uint32_t len)
{
    DEBUG_DEV("reg=%02x data=%p len=%"PRIu32"", dev, reg, data, len);

    if (i2c_acquire(dev->params.i2c_dev)) {
        DEBUG_DEV("could not acquire I2C bus", dev);
        return -PCA9685_ERROR_I2C;
    }

    if (i2c_write_regs(dev->params.i2c_dev,
                       dev->params.i2c_addr, reg, data, len, 0) != 0) {
        i2c_release(dev->params.i2c_dev);
        return -PCA9685_ERROR_I2C;
    }

    /* release the I2C device */
    i2c_release(dev->params.i2c_dev);

    return PCA9685_OK;
}

inline static int _write_word(const pca9685_t *dev, uint8_t reg, uint16_t data)
{
    uint8_t bytes[2] = { data & 0xff, (data >> 8) & 0xff };
    return _write (dev, reg, bytes, 2);
}

static int _update(const pca9685_t *dev, uint8_t reg, uint8_t mask, uint8_t data)
{
    uint8_t byte;

    /* read current register value */
    EXEC_RET(_read(dev, reg, &byte, 1));

    /* set masked bits to the given value  */
    _set_reg_bit(&byte, mask, data);

    /* write back new register value */
    EXEC_RET(_write(dev, reg, &byte, 1));

    return PCA9685_OK;
}

static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit)
{
    uint8_t shift = 0;
    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    *byte = ((*byte & ~mask) | ((bit << shift) & mask));
}
