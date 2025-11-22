/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "ztimer.h"
#if IS_USED(MODULE_STMPE811_SPI)
#include "periph/spi.h"
#else
#include "periph/i2c.h"
#endif
#include "periph/gpio.h"

#include "stmpe811.h"
#include "stmpe811_constants.h"
#include "stmpe811_params.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#if IS_USED(MODULE_STMPE811_SPI)
#define BUS                 (dev->params.spi)
#define CS                  (dev->params.cs)
#define CLK                 (dev->params.clk)
#define MODE                (dev->params.mode)
#define WRITE_MASK          (0x7F)
#else
#define BUS                 (dev->params.i2c)
#define ADDR                (dev->params.addr)
#endif

#ifndef STMPE811_FIFO_THRESHOLD_ENABLED
#define STMPE811_FIFO_THRESHOLD_ENABLED    (0)
#endif

/* The driver only works reliably with FIFO threshold interrupts if the FIFO
 * threshold is at least 2. The reason is that all interrupts are cleared when
 * the status is checked in `stmpe811_read_touch_state` but the FIFO Threshold
 * interrupt is asserted again immediately since the FIFO is not read so that
 * a new interrupt is pending. On the other hand, the Touch Detected interrupt
 * does not work reliably for the release event if only the Touch Detected
 * interrupt in `stmpe811_read_touch_state` is cleared. The workaround is
 * to set the FIFO threshold to at least 2 to introduce a small delay between
 * the Touch Detect interrupt on a touch and the first FIFO threshold
 * interrupt. */
#ifndef STMPE811_FIFO_THRESHOLD
#define STMPE811_FIFO_THRESHOLD     (STMPE811_FIFO_THRESHOLD_ENABLED ? 2 : 1)
#endif

#if IS_USED(MODULE_STMPE811_SPI) /* using SPI mode */
static inline void _acquire(const stmpe811_t *dev)
{
    spi_acquire(BUS, CS, MODE, CLK);
}

static inline void _release(const stmpe811_t *dev)
{
    spi_release(BUS);
}

static int _read_reg(const stmpe811_t *dev, uint8_t reg, uint8_t *data)
{
    *data = spi_transfer_reg(BUS, CS, reg | 0x80, 0x00);
    return 0;
}

static int _write_reg(const stmpe811_t *dev, uint8_t reg, uint8_t data)
{
    (void)spi_transfer_reg(BUS, CS, (reg & WRITE_MASK), data);
    return 0;
}

static int _read_burst(const stmpe811_t *dev, uint8_t reg, void *buf, size_t len)
{
    uint8_t reg_read = reg | 0x80;

    /* since SPI is in auto-increment mode subsequent reads will ignore the
       content of the reg_read buffer, and itself can't overflow since it matches
       the amount of data per register */
    spi_transfer_regs(BUS, CS, reg_read, &reg_read, buf, len);
    return 0;
}

#else /* using I2C mode */

static inline void _acquire(const stmpe811_t *dev)
{
    i2c_acquire(BUS);
}

static inline void _release(const stmpe811_t *dev)
{
    i2c_release(BUS);
}

static int _read_reg(const stmpe811_t *dev, uint8_t reg, uint8_t *data)
{
    if (i2c_read_reg(BUS, ADDR, reg, data, 0) != 0) {
        return -EIO;
    }
    return 0;
}

static int _write_reg(const stmpe811_t *dev, uint8_t reg, uint8_t data)
{
    if (i2c_write_reg(BUS, ADDR, reg, data, 0) != 0) {
        return -EIO;
    }
    return 0;
}

static int _read_burst(const stmpe811_t *dev, uint8_t reg, void *buf, size_t len)
{
    if (i2c_read_regs(BUS, ADDR, reg, buf, len, 0) != 0) {
        return -EIO;
    }
    return 0;
}
#endif /* bus mode selection */

static int _soft_reset(const stmpe811_t *dev)
{
    if (_write_reg(dev, STMPE811_SYS_CTRL1, STMPE811_SYS_CTRL1_SOFT_RESET ) < 0) {
        DEBUG("[stmpe811] soft reset: cannot write soft reset bit to SYS_CTRL1 register\n");
        return -EPROTO;
    }
    ztimer_sleep(ZTIMER_MSEC, 10);

    if (_write_reg(dev, STMPE811_SYS_CTRL1, 0) < 0) {
        DEBUG("[stmpe811] soft reset: cannot clear SYS_CTRL1 register\n");
        return -EPROTO;
    }
    ztimer_sleep(ZTIMER_MSEC, 2);

    return 0;
}

static void _reset_fifo(const stmpe811_t *dev)
{
    _write_reg(dev, STMPE811_FIFO_CTRL_STA, STMPE811_FIFO_CTRL_STA_RESET);
    _write_reg(dev, STMPE811_FIFO_CTRL_STA, 0);
}

static void _clear_interrupt_status(const stmpe811_t *dev)
{
    _write_reg(dev, STMPE811_INT_STA, 0xff);
}

