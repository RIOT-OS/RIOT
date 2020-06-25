/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca9633
 * @brief       Device driver for the PCA9633 I2C PWM controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#include <stdio.h>

#include "pca9633.h"
#include "pca9633_regs.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Write data to a register.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] reg       Register address to write to
 * @param[in] data      Data to write
 *
 * @return  0 on success
 * @return  -PCA9633_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static int _write_reg(pca9633_t* dev, uint8_t reg, uint8_t data);

/**
 * @brief Read data from a register.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] reg       Register address to read from
 * @param[out] data     Byte read from given registerAddress
 *
 * @return  0 on success
 * @return  -PCA9633_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static int _read_reg(pca9633_t* dev, uint8_t reg, uint8_t* data);

int pca9633_init(pca9633_t *dev, const pca9633_params_t *params)
{
    assert(dev);
    assert(params);

    dev->params = *params;

    i2c_init(dev->params.i2c_dev);

    int rc = _write_reg(dev, PCA9633_REG_MODE1, 0x0);
    _write_reg(dev, PCA9633_REG_MODE2, 0x0);

    if (rc != PCA9633_OK) {
        return rc;
    }

    return PCA9633_OK;
}

void pca9633_wakeup(pca9633_t* dev)
{
    uint8_t reg;
    _read_reg(dev, PCA9633_REG_MODE1, &reg);
    reg = reg & ~(1 << PCA9633_BIT_SLEEP);

    _write_reg(dev, PCA9633_REG_MODE1, reg);
}

void pca9633_sleep(pca9633_t* dev)
{
    uint8_t reg;
    _read_reg(dev, PCA9633_REG_MODE1, &reg);
    reg = reg | (1 << PCA9633_BIT_SLEEP);

    _write_reg(dev, PCA9633_REG_MODE1, reg);
}

void pca9633_turn_on(pca9633_t* dev)
{
    _write_reg(dev, PCA9633_REG_LEDOUT, dev->stored_reg_ledout);
}

void pca9633_turn_off(pca9633_t* dev)
{
    _read_reg(dev, PCA9633_REG_LEDOUT, &dev->stored_reg_ledout);
    _write_reg(dev, PCA9633_REG_LEDOUT, PCA9633_LDR_STATE_OFF);
}

void pca9633_set_pwm(pca9633_t* dev,
        pca9633_pwm_channel_t pwm_channel, uint8_t pwm)
{
    _write_reg(dev, pwm_channel, pwm);
}

void pca9633_set_grp_pwm(pca9633_t* dev, uint8_t pwm)
{
    _write_reg(dev, PCA9633_REG_GRPPWM, pwm);
}

void pca9633_set_blinking(pca9633_t* dev, uint16_t blink_period_ms,
        uint8_t on_off_ratio)
{
    /* frequency of 24 Hz is used: */
    uint16_t blink_period = (24 * blink_period_ms) / 1000;
    if (blink_period > 255) {
        blink_period = 255;
    }

    _write_reg(dev, PCA9633_REG_GRPFREQ, (uint8_t) blink_period);
    _write_reg(dev, PCA9633_REG_GRPPWM, on_off_ratio);
}

void pca9633_set_rgb(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b)
{
    pca9633_set_pwm(dev, dev->params.reg_pwm_red, r);
    pca9633_set_pwm(dev, dev->params.reg_pwm_green, g);
    pca9633_set_pwm(dev, dev->params.reg_pwm_blue, b);
}

void pca9633_set_rgba(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    pca9633_set_rgb(dev, r, g, b);

    if (dev->params.has_amber_channel) {
        pca9633_set_pwm(dev, dev->params.reg_pwm_amber, a);
    }
}

