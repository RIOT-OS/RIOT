/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_itg320x
 * @brief       Device driver for the InvenSense ITG320X 3-axis gyroscope
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "itg320x_regs.h"
#include "itg320x.h"

#include "log.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[itg320x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__);

#define ERROR_DEV(f, d, ...) \
     do { \
        LOG_ERROR("[itg320x] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.dev, d->params.addr, ## __VA_ARGS__); \
    } while (0)

#define EXEC_RET(f) \
    do { \
        int _r; \
        if ((_r = f) != ITG320X_OK) { \
            DEBUG("[itg320x] %s: error code %d\n", __func__, _r); \
            return _r; \
        } \
    } while (0)

/** Forward declaration of functions for internal use */

static int _is_available(const itg320x_t *dev);
static int _reset(itg320x_t *dev);

static int _reg_read(const itg320x_t *dev, uint8_t reg, uint8_t *data, uint16_t len);
static int _reg_write(const itg320x_t *dev, uint8_t reg, uint8_t data);
static int _update_reg(const itg320x_t *dev, uint8_t reg, uint8_t mask, uint8_t val);

int itg320x_init(itg320x_t *dev, const itg320x_params_t *params)
{
    assert(dev != NULL);
    assert(params != NULL);

    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = *params;

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev));

    /* reset the sensor */
    EXEC_RET(_reset(dev));

    /* set internal sample rate divider (ISR) from parameters */
    EXEC_RET(_reg_write(dev, ITG320X_REG_SMPLRT_DIV, params->isr_div));

    /* set full scale always to +-2000 and LPF bandwidth from parameters */
    EXEC_RET(_reg_write(dev, ITG320X_REG_DLPFS,
                        params->lpf_bw | ITG320X_REG_DLPFS_FS_SEL_VAL));

    /* set clock source selection from parameters */
    EXEC_RET(_reg_write(dev, ITG320X_REG_PWR_MGM, params->clk_sel));

    return ITG320X_OK;
}

#ifdef MODULE_ITG320X_INT

int itg320x_init_int(const itg320x_t *dev, itg320x_drdy_int_cb_t cb, void *arg)
{
    assert(dev != NULL);
    assert(gpio_is_valid(dev->params.int_pin));

    DEBUG_DEV("cb=%p, arg=%p", dev, cb, arg);

    if (dev->params.int_level == ITG320X_INT_HIGH) {
        /* for high active interrupt signal (default) */
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, cb, 0);
    }
    else {
        /* for low active interrupt signal (default) */
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb, 0);
    }

    /*
     * Set interrupt configuration as following
     * - Logic level and drive type used from parameters
     * - Latching interrupt is enabled
     * - Latch clear method is reading the status register
     * - RAW data ready interrupt is enabled
     */
    EXEC_RET(_reg_write(dev, ITG320X_REG_INT_CFG,
                        dev->params.int_level | dev->params.int_drive |
                        ITG320X_REG_INT_CFG_LATCH_INT |
                        ITG320X_REG_INT_STATUS_RAW_RDY |
                        ITG320X_REG_INT_CFG_RAW_RDY_EN));

    return ITG320X_OK;
}

#endif /* MODULE_ITG320X_INT */

int itg320x_data_ready(const itg320x_t *dev)
{
    assert(dev != NULL);

    DEBUG_DEV("", dev);

    uint8_t reg;
    EXEC_RET(_reg_read(dev, ITG320X_REG_INT_STATUS, &reg, 1));

    return (reg & ITG320X_REG_INT_STATUS_RAW_RDY) ? ITG320X_OK
                                                  : ITG320X_ERROR_NO_DATA;
}

int itg320x_read(const itg320x_t *dev, itg320x_data_t *data)
{
    assert(dev != NULL);
    assert(data != NULL);

    DEBUG_DEV("data=%p", dev, data);

    itg320x_raw_data_t raw;

    EXEC_RET(itg320x_read_raw(dev, &raw));

    /*
     * The sensitivity of the sensor is 1/14.375 degree/seconds per LSB
     * with a tolerance of the scale factor of +-6 %. Scale raw values to
     * tenths of a degree/seconds according to sensors sensitivity
     * which corresponds to 8/115 degrees/seconds per LSB.
     */
    data->x = (uint32_t)(raw.x * 80 / 115);
    data->y = (uint32_t)(raw.y * 80 / 115);
    data->z = (uint32_t)(raw.z * 80 / 115);

    return ITG320X_OK;
}

int itg320x_read_raw(const itg320x_t *dev, itg320x_raw_data_t *raw)
{
    assert(dev != NULL);
    assert(raw != NULL);

    DEBUG_DEV("raw=%p", dev, raw);

    uint8_t data[6];

    /* read raw data sample */
    EXEC_RET(_reg_read(dev, ITG320X_REG_GYRO_XOUT_H, data, 6));

    /* data MSB @ lower address */
    raw->x = (data[0] << 8) | data[1];
    raw->y = (data[2] << 8) | data[3];
    raw->z = (data[4] << 8) | data[5];

#ifdef MODULE_ITG320X_INT
    /* read status register to clear the interrupt */
    EXEC_RET(_reg_read(dev, ITG320X_REG_INT_STATUS, data, 1));
#endif

    return ITG320X_OK;
}