#if IS_USED(MODULE_STMPE811_SPI)
static int _stmpe811_check_mode(stmpe811_t *dev)
{
    /* can iterate directly through the enum since they might not be
       monotonically incrementing */
    uint8_t modes[] = { SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3};
    uint8_t reg;

    for (uint8_t i = 0; i < sizeof(modes); i++) {
        DEBUG("[stmpe811] init: set spi mode to 0x%02x ... ", modes[i]);
        dev->params.mode = modes[i];
        /* acquire */
        _acquire(dev);
        /* configure auto increment SPI */
        _read_reg(dev, STMPE811_SPI_CFG, &reg);
        reg = STMPE811_SPI_CFG_AUTO_INCR;
        _write_reg(dev, STMPE811_SPI_CFG, reg);
        _read_reg(dev, STMPE811_SPI_CFG, &reg);
        if (reg & STMPE811_SPI_CFG_AUTO_INCR) {
            DEBUG("success\n");
            _release(dev);
            return 0;
        }
        DEBUG("failed\n");
        _release(dev);
    }
    return 1;
}
#endif

int stmpe811_init(stmpe811_t *dev, const stmpe811_params_t *params, stmpe811_event_cb_t cb,
                  void *arg)
{
    dev->params = *params;
    dev->prev_x = 0;
    dev->prev_y = 0;

    int ret = 0;
    uint8_t reg;

#if IS_USED(MODULE_STMPE811_SPI)
    /* configure the chip-select pin */
    if (spi_init_cs(BUS, CS) != SPI_OK) {
        DEBUG("[stmpe811] error: unable to configure chip the select pin\n");
        return -EIO;
    }

    /* check mode configuration */
    if (_stmpe811_check_mode(dev) != 0) {
        DEBUG("[stmpe811] error: couldn't setup SPI\n");
        return -EIO;
    }
#endif

    /* acquire bus */
    _acquire(dev);

    uint16_t device_id;
    if (_read_burst(dev, STMPE811_CHIP_ID, &device_id, 2) < 0) {
        DEBUG("[stmpe811] init: cannot read CHIP_ID register\n");
        _release(dev);
        return -EPROTO;
    }

    device_id = (device_id << 8) | (device_id >> 8);
    if (device_id != STMPE811_CHIP_ID_VALUE) {
        DEBUG("[stmpe811] init: invalid device id (actual: 0x%04X, expected: 0x%04X)\n",
              device_id, STMPE811_CHIP_ID_VALUE);
        _release(dev);
        return -ENODEV;
    }

    DEBUG("[stmpe811] init: valid device\n");

    ret = _soft_reset(dev);
    if (ret != 0) {
        DEBUG("[stmpe811] init: reset failed\n");
        _release(dev);
        return -EIO;
    }

    DEBUG("[stmpe811] init: soft reset done\n");

    /* Initialization sequence */

    /* disable temperature sensor and GPIO */
    ret =
        _write_reg(dev, STMPE811_SYS_CTRL2,
                   (STMPE811_SYS_CTRL2_TS_OFF | STMPE811_SYS_CTRL2_GPIO_OFF));

    /* set to 80 cycles and adc resolution to 12 bit*/
    reg =
        ((uint8_t)(STMPE811_ADC_CTRL1_SAMPLE_TIME_80 << STMPE811_ADC_CTRL1_SAMPLE_TIME_POS) |
         STMPE811_ADC_CTRL1_MOD_12B);

    ret += _write_reg(dev, STMPE811_ADC_CTRL1, reg);

    /* set adc clock speed to 3.25 MHz */
    ret += _write_reg(dev, STMPE811_ADC_CTRL2, STMPE811_ADC_CTRL2_FREQ_3_25MHZ);

    /* set GPIO AF to function as ts/adc */
    ret += _write_reg(dev, STMPE811_GPIO_ALT_FUNCTION, 0x00);

    /* set touchscreen configuration */
    reg = ((uint8_t)(STMPE811_TSC_CFG_AVE_CTRL_4 << STMPE811_TSC_CFG_AVE_CTRL_POS) |
           (uint8_t)(STMPE811_TSC_CFG_TOUCH_DET_DELAY_500US <<
                     STMPE811_TSC_CFG_TOUCH_DET_DELAY_POS) |
           (STMPE811_TSC_CFG_SETTLING_500US));
    ret += _write_reg(dev, STMPE811_TSC_CFG, reg);

    /* set fifo threshold */
    ret += _write_reg(dev, STMPE811_FIFO_TH, STMPE811_FIFO_THRESHOLD);

    /* reset fifo */
    _reset_fifo(dev);

    /* set fractional part to 7, whole part to 1 */
    ret += _write_reg(dev, STMPE811_TSC_FRACTION_Z, STMPE811_TSC_FRACTION_Z_7_1);

    /* set current limit value to 50 mA */
    ret += _write_reg(dev, STMPE811_TSC_I_DRIVE, STMPE811_TSC_I_DRIVE_50MA);

    /* enable touchscreen clock */
    ret += _read_reg(dev, STMPE811_SYS_CTRL2, &reg);
    reg &= ~STMPE811_SYS_CTRL2_TSC_OFF;
    ret += _write_reg(dev, STMPE811_SYS_CTRL2, reg);

    ret += _read_reg(dev, STMPE811_TSC_CTRL, &reg);
    reg |= STMPE811_TSC_CTRL_EN;
    ret += _write_reg(dev, STMPE811_TSC_CTRL, reg);

    /* clear interrupt status */
    _clear_interrupt_status(dev);

    if (gpio_is_valid(dev->params.int_pin)) {
        DEBUG("[stmpe811] init: configuring touchscreen interrupt\n");
        if (cb) {
            gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb, arg);
        }

        /* Enable touchscreen interrupt */
        ret += _write_reg(dev, STMPE811_INT_EN,
                          STMPE811_INT_EN_TOUCH_DET |
                          (STMPE811_FIFO_THRESHOLD_ENABLED ? STMPE811_INT_EN_FIFO_TH : 0));

        /* Enable global interrupt */
        ret += _write_reg(dev, STMPE811_INT_CTRL,
                          STMPE811_INT_CTRL_GLOBAL_INT | STMPE811_INT_CTRL_INT_TYPE);
    }

    if (ret < 0) {
        _release(dev);
        DEBUG("[stmpe811] init: initialization sequence failed\n");
        return -EPROTO;
    }

    /* Release I2C device */
    _release(dev);

    DEBUG("[stmpe811] initialization successful\n");

    return ret;
}

