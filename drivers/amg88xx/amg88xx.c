/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_amg88xx
 * @{
 *
 * @file
 * @brief       Device driver implementation for the AMG88xx infrared array sensor
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "ztimer.h"

#include "amg88xx.h"
#include "amg88xx_constants.h"
#include "amg88xx_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define AMG88XX_I2C     (dev->params.i2c_dev)
#define AMG88XX_ADDR    (dev->params.address)

/**
 * @brief   Encode a signed value into 12-bit sign-magnitude format
 */
static inline uint16_t _encode_12bit(int16_t val)
{
    uint16_t raw;

    if (val < 0) {
        raw = (uint16_t)(-val) & 0x07FF;
        raw |= 0x0800;
    }
    else {
        raw = (uint16_t)val & 0x07FF;
    }

    return raw;
}

/**
 * @brief   Decode a raw 12-bit sign-magnitude value to a signed int16_t
 */
static inline int16_t _decode_12bit(uint8_t low, uint8_t high)
{
    int16_t val = low | ((high & 0x07) << 8);

    if (high & 0x08) {
        val = -val;
    }

    return val;
}

int amg88xx_set_fps(const amg88xx_t *dev, amg88xx_fps_t fps)
{
    i2c_acquire(AMG88XX_I2C);

    if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_FPSC,
                      (uint8_t)fps, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_set_fps: cannot set FPS\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    return 0;
}

int amg88xx_get_fps(const amg88xx_t *dev, amg88xx_fps_t *fps)
{
    uint8_t val;

    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_FPSC,
                      &val, 1, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_fps: cannot read FPS\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    *fps = (amg88xx_fps_t)(val & AMG88XX_FPSC_FPS);

    return 0;
}

int amg88xx_set_mode(amg88xx_t *dev, amg88xx_mode_t mode)
{
    i2c_acquire(AMG88XX_I2C);

    /* perform a wake-up sequence if the last mode set was sleep mode */
    if (dev->mode == AMG88XX_MODE_SLEEP) {
        if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_PCTL,
                          (uint8_t)AMG88XX_MODE_NORMAL, 0) != 0) {
            i2c_release(AMG88XX_I2C);
            DEBUG("[amg88xx] amg88xx_set_mode: cannot wake from sleep\n");
            return -EIO;
        }

        ztimer_sleep(ZTIMER_MSEC, 50);

        if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_RST,
                          AMG88XX_RST_INITIAL_RESET, 0) != 0) {
            i2c_release(AMG88XX_I2C);
            DEBUG("[amg88xx] amg88xx_set_mode: cannot initial reset\n");
            return -EIO;
        }

        ztimer_sleep(ZTIMER_MSEC, 2);

        if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_RST,
                          AMG88XX_RST_FLAG_RESET, 0) != 0) {
            i2c_release(AMG88XX_I2C);
            DEBUG("[amg88xx] amg88xx_set_mode: cannot flag reset\n");
            return -EIO;
        }
    }

    /* set the desired mode */
    if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_PCTL,
                      (uint8_t)mode, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_set_mode: cannot set mode\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    dev->mode = mode;

    return 0;
}

int amg88xx_get_mode(const amg88xx_t *dev, amg88xx_mode_t *mode)
{
    uint8_t val;

    /* when the sensor is asleep, the power control is volatile and unreliable,
     * so return the last mode set by the driver */
    if (dev->mode == AMG88XX_MODE_SLEEP) {
        *mode = AMG88XX_MODE_SLEEP;
        return 0;
    }

    /* read the actual mode */
    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_PCTL,
                      &val, 1, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_mode: cannot read mode\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    *mode = (amg88xx_mode_t)val;

    return 0;
}

int amg88xx_set_averaging(const amg88xx_t *dev, bool enabled)
{
    uint8_t val = enabled ? AMG88XX_AVE_MAMOD_BIT : 0;

    i2c_acquire(AMG88XX_I2C);

    if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_AVE,
                      val, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_set_averaging: cannot set averaging\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    return 0;
}

int amg88xx_get_averaging(const amg88xx_t *dev, bool *enabled)
{
    uint8_t val;

    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_AVE,
                      &val, 1, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_averaging: cannot read averaging\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    *enabled = (val & AMG88XX_AVE_MAMOD_BIT) != 0;

    return 0;
}

int amg88xx_get_temperature(const amg88xx_t *dev, int16_t *temp)
{
    uint8_t data[2];

    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_TTHL,
                      data, 2, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_temperature: cannot get temperature\n");
        return 0;
    }

    i2c_release(AMG88XX_I2C);

    int16_t val = _decode_12bit(data[0], data[1]);

    /* convert to centi-degrees: 0.0625 deg C per LSB * 100 = 6.25 */
    *temp = (val * 625) / 100;

    return 0;
}

int amg88xx_get_frame(const amg88xx_t *dev, int16_t pixels[AMG88XX_PIXELS_COUNT])
{
    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_T01L,
                      (uint8_t *)pixels, AMG88XX_BYTES_PER_FRAME, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_frame: cannot read frame\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    uint8_t *raw = (uint8_t *)pixels;

    for (unsigned i = AMG88XX_PIXELS_COUNT; i-- > 0; ) {
        pixels[i] = _decode_12bit(raw[i * 2], raw[i * 2 + 1]);
    }

    return 0;
}

