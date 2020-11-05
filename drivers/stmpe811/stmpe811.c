/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stmpe811
 * @{
 *
 * @file
 * @brief       Device driver implementation for the STMPE811 touchscreen controller.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <inttypes.h>

#include "xtimer.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#include "stmpe811.h"
#include "stmpe811_constants.h"
#include "stmpe811_params.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define STMPE811_DEV_I2C    (dev->params.i2c)
#define STMPE811_DEV_ADDR   (dev->params.addr)

static int _soft_reset(const stmpe811_t *dev)
{
    if (i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                      STMPE811_SYS_CTRL1, STMPE811_SYS_CTRL1_SOFT_RESET, 0) < 0) {
        DEBUG("[stmpe811] soft reset: cannot write soft reset bit to SYS_CTRL1 register\n");
        return -STMPE811_ERR_I2C;
    }
    xtimer_msleep(10);

    if (i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                      STMPE811_SYS_CTRL1, 0, 0) < 0) {
        DEBUG("[stmpe811] soft reset: cannot clear SYS_CTRL1 register\n");
        return -STMPE811_ERR_I2C;
    }
    xtimer_msleep(2);

    return STMPE811_OK;
}

static void _reset_fifo(const stmpe811_t *dev)
{
    i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                  STMPE811_FIFO_CTRL_STA, STMPE811_FIFO_CTRL_STA_RESET, 0);
    i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                  STMPE811_FIFO_CTRL_STA, 0, 0);
}

static void _clear_interrupt_status(const stmpe811_t *dev)
{
    i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR, STMPE811_INT_STA, 0xff, 0);
}

int stmpe811_init(stmpe811_t *dev, const stmpe811_params_t * params, touch_event_cb_t cb, void *arg)
{
    dev->params = *params;
    int ret = STMPE811_OK;

    /* Acquire I2C device */
    i2c_acquire(STMPE811_DEV_I2C);

    uint16_t device_id;
    if (i2c_read_regs(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                      STMPE811_CHIP_ID, &device_id, 2, 0) < 0) {
        DEBUG("[stmpe811] init: cannot read CHIP_ID register\n");
        i2c_release(STMPE811_DEV_I2C);
        return -STMPE811_ERR_I2C;
    }

    device_id = (device_id << 8) | (device_id >> 8);
    if (device_id != STMPE811_CHIP_ID_VALUE) {
        DEBUG("[stmpe811] init: invalid device id (actual: 0x%04X, expected: 0x%04X)\n",
              device_id, STMPE811_CHIP_ID_VALUE);
        i2c_release(STMPE811_DEV_I2C);
        return -STMPE811_ERR_NODEV;
    }

    DEBUG("[stmpe811] init: valid device\n");

    ret = _soft_reset(dev);
    if (ret != STMPE811_OK) {
        DEBUG("[stmpe811] init: reset failed\n");
        i2c_release(STMPE811_DEV_I2C);
        return -STMPE811_ERR_RESET;
    }

    DEBUG("[stmpe811] init: soft reset done\n");

    /* Initialization sequence */

    /* disable temperature sensor and GPIO */
    ret = i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                        STMPE811_SYS_CTRL2,
                        (STMPE811_SYS_CTRL2_TS_OFF | STMPE811_SYS_CTRL2_GPIO_OFF), 0);

    /* set to 80 cycles and adc resolution to 12 bit*/
    uint8_t reg = ((uint8_t)(STMPE811_ADC_CTRL1_SAMPLE_TIME_80 << STMPE811_ADC_CTRL1_SAMPLE_TIME_POS) |
                    STMPE811_ADC_CTRL1_MOD_12B);
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_ADC_CTRL1, reg, 0);

    /* set adc clock speed to 3.25 MHz */
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_ADC_CTRL2, STMPE811_ADC_CTRL2_FREQ_3_25MHZ, 0);

    /* set GPIO AF to function as ts/adc */
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_GPIO_ALT_FUNCTION, 0x00, 0);

    /* set touchscreen configuration */
    reg = ((uint8_t)(STMPE811_TSC_CFG_AVE_CTRL_4 << STMPE811_TSC_CFG_AVE_CTRL_POS) |
                (uint8_t)(STMPE811_TSC_CFG_TOUCH_DET_DELAY_500US << STMPE811_TSC_CFG_TOUCH_DET_DELAY_POS) |
                (STMPE811_TSC_CFG_SETTLING_500US));
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_TSC_CFG, reg, 0);

    /* set fifo threshold */
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_FIFO_TH, 0x01, 0);

    /* reset fifo */
    _reset_fifo(dev);

    /* set fractional part to 7, whole part to 1 */
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_TSC_FRACTION_Z, STMPE811_TSC_FRACTION_Z_7_1, 0);

    /* set current limit value to 50 mA */
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_TSC_I_DRIVE, STMPE811_TSC_I_DRIVE_50MA, 0);

    /* enable touchscreen clock */
    ret += i2c_read_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                        STMPE811_SYS_CTRL2, &reg, 0);
    reg &= ~STMPE811_SYS_CTRL2_TSC_OFF;
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_SYS_CTRL2, reg, 0);

    ret += i2c_read_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                        STMPE811_TSC_CTRL, &reg, 0);
    reg |= STMPE811_TSC_CTRL_EN;
    ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                         STMPE811_TSC_CTRL, reg, 0);

    /* clear interrupt status */
    _clear_interrupt_status(dev);

    if (gpio_is_valid(dev->params.int_pin) && cb) {
        DEBUG("[stmpe811] init: configuring touchscreen interrupt\n");
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb, arg);

        /* Enable touchscreen interrupt */
        ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                             STMPE811_INT_EN, STMPE811_INT_EN_TOUCH_DET, 0);

        /* Enable global interrupt */
        ret += i2c_write_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                             STMPE811_INT_CTRL, STMPE811_INT_CTRL_GLOBAL_INT | STMPE811_INT_CTRL_INT_TYPE, 0);
    }

    if (ret < 0) {
        i2c_release(STMPE811_DEV_I2C);
        DEBUG("[stmpe811] init: initialization sequence failed\n");
        return -STMPE811_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(STMPE811_DEV_I2C);

    DEBUG("[stmpe811] initialization successful\n");

    return ret;
}