int stmpe811_read_touch_position(stmpe811_t *dev, stmpe811_touch_position_t *position)
{
    uint16_t tmp_x, tmp_y;

    /* Acquire device bus */
    _acquire(dev);

    /* Ensure there's a least one position measured in the FIFO */
    uint8_t fifo_size = 0;

    do {
        _read_reg(dev, STMPE811_FIFO_SIZE, &fifo_size);
    } while (!fifo_size);

    uint8_t xyz[4];
    uint32_t xyz_ul;

#if IS_USED(MODULE_STMPE811_SPI)
    for (uint8_t i = 0; i < sizeof(xyz); i++) {
        if (_read_reg(dev, STMPE811_TSC_DATA_NON_INC, &xyz[i]) < 0) {
            DEBUG("[stmpe811] position: cannot read position\n");
            _release(dev);
            return -EPROTO;
        }
    }
#else
    if (_read_burst(dev, STMPE811_TSC_DATA_NON_INC, xyz, sizeof(xyz)) < 0) {
        DEBUG("[stmpe811] position: cannot read position\n");
        _release(dev);
        return -EPROTO;
    }
#endif

    /* Reset the FIFO, otherwise new touch data will be processed with a delay
     * if the rate of calling this function to read the FIFO is slower than
     * the rate at which the FIFO is filled. The reason for this is that with
     * each call of this function only the oldest touch data is read
     * value by value from the FIFO. Gestures, for example, can't be
     * implemented with such a behavior. */
    _reset_fifo(dev);

    /* Release device bus */
    _release(dev);

    xyz_ul = ((uint32_t)xyz[0] << 24) | ((uint32_t)xyz[1] << 16) | \
             ((uint32_t)xyz[2] << 8) | (xyz[3] << 0);

    tmp_x = (xyz_ul >> 20) & 0xfff;
    tmp_y = (xyz_ul >>  8) & 0xfff;

    /* Y value first correction */
    tmp_y -= 360;

    /* Y value second correction */
    tmp_y /= 11;

    /* maximum values in device coordinates */
    uint16_t tmp_xmax;
    uint16_t tmp_ymax;

    if (dev->params.xyconv & STMPE811_SWAP_XY) {
        tmp_xmax = dev->params.ymax;
        tmp_ymax = dev->params.xmax;
    }
    else {
        tmp_xmax = dev->params.xmax;
        tmp_ymax = dev->params.ymax;
    }

    /* clamp y position */
    if (tmp_y > tmp_ymax) {
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
    if (tmp_x > tmp_xmax) {
        tmp_x = dev->prev_x;
    }

    dev->prev_x = tmp_x;
    dev->prev_y = tmp_y;

    /* conversion to screen coordinates */
    if (dev->params.xyconv & STMPE811_SWAP_XY) {
        position->x = tmp_y;
        position->y = tmp_x;
    }
    else {
        position->x = tmp_x;
        position->y = tmp_y;
    }
    if (dev->params.xyconv & STMPE811_MIRROR_X) {
        assert(position->x <= dev->params.xmax);
        position->x = dev->params.xmax - position->x;
    }
    if (dev->params.xyconv & STMPE811_MIRROR_Y) {
        assert(position->y <= dev->params.ymax);
        position->y = dev->params.ymax - position->y;
    }

    return 0;
}

int stmpe811_read_touch_state(const stmpe811_t *dev, stmpe811_touch_state_t *state)
{
    uint8_t val;

    /* Acquire device bus */
    _acquire(dev);

    if (_read_reg(dev, STMPE811_TSC_CTRL, &val) < 0) {
        DEBUG("[stmpe811] position: cannot read touch state\n");
        _release(dev);
        return -EPROTO;
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
    _release(dev);

    return 0;
}