#if IS_USED(MODULE_PERIPH_GPIO_IRQ)
int amg88xx_init_int(amg88xx_t *dev, amg88xx_int_cb_t cb, void *arg)
{
    if (!gpio_is_valid(dev->params.int_pin)) {
        DEBUG("[amg88xx] amg88xx_init_int: invalid interrupt pin\n");
        return -ENODEV;
    }

    if (gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING,
                      cb, arg) < 0) {
        DEBUG("[amg88xx] amg88xx_init_int: cannot initialize interrupt pin\n");
        return -EIO;
    }

    return 0;
}
#endif

int amg88xx_set_interrupt(const amg88xx_t *dev, amg88xx_interrupt_mode_t mode,
                          bool enabled)
{
    uint8_t val = 0;

    if (enabled) {
        val |= AMG88XX_INTC_INTEN_BIT;
    }

    if (mode == AMG88XX_INT_ABSOLUTE) {
        val |= AMG88XX_INTC_INTMOD_BIT;
    }

    i2c_acquire(AMG88XX_I2C);

    if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_INTC,
                      val, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_set_interrupt: cannot set interrupt control\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    return 0;
}

int amg88xx_get_interrupt(const amg88xx_t *dev, amg88xx_interrupt_mode_t *mode,
                          bool *enabled)
{
    uint8_t val;

    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_INTC,
                      &val, 1, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_interrupt: cannot read interrupt control\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    *enabled = (val & AMG88XX_INTC_INTEN_BIT) != 0;
    *mode = (val & AMG88XX_INTC_INTMOD_BIT) ? AMG88XX_INT_ABSOLUTE
                                            : AMG88XX_INT_DIFFERENCE;

    return 0;
}

int amg88xx_set_interrupt_levels(const amg88xx_t *dev, int16_t upper,
                                 int16_t lower, int16_t hysteresis)
{
    i2c_acquire(AMG88XX_I2C);

    uint16_t raw;

    raw = _encode_12bit(upper);
    if (i2c_write_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_INTHL,
                       (uint8_t *)&raw, 2, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_set_interrupt_levels: cannot set upper level\n");
        return -EIO;
    }

    raw = _encode_12bit(lower);
    if (i2c_write_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_INTLL,
                       (uint8_t *)&raw, 2, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_set_interrupt_levels: cannot set lower level\n");
        return -EIO;
    }

    raw = _encode_12bit(hysteresis);
    if (i2c_write_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_INYSL,
                       (uint8_t *)&raw, 2, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_set_interrupt_levels: cannot set hysteresis\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    return 0;
}

int amg88xx_get_interrupt_levels(const amg88xx_t *dev, int16_t *upper,
                                 int16_t *lower, int16_t *hysteresis)
{
    uint8_t data[6];

    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_INTHL,
                      data, 6, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_interrupt_levels: cannot read levels\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    *upper = _decode_12bit(data[0], data[1]);
    *lower = _decode_12bit(data[2], data[3]);
    *hysteresis = _decode_12bit(data[4], data[5]);

    return 0;
}

int amg88xx_get_interrupt_table(const amg88xx_t *dev, uint8_t table[8])
{
    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_INT0,
                      table, 8, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_interrupt_table: cannot read table\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    return 0;
}

int amg88xx_get_status(const amg88xx_t *dev, uint8_t *status)
{
    i2c_acquire(AMG88XX_I2C);

    if (i2c_read_regs(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_STAT,
                      status, 1, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_get_status: cannot read status\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    return 0;
}

int amg88xx_clear_status(const amg88xx_t *dev)
{
    i2c_acquire(AMG88XX_I2C);

    if (i2c_write_reg(AMG88XX_I2C, AMG88XX_ADDR, AMG88XX_REG_SCLR,
                      AMG88XX_SCLR_INT_CLR | AMG88XX_SCLR_OVS_CLR |
                      AMG88XX_SCLR_OVT_CLR, 0) != 0) {
        i2c_release(AMG88XX_I2C);
        DEBUG("[amg88xx] amg88xx_clear_status: cannot clear status\n");
        return -EIO;
    }

    i2c_release(AMG88XX_I2C);

    return 0;
}

int amg88xx_init(amg88xx_t *dev, const amg88xx_params_t *params)
{
    /* initialize the device descriptor */
    dev->params = *params;
    dev->mode = AMG88XX_MODE_SLEEP;

    /* reset the device by changing its power mode to normal, which will
     * perform a wakeup routine when the last mode is sleep */
    unsigned int retries = 3;

    while (retries--) {
        if (amg88xx_set_mode(dev, AMG88XX_MODE_NORMAL) == 0) {
            return 0;
        }

        /* communication is ready after at most 50 ms after power up, but only
         * wait after a failed attempt */
        ztimer_sleep(ZTIMER_MSEC, 50);
    }

    return -ENODEV;
}