void pca9633_set_ldr_state(pca9633_t* dev,
        pca9633_ldr_state_t state, pca9633_pwm_channel_t pwm_channel)
{
    uint8_t ldr_bit;
    switch (pwm_channel) {
        case PCA9633_PWM_CHANNEL_0:
            ldr_bit = PCA9633_BIT_LDR0;
            break;
        case PCA9633_PWM_CHANNEL_1:
            ldr_bit = PCA9633_BIT_LDR1;
            break;
        case PCA9633_PWM_CHANNEL_2:
            ldr_bit = PCA9633_BIT_LDR2;
            break;
        case PCA9633_PWM_CHANNEL_3:
            ldr_bit = PCA9633_BIT_LDR3;
            break;
        default:
            return;
    }

    uint8_t reg;
    _read_reg(dev, PCA9633_REG_LEDOUT, &reg);

    /* first clear both bits of ldr */
    reg = reg & ~(0b11 << ldr_bit);

    /* second set new state to specified ldr */
    reg |= (state << ldr_bit);

    _write_reg(dev, PCA9633_REG_LEDOUT, reg);
}

void pca9633_set_ldr_state_all(pca9633_t* dev, pca9633_ldr_state_t state)
{
    uint8_t reg = (state << PCA9633_BIT_LDR3)
                | (state << PCA9633_BIT_LDR2)
                | (state << PCA9633_BIT_LDR1)
                | (state << PCA9633_BIT_LDR0);

    _write_reg(dev, PCA9633_REG_LEDOUT, reg);
}

void pca9633_set_auto_increment(pca9633_t* dev, pca9633_auto_inc_option_t option)
{
    uint8_t new_reg;

    switch (option) {

        case PCA9633_AI_ALL:
            new_reg = (1 << PCA9633_BIT_AI2)
                    | (0 << PCA9633_BIT_AI1)
                    | (0 << PCA9633_BIT_AI0);
            break;

        case PCA9633_AI_IND:
            new_reg = (1 << PCA9633_BIT_AI2)
                    | (1 << PCA9633_BIT_AI1)
                    | (0 << PCA9633_BIT_AI0);
            break;

        case PCA9633_AI_GBL:
            new_reg = (1 << PCA9633_BIT_AI2)
                    | (0 << PCA9633_BIT_AI1)
                    | (1 << PCA9633_BIT_AI0);
            break;

        case PCA9633_AI_IND_GBL:
            new_reg = (1 << PCA9633_BIT_AI2)
                    | (1 << PCA9633_BIT_AI1)
                    | (1 << PCA9633_BIT_AI0);
            break;

        case PCA9633_AI_DISABLED:
            /* fall-thru */
        default:
            new_reg = (0 << PCA9633_BIT_AI2)
                    | (0 << PCA9633_BIT_AI1)
                    | (0 << PCA9633_BIT_AI0);
            break;
    }

    _write_reg(dev, PCA9633_REG_MODE1, new_reg);
}

void pca9633_set_group_control_mode(pca9633_t* dev,
        pca9633_group_control_mode_t mode)
{
    uint8_t prev_reg;
    _read_reg(dev, PCA9633_REG_MODE2, &prev_reg);

    switch (mode) {

        case PCA9633_GROUP_CONTROL_MODE_BLINKING:
            _write_reg(dev, PCA9633_REG_MODE2, prev_reg | (1 << PCA9633_BIT_DMBLNK));
            break;

        case PCA9633_GROUP_CONTROL_MODE_DIMMING:
        default:
            _write_reg(dev, PCA9633_REG_MODE2, prev_reg & ~(1 << PCA9633_BIT_DMBLNK));
            break;
    }
}

int _write_reg(pca9633_t* dev, uint8_t reg, uint8_t data)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    if (i2c_acquire(i2c_dev) != 0) {
        return -PCA9633_ERROR_I2C;
    }
    int rc = i2c_write_reg(i2c_dev, dev->params.i2c_addr, reg, data, 0);
    i2c_release(i2c_dev);

    return rc;
}

int _read_reg(pca9633_t* dev, uint8_t reg, uint8_t* data)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    if (i2c_acquire(i2c_dev) != 0) {
        return -PCA9633_ERROR_I2C;
    }
    int rc = i2c_read_reg(i2c_dev, dev->params.i2c_addr, reg, data, 0);
    i2c_release(i2c_dev);

    return rc;
}
