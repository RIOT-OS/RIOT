/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hmc5883l
 * @brief       Device driver for the Honeywell HMC5883L 3-axis digital compass
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "hmc5883l_regs.h"
#include "hmc5883l.h"

#include "log.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[hmc5883l] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->dev, HMC5883L_I2C_ADDRESS, ## __VA_ARGS__);

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[hmc5883l] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->dev, HMC5883L_I2C_ADDRESS, ## __VA_ARGS__);

#define EXEC_RET(f, r) \
    if ((r = f) != HMC5883L_OK) { \
        DEBUG("[hmc5883l] %s: error code %d\n", __func__, res); \
        return res; \
    }

#define EXEC_RET_CODE(f, r, c) \
    if ((r = f) != HMC5883L_OK) { \
        DEBUG("[hmc5883l] %s: error code %d\n", __func__, res); \
        return c; \
    }

/** Forward declaration of functions for internal use */

static int _is_available(const hmc5883l_t *dev);

static int _reg_read(const hmc5883l_t *dev, uint8_t reg, uint8_t *data, uint16_t len);
static int _reg_write(const hmc5883l_t *dev, uint8_t reg, uint8_t data);

int hmc5883l_init(hmc5883l_t *dev, const hmc5883l_params_t *params)
{
    int res = HMC5883L_OK;

    assert(dev != NULL);
    assert(params != NULL);
    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->dev = params->dev;
#if MODULE_HMC5883L_INT
    dev->int_pin = params->int_pin;
#endif
    dev->op_mode = params->op_mode;
    dev->gain = params->gain;

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev), res)

    /* set configuration register A and B */
    EXEC_RET(_reg_write(dev, HMC5883L_REG_CFG_A, params->meas_avg |
                                                 params->meas_mode |
                                                 params->dor), res);
    EXEC_RET(_reg_write(dev, HMC5883L_REG_CFG_B, params->gain), res);

    /* set operation mode */
    EXEC_RET(_reg_write(dev, HMC5883L_REG_MODE, params->op_mode), res);

    /* to set the LOCK flag, read the first data sample that is not valid */
    uint8_t data[6];
    EXEC_RET(_reg_read(dev, HMC5883L_REG_OUT_X_MSB, data, 6), res);

    return res;
}

#ifdef MODULE_HMC5883L_INT

int hmc5883l_init_int(hmc5883l_t *dev, hmc5883l_drdy_int_cb_t cb, void *arg)
{
    assert(dev != NULL);
    assert(gpio_is_valid(dev->int_pin));
    DEBUG_DEV("", dev);

    if (gpio_init_int(dev->int_pin, GPIO_IN, GPIO_FALLING, cb, arg)) {
        return HMC5883L_ERROR_COMMON;
    }

    return HMC5883L_OK;
}

#endif /* MODULE_HMC5883L_INT */

int hmc5883l_data_ready(const hmc5883l_t *dev)
{
    assert(dev != NULL);
    DEBUG_DEV("", dev);

    int res = HMC5883L_OK;

    uint8_t reg;

    EXEC_RET(_reg_read(dev, HMC5883L_REG_STATUS, &reg, 1), res);
    return (reg == HMC5883L_REG_STATUS_RDY) ? HMC5883L_OK : HMC5883L_ERROR_NO_DATA;
}

/*
 * Scale factors for conversion of raw sensor data to uGs for possible
 * sensitivities according to the datasheet.
 */
static const uint16_t HMC5883L_RES[] = {
     730,      /* uG/LSb for HMC5883L_GAIN_1370 with range +-0.88 Gs */
     917,      /* uG/LSb for HMC5883L_GAIN_1090 with range +-1.3 Gs */
    1220,      /* uG/LSb for HMC5883L_GAIN_820  with range +-1.9 Gs */
    1515,      /* uG/LSb for HMC5883L_GAIN_660  with range +-2.5 Gs */
    2273,      /* uG/LSb for HMC5883L_GAIN_440  with range +-4.0 Gs */
    2564,      /* uG/LSb for HMC5883L_GAIN_390  with range +-4.7 Gs */
    3030,      /* uG/LSb for HMC5883L_GAIN_330  with range +-5.6 Gs */
    4348,      /* uG/LSb for HMC5883L_GAIN_230  with range +-8.1 Gs */
};

int hmc5883l_read(const hmc5883l_t *dev, hmc5883l_data_t *data)