int stmpe811_read_touch_position(stmpe811_t *dev, stmpe811_touch_position_t *position)
{
    uint16_t tmp_x, tmp_y;

    /* Acquire I2C device */
    i2c_acquire(STMPE811_DEV_I2C);

    /* Ensure there's a least one position measured in the FIFO */
    uint8_t fifo_size = 0;
    do {
        i2c_read_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                     STMPE811_FIFO_SIZE, &fifo_size, 0);
    } while (!fifo_size);

    uint8_t  xyz[4];
    uint32_t xyz_ul;

    if (i2c_read_regs(STMPE811_DEV_I2C, STMPE811_DEV_ADDR,
                      STMPE811_TSC_DATA_NON_INC, &xyz, sizeof(xyz), 0) < 0) {
        DEBUG("[stmpe811] position: cannot read position\n");
        i2c_release(STMPE811_DEV_I2C);
        return -STMPE811_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(STMPE811_DEV_I2C);

    xyz_ul = ((uint32_t)xyz[0] << 24) | ((uint32_t)xyz[1] << 16) | \
            ((uint32_t)xyz[2] << 8) | (xyz[3] << 0);

    tmp_x = (xyz_ul >> 20) & 0xfff;
    tmp_y = (xyz_ul >>  8) & 0xfff;

    /* Y value first correction */
    tmp_y -= 360;

    /* Y value second correction */
    tmp_y /= 11;

    /* clamp y position */
    if (tmp_y > dev->params.ymax) {
        tmp_y = dev->prev_y;
    }

    /* X value first correction */
    if (tmp_x <= 3000) {
        tmp_x = 3870 - tmp_x;
    }
    else {
        tmp_x = 3800 - tmp_x;
    }

    /* X value second correction */
    tmp_x /= 15;

    /* clamp x position */
    if (tmp_x > dev->params.xmax) {
        tmp_x = dev->prev_x;
    }

    dev->prev_x = tmp_x;
    dev->prev_y = tmp_y;
    position->x = tmp_x;
    position->y = tmp_y;

    return STMPE811_OK;
}

int stmpe811_read_touch_state(const stmpe811_t *dev, stmpe811_touch_state_t *state)
{
    uint8_t val;

    /* Acquire I2C device */
    i2c_acquire(STMPE811_DEV_I2C);

    if (i2c_read_reg(STMPE811_DEV_I2C, STMPE811_DEV_ADDR, STMPE811_TSC_CTRL, &val, 0) < 0) {
        DEBUG("[stmpe811] position: cannot read touch state\n");
        i2c_release(STMPE811_DEV_I2C);
        return -STMPE811_ERR_I2C;
    }

    _clear_interrupt_status(dev);

    if ((val & STMPE811_TSC_CTRL_STA)) {
        *state = STMPE811_TOUCH_STATE_PRESSED;
    }
    else {
        _reset_fifo(dev);
        *state = STMPE811_TOUCH_STATE_RELEASED;
    }

    /* Release I2C device */
    i2c_release(STMPE811_DEV_I2C);

    return STMPE811_OK;
}