int itg320x_read_temp(const itg320x_t *dev, int16_t *temp)
{
    assert(dev != NULL);
    assert(temp != NULL);

    DEBUG_DEV("temp=%p", dev, temp);

    uint8_t data[2];

    /* read raw temperature */
    EXEC_RET(_reg_read(dev, ITG320X_REG_TEMP_OUT_H, data, 2));

    /* data MSB @ lower address */
    *temp = (data[0] << 8) | data[1];
    /* convert raw temperature data to tenths of a degree Celsius */
    *temp =  (*temp + 13200) / 28 + 350;

    return ITG320X_OK;
}

int itg320x_power_down(itg320x_t *dev)
{
    assert(dev != NULL);

    DEBUG_DEV("", dev);

    return _update_reg(dev, ITG320X_REG_PWR_MGM, ITG320X_REG_PWR_MGM_SLEEP, 1);
}

int itg320x_power_up(itg320x_t *dev)
{
    assert(dev != NULL);

    DEBUG_DEV("", dev);

    EXEC_RET(_update_reg(dev, ITG320X_REG_PWR_MGM, ITG320X_REG_PWR_MGM_SLEEP, 0));

    /* wait 20 ms after power-up */
    xtimer_msleep(20);

    return ITG320X_OK;
}

/** Functions for internal use only */

static int _reset(itg320x_t *dev)
{
    assert(dev != NULL);

    DEBUG_DEV("", dev);

    /* set the reset flag, it automatically reset by the device */
    EXEC_RET(_update_reg(dev, ITG320X_REG_PWR_MGM, ITG320X_REG_PWR_MGM_H_RESET, 1));

    /* wait 20 ms after reset */
    xtimer_msleep(20);

    return ITG320X_OK;
}

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const itg320x_t *dev)
{
    assert(dev != NULL);

    DEBUG_DEV("", dev);

    uint8_t reg;

    /* read the chip id from ITG320X_REG_ID_X */
    EXEC_RET(_reg_read(dev, ITG320X_REG_WHO_AM_I, &reg, 1));

    if (reg != ITG320X_ID) {
        DEBUG_DEV("sensor is not available, wrong id %02x, should be %02x",
                  dev, reg, ITG320X_ID);
        return ITG320X_ERROR_WRONG_ID;
    }

    return ITG320X_OK;
}

static int _update_reg(const itg320x_t *dev, uint8_t reg, uint8_t mask, uint8_t val)
{
    assert(dev != NULL);

    DEBUG_DEV("reg=%02x mask=%02x val=%02x", dev, reg, mask, val);

    uint8_t reg_val;
    uint8_t shift = 0;

    while (!((mask >> shift) & 0x01)) {
        shift++;
    }

    /* read current register value */
    EXEC_RET(_reg_read(dev, reg, &reg_val, 1));

    /* set masked bits to the given value  */
    reg_val = (reg_val & ~mask) | ((val << shift) & mask);

    /* write back new register value */
    EXEC_RET(_reg_write(dev, reg, reg_val));

    return ITG320X_OK;
}

static int _reg_read(const itg320x_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    assert(dev != NULL);
    assert(data != NULL);
    assert(len != 0);

    DEBUG_DEV("read %d bytes from reg 0x%02x", dev, len, reg);

    if (i2c_acquire(dev->params.dev) != 0) {
        DEBUG_DEV("could not acquire the I2C bus", dev);
        return ITG320X_ERROR_I2C;
    }

    int res = i2c_read_regs(dev->params.dev, dev->params.addr, reg, data, len, 0);
    i2c_release(dev->params.dev);

    if (res != 0) {
        DEBUG_DEV("could not read %d bytes from sensor registers "
                  "starting at addr %02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return ITG320X_ERROR_I2C;
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf("[itg320x] %s i2c dev=%d addr=%02x: read %d bytes from reg 0x%02x: ",
            __func__, dev->params.dev, dev->params.addr, len, reg);
        for (unsigned i = 0; i < len; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }

    return ITG320X_OK;
}

static int _reg_write(const itg320x_t *dev, uint8_t reg, uint8_t data)
{
    assert(dev != NULL);

    DEBUG_DEV("write 1 byte to reg 0x%02x: 0x%02x", dev, reg, data);

    if (i2c_acquire(dev->params.dev) != 0) {
        DEBUG_DEV("could not acquire the I2C bus", dev);
        return ITG320X_ERROR_I2C;
    }

    int res = i2c_write_regs(dev->params.dev, dev->params.addr, reg, &data, 1, 0);
    i2c_release(dev->params.dev);

    if (res != 0) {
        DEBUG_DEV("could not write to sensor register 0x%02x, reason %d (%s)",
                  dev, reg, res, strerror(res * -1));
        return ITG320X_ERROR_I2C;
    }

    return ITG320X_OK;
}