{
    assert(dev != NULL);
    assert(data != NULL);
    DEBUG_DEV("data=%p", dev, data);

    int res = HMC5883L_OK;

    hmc5883l_raw_data_t raw;

    EXEC_RET(hmc5883l_read_raw (dev, &raw), res);

    /*
     * The range of raw data is -2048 ... -2047. That is, raw data multiplied
     * by scale fit into 32 bit integer.
     */
    data->x = ((int32_t)raw.x * HMC5883L_RES[dev->gain >> HMC5883L_REG_CFG_B_GN_S]) / 1000;
    data->y = ((int32_t)raw.y * HMC5883L_RES[dev->gain >> HMC5883L_REG_CFG_B_GN_S]) / 1000;
    data->z = ((int32_t)raw.z * HMC5883L_RES[dev->gain >> HMC5883L_REG_CFG_B_GN_S]) / 1000;

    return res;
}

int hmc5883l_read_raw(const hmc5883l_t *dev, hmc5883l_raw_data_t *raw)
{
    assert(dev != NULL);
    assert(raw != NULL);
    DEBUG_DEV("raw=%p", dev, raw);

    int res = HMC5883L_OK;

    uint8_t data[6];

    /* read raw data sample */
    EXEC_RET_CODE(_reg_read(dev, HMC5883L_REG_OUT_X_MSB, data, 6),
                  res, HMC5883L_ERROR_RAW_DATA);

    /* data MSB @ lower address */
    raw->x = (data[0] << 8) | data[1];
    raw->y = (data[4] << 8) | data[5];
    raw->z = (data[2] << 8) | data[3];

    return res;
}

int hmc5883l_power_down(hmc5883l_t *dev)
{
    assert(dev != NULL);
    DEBUG_DEV("", dev);

    /* set operation mode to Idle mode with only 5 uA current */
    return _reg_write(dev, HMC5883L_REG_MODE, HMC5883L_OP_MODE_IDLE);
}

int hmc5883l_power_up(hmc5883l_t *dev)
{
    assert(dev != NULL);
    DEBUG_DEV("", dev);

    /* set operation mode to last operation mode */
    return _reg_write(dev, HMC5883L_REG_MODE, dev->op_mode);
}

/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const hmc5883l_t *dev)
{
    DEBUG_DEV("", dev);

    int res = HMC5883L_OK;

    uint8_t id_c[] = HMC5883L_ID;
    uint8_t id_r[HMC5883L_ID_LEN];

    /* read the chip id from HMC5883L_REG_ID_X */
    EXEC_RET(_reg_read(dev, HMC5883L_REG_ID_A, id_r, HMC5883L_ID_LEN), res);

    if (memcmp(id_r, id_c, HMC5883L_ID_LEN)) {
        DEBUG_DEV("sensor is not available, wrong id %02x%02x%02x, "
                  "should be %02x%02x%02x",
                  dev, id_r[0], id_r[1], id_r[2], id_c[0], id_c[1], id_c[2]);
        return HMC5883L_ERROR_WRONG_ID;
    }

    return res;
}

static int _reg_read(const hmc5883l_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    assert(dev != NULL);
    assert(data != NULL);
    assert(len != 0);

    DEBUG_DEV("read %d byte from sensor registers starting at addr 0x%02x",
              dev, len, reg);

    i2c_acquire(dev->dev);
    int res = i2c_read_regs(dev->dev, HMC5883L_I2C_ADDRESS, reg, data, len, 0);
    i2c_release(dev->dev);

    if (res == 0) {
        if (IS_ACTIVE(ENABLE_DEBUG)) {
            printf("[hmc5883l] %s i2c dev=%d addr=%02x: read following bytes: ",
                   __func__, dev->dev, HMC5883L_I2C_ADDRESS);
            for (unsigned i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
    }
    else {
        DEBUG_DEV("could not read %d bytes from sensor registers "
                  "starting at addr %02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return HMC5883L_ERROR_I2C;
    }

    return res;
}

static int _reg_write(const hmc5883l_t *dev, uint8_t reg, uint8_t data)
{
    assert(dev != NULL);

    DEBUG_DEV("write register 0x%02x", dev, reg);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf("[hmc5883l] %s i2c dev=%d addr=%02x: write following bytes: ",
               __func__, dev->dev, HMC5883L_I2C_ADDRESS);
        printf("%02x ", data);
        printf("\n");
    }

    i2c_acquire(dev->dev);
    int res = i2c_write_regs(dev->dev, HMC5883L_I2C_ADDRESS, reg, &data, 1, 0);
    i2c_release(dev->dev);

    if (res != 0) {
        DEBUG_DEV("could not write to sensor registers "
                  "starting at addr 0x%02x, reason %d (%s)",
                  dev, reg, res, strerror(res * -1));
        return HMC5883L_ERROR_I2C;
    }

    return res;
}
