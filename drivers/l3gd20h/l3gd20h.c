/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l3gd20h
 * @brief       Device Driver for ST L3GD20H 3-axis digital output gyroscope
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "l3gd20h_regs.h"
#include "l3gd20h.h"

#include "log.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[l3gd20h] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#if MODULE_L3GD20H_I2C
#define DEBUG_DEV(f, d, ...) \
        DEBUG("[l3gd20h] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__);
#endif

#if MODULE_L3GD20H_SPI
#define DEBUG_DEV(f, d, ...) \
        DEBUG("[l3gd20h] %s spi dev=%d: " f "\n", \
              __func__, d->params.dev, ## __VA_ARGS__);
#endif

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#if MODULE_L3GD20H_I2C
#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[l3gd20h] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.dev, d->params.addr, ## __VA_ARGS__);
#endif

#if MODULE_L3GD20H_SPI
#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[l3gd20h] %s spi dev=%d: " f "\n", \
                  __func__, d->params.dev, ## __VA_ARGS__);
#endif

#define EXEC_RET(f, r) \
    if ((r = f) != L3GD20H_OK) { \
        DEBUG("[l3gd20h] %s: error code %d\n", __func__, res); \
        return res; \
    }

#define EXEC_RET_CODE(f, r, c) \
    if ((r = f) != L3GD20H_OK) { \
        DEBUG("[l3gd20h] %s: error code %d\n", __func__, res); \
        return c; \
    }

/** Forward declaration of functions for internal use */

static int _reset(l3gd20h_t *dev);
static int _is_available(l3gd20h_t *dev);

static uint8_t _get_reg_bit(uint8_t byte, uint8_t mask);
static void    _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit);
static int     _update_reg(const l3gd20h_t *dev, uint8_t reg, uint8_t mask, uint8_t val);

#define msb_lsb_to_type(t, b, o) (t)(((t)b[o] << 8) | b[o + 1])
#define lsb_msb_to_type(t, b, o) (t)(((t)b[o + 1] << 8) | b[o])
#define lsb_to_type(t, b, o)     (t)(b[o])

int l3gd20h_init(l3gd20h_t *dev, const l3gd20h_params_t *params)
{
    int res = L3GD20H_OK;

    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);
    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = *params;

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev), res)

    /* reset the sensor */
    EXEC_RET(_reset(dev), res);

    /* set the operation mode */
    EXEC_RET(l3gd20h_set_mode(dev, dev->params.mode, true, true, true), res);

    /* set full scale */
    EXEC_RET(l3gd20h_set_scale(dev, dev->params.scale), res);

    _update_reg(dev, L3GD20H_REG_CTRL4, L3GD20H_BLOCK_DATA_UPDATE, 1);

    return res;
}

int l3gd20h_set_mode(l3gd20h_t *dev, l3gd20h_mode_t mode,
                     bool x, bool y, bool z)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("mode=%02x x=%d y=%d z=%d", dev, mode, x, y, z);

    int res = L3GD20H_OK;

    if (dev->sensor != L3GD20H &&
        mode != L3GD20H_MODE_POWER_DOWN && mode >= L3GD20H_MODE_12) {
        DEBUG_DEV("low ODRs are not available for this sensor", dev);
        return -L3GD20H_ERROR_INV_MODE;
    }

    uint8_t reg1 = 0;
    uint8_t reg2 = 0;

    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_CTRL1, &reg1, 1), res);

    if (mode != L3GD20H_MODE_POWER_DOWN) {
        /* read current register values */
        EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_LOW_ODR, &reg2, 1), res);

        /* if sensor is in power down mode it takes at least 100 ms to start */
        if (!_get_reg_bit(reg1, L3GD20H_POWER_MODE)) {
            xtimer_usleep(200000);
        }

        if (mode <= L3GD20H_MODE_12) {
            /* high output data rate */
            _set_reg_bit(&reg2, L3GD20H_LOW_ODR, 0);
            _set_reg_bit(&reg1, L3GD20H_ODR_BW, mode);
        }
        else {
            /* low output data rate */
            _set_reg_bit(&reg2, L3GD20H_LOW_ODR, 1);
            _set_reg_bit(&reg1, L3GD20H_ODR_BW, mode - L3GD20H_MODE_12);
        }
        _set_reg_bit(&reg1, L3GD20H_POWER_MODE, 1);
        _set_reg_bit(&reg1, L3GD20H_X_ENABLED, x);
        _set_reg_bit(&reg1, L3GD20H_Y_ENABLED, y);
        _set_reg_bit(&reg1, L3GD20H_Z_ENABLED, z);

        if (dev->sensor == L3GD20H) {
            EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_LOW_ODR, &reg2, 1), res);
        }
    }
    else {
        _set_reg_bit(&reg1, L3GD20H_POWER_MODE, 0);
    }

    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_CTRL1, &reg1, 1), res);

    dev->params.mode = mode;

    return res;
}

int l3gd20h_set_scale(l3gd20h_t *dev, l3gd20h_scale_t scale)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("scale=%02x", dev, scale);

    dev->params.scale = scale;

    /* read CTRL4 register and write scale */
    return _update_reg(dev, L3GD20H_REG_CTRL4, L3GD20H_FULL_SCALE, scale);
}

int l3gd20h_power_down (l3gd20h_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    return _update_reg(dev, L3GD20H_REG_CTRL1, L3GD20H_POWER_MODE, 0);
}

int l3gd20h_power_up (l3gd20h_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    int res = _update_reg(dev, L3GD20H_REG_CTRL1, L3GD20H_POWER_MODE, 1);

    /* it takes up to 100 ms to switch from power down to another mode */
    xtimer_usleep(100000);

    return res;
}

int l3gd20h_sleep (l3gd20h_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    return _update_reg(dev, L3GD20H_REG_CTRL1, L3GD20H_XYZ_ENABLED, 0);
}

int l3gd20h_wake_up (l3gd20h_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    int res = _update_reg(dev, L3GD20H_REG_CTRL1, L3GD20H_XYZ_ENABLED, 7);

    return res;
}

int l3gd20h_data_ready(const l3gd20h_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    int res = L3GD20H_OK;

    uint8_t reg;

    if (dev->params.fifo_mode == L3GD20H_BYPASS) {
        EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_STATUS, &reg, 1), res);
        return _get_reg_bit(reg, L3GD20H_ANY_DATA_READY);
    }
    else {
        EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_FIFO_SRC, &reg, 1), res);
        return _get_reg_bit(reg, L3GD20H_FIFO_FFS);
    }
}

/*
 * scale factors for conversion of raw sensor data to degree for possible
 * sensitivities according to mechanical characteristics in datasheet
 */
static const float L3GD20H_SCALES[3] = { (0.00875F), (0.0175F), (0.070F) };

int l3gd20h_read(const l3gd20h_t *dev, l3gd20h_data_t *data)

{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("data=%p", dev, data);

    int res = L3GD20H_OK;

    l3gd20h_raw_data_t raw;

    EXEC_RET(l3gd20h_read_raw (dev, &raw), res);

    data->x = ((double)raw.x * L3GD20H_SCALES[dev->params.scale]) * 1000;
    data->y = ((double)raw.y * L3GD20H_SCALES[dev->params.scale]) * 1000;
    data->z = ((double)raw.z * L3GD20H_SCALES[dev->params.scale]) * 1000;

    return res;
}

int l3gd20h_read_raw(const l3gd20h_t *dev, l3gd20h_raw_data_t *raw)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL);
    DEBUG_DEV("raw=%p", dev, raw);

    int res = L3GD20H_OK;

#if MODULE_L3GD20H_FULL
    /* in bypass mode we read out the FIFO and return newest data sample */
    if (dev->params.fifo_mode != L3GD20H_BYPASS) {
        l3gd20h_raw_data_fifo_t raw_fifo = {};
        int num;

        /* read raw data samples from FIFO */
        if ((num = l3gd20h_read_raw_fifo(dev, raw_fifo)) < 0) {
            DEBUG_DEV("reading raw data samples from FIFO failed", dev);
            return num;
        }

        if (num == 0) {
            DEBUG_DEV("no raw data in FIFO", dev);
            return -L3GD20H_ERROR_NO_NEW_DATA;
        }

        *raw = raw_fifo[num - 1];

        return res;
    }
#endif /* MODULE_L3GD20H_FULL */

    uint8_t data[6];

    /* read raw data sample */
    if (l3gd20h_reg_read(dev, L3GD20H_REG_OUT_X_L, data, 6)) {
        DEBUG_DEV("could not get raw data", dev);
        return -L3GD20H_ERROR_RAW_DATA;
    }

    /* L3GD20H_REG_CTRL4.BLE = 0, Data LSB @ lower address */
    raw->x = (data[1] << 8) | data[0];
    raw->y = (data[3] << 8) | data[2];
    raw->z = (data[5] << 8) | data[4];

    return res;
}

#if MODULE_L3GD20H_FULL

int l3gd20h_set_fifo_mode(l3gd20h_t *dev, l3gd20h_fifo_mode_t mode,
                          uint8_t thresh)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("mode=%d thresh=%d", dev, mode, thresh);

    int res = L3GD20H_OK;

    if (dev->sensor != L3GD20H && mode > L3GD20H_BYPASS_TO_STREAM) {
        DEBUG_DEV("FIFO mode %02x is not available for this sensor", dev, mode);
        return -L3GD20H_ERROR_INV_FIFO_MODE;
    }

    dev->params.fifo_mode = mode;

    /* read CTRL5 register and write FIFO_EN flag */
    EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL5, L3GD20H_FIFO_EN, (mode != L3GD20H_BYPASS)), res);

    /* read FIFO_CTRL register and write FIFO mode */
    EXEC_RET(_update_reg(dev, L3GD20H_REG_FIFO_CTRL, L3GD20H_FIFO_THRESH, thresh), res);
    EXEC_RET(_update_reg(dev, L3GD20H_REG_FIFO_CTRL, L3GD20H_FIFO_MODE, mode), res);

    return res;
}

int l3gd20h_read_fifo(const l3gd20h_t *dev, l3gd20h_data_fifo_t data)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("", dev);

    l3gd20h_raw_data_fifo_t raw = {};
    int num;

    if ((num = l3gd20h_read_raw_fifo(dev, raw)) < 0) {
        DEBUG_DEV("reading raw data samples from FIFO failed", dev);
        return num;
    }

    for (int i = 0; i < num; i++) {
        data[i].x = ((double)raw[i].x * L3GD20H_SCALES[dev->params.scale]) * 1000;
        data[i].y = ((double)raw[i].y * L3GD20H_SCALES[dev->params.scale]) * 1000;
        data[i].z = ((double)raw[i].z * L3GD20H_SCALES[dev->params.scale]) * 1000;
    }

    return num;
}

int l3gd20h_read_raw_fifo(const l3gd20h_t *dev, l3gd20h_raw_data_fifo_t raw)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL);
    DEBUG_DEV("", dev);

    int res = L3GD20H_OK;

    /* in bypass mode, we use lis3dh_read to return one sample */
    if (dev->params.fifo_mode == L3GD20H_BYPASS) {
        res = l3gd20h_read_raw(dev, raw);
        if (res == L3GD20H_OK) {
            return 1;
        }
    }

    uint8_t reg;

    /* read FIFO state */
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_FIFO_SRC, &reg, 1), res);

    /* if nothing is in the FIFO, just return with 0 */
    if (reg & L3GD20H_FIFO_EMPTY) {
        return 0;
    }

    /* read samples from FIFO */
    int samples = (reg & L3GD20H_FIFO_FFS) + ((reg & L3GD20H_FIFO_OVR) ? 1 : 0);

    /* read samples from FIFO */
    for (int i = 0; i < samples; i++) {

        uint8_t data[6];

        /* read raw data sample */
        EXEC_RET_CODE(l3gd20h_reg_read(dev, L3GD20H_REG_OUT_X_L, data, 6),
                      res, -L3GD20H_ERROR_RAW_DATA_FIFO);

        /* L3GD20H_REG_CTRL4.BLE = 0, Data LSB @ lower address */
        raw[i].x = (data[1] << 8) | data[0];
        raw[i].y = (data[3] << 8) | data[2];
        raw[i].z = (data[5] << 8) | data[4];
    }

    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_FIFO_SRC, &reg, 1), res);

    if (reg & L3GD20H_FIFO_FFS) {
        DEBUG_DEV("New samples stored in FIFO while reading out the FIFO, "
                  "output data rate (ODR) is too high", dev);
    }

    if (dev->params.fifo_mode == L3GD20H_FIFO && samples == 32) {
        /* clean FIFO (see app note) */
        EXEC_RET(_update_reg(dev, L3GD20H_REG_FIFO_CTRL,
                                  L3GD20H_FIFO_MODE, L3GD20H_BYPASS), res);
        EXEC_RET(_update_reg(dev, L3GD20H_REG_FIFO_CTRL,
                                  L3GD20H_FIFO_MODE, L3GD20H_FIFO), res);
    }

    return samples;
}

int l3gd20h_select_output_filter(const l3gd20h_t *dev,
                                 l3gd20h_filter_sel_t filter)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("filter=%02x", dev, filter);

    int res = L3GD20H_OK;

    /* try to set the register OUT_SEL in any case */
    EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL5, L3GD20H_OUT_SEL, filter), res);

    /* try to set HPen in case LPF2 and HPF is used */
    if (filter == L3GD20H_HPF_AND_LPF2) {
        EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL5, L3GD20H_HP_ENABLED, 1), res);
    }

    return res;
}

int l3gd20h_config_hpf(const l3gd20h_t *dev,
                       l3gd20h_hpf_mode_t mode, uint8_t cutoff)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(cutoff <= 9);
    DEBUG_DEV("mode=%02x cutoff=%d", dev, mode, cutoff);

    int res = L3GD20H_OK;

    EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL2, L3GD20H_HPF_MODE, mode), res);
    EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL2, L3GD20H_HPF_CUTOFF, cutoff), res);

    return res;
}

int l3gd20h_set_hpf_ref(const l3gd20h_t *dev, int8_t ref)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("ref=%d", dev, ref);

    return l3gd20h_reg_write(dev, L3GD20H_REG_REFERENCE, (uint8_t *)&ref, 1);
}

int l3gd20h_get_hpf_ref(const l3gd20h_t *dev, int8_t *ref)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(ref != NULL);
    DEBUG_DEV("ref=%p", dev, ref);

    return l3gd20h_reg_read(dev, L3GD20H_REG_REFERENCE, (uint8_t *)&ref, 1);
}

int l3gd20h_enable_int(const l3gd20h_t *dev,
                       l3gd20h_int_types_t type, bool enable)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("type=%d enable=%d", dev, type, enable);

    switch (type) {
        case L3GD20H_INT_EVENT:
                if (dev->params.int1 == GPIO_UNDEF) {
                    DEBUG_DEV("INT1 signal pin not configured", dev);
                    return -L3GD20H_ERROR_NO_INT1_PIN;
                }
                break;
        case L3GD20H_INT_DATA_READY:
        case L3GD20H_INT_FIFO_THRESHOLD:
        case L3GD20H_INT_FIFO_OVERRUN:
        case L3GD20H_INT_FIFO_EMPTY:
                if (dev->params.int2 == GPIO_UNDEF) {
                    DEBUG_DEV("INT2 signal pin not configured", dev);
                    return -L3GD20H_ERROR_NO_INT2_PIN;
                }
                break;
        default:
                DEBUG_DEV("wrong interrupt type", dev);
                return -L3GD20H_ERROR_INV_INT_TYPE;
    }

    uint8_t mask;

    switch (type) {
        case L3GD20H_INT_EVENT:          mask  = L3GD20H_INT1_IG;    break;
        case L3GD20H_INT_DATA_READY:     mask  = L3GD20H_INT2_DRDY;  break;
        case L3GD20H_INT_FIFO_THRESHOLD: mask  = L3GD20H_INT2_FTH;   break;
        case L3GD20H_INT_FIFO_OVERRUN:   mask  = L3GD20H_INT2_ORUN;  break;
        case L3GD20H_INT_FIFO_EMPTY:     mask  = L3GD20H_INT2_EMPTY; break;
        default: return -L3GD20H_ERROR_INV_INT_TYPE;
    }

    return _update_reg(dev, L3GD20H_REG_CTRL3, mask, enable);
}

int l3gd20h_config_int_signals(const l3gd20h_t *dev,
                               l3gd20h_int_pin_type_t type,
                               l3gd20h_int_pin_level_t level)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("type=%d level=%d", dev, type, level);

    int res = L3GD20H_OK;

    EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL3, L3GD20H_HL_ACTIVE, level), res);
    EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL3, L3GD20H_PP_OD, type), res);
    EXEC_RET(_update_reg(dev, L3GD20H_REG_LOW_ODR, L3GD20H_DRDY_HL, level), res);

    return res;
}

int l3gd20h_set_int_event_config(const l3gd20h_t *dev,
                                 const l3gd20h_int_event_config_t *config)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(config != NULL);
    DEBUG_DEV("config=%p", dev, config);

    int res = L3GD20H_OK;

    uint8_t ig_cfg = 0;
    uint8_t ig_dur = 0;
    uint8_t ig_ths[6] = { 0 };

    _set_reg_bit(&ig_cfg, L3GD20H_INT1_X_LOW, config->x_low_enabled);
    _set_reg_bit(&ig_cfg, L3GD20H_INT1_X_HIGH, config->x_high_enabled);

    _set_reg_bit(&ig_cfg, L3GD20H_INT1_Y_LOW, config->y_low_enabled);
    _set_reg_bit(&ig_cfg, L3GD20H_INT1_Y_HIGH, config->y_high_enabled);

    _set_reg_bit(&ig_cfg, L3GD20H_INT1_Z_LOW, config->z_low_enabled);
    _set_reg_bit(&ig_cfg, L3GD20H_INT1_Z_HIGH, config->z_high_enabled);

    _set_reg_bit(&ig_cfg, L3GD20H_INT1_LATCH, config->latch);
    _set_reg_bit(&ig_cfg, L3GD20H_INT1_AND_OR, config->and_or);

    _set_reg_bit(&ig_dur, L3GD20H_INT1_WAIT, config->wait);
    _set_reg_bit(&ig_dur, L3GD20H_INT1_DURATION, config->duration);

    ig_ths[0] = (config->x_threshold >> 8) & 0x7f;
    ig_ths[1] = (config->x_threshold & 0xff);
    ig_ths[2] = (config->y_threshold >> 8) & 0x7f;
    ig_ths[3] = (config->y_threshold & 0xff);
    ig_ths[4] = (config->z_threshold >> 8) & 0x7f;
    ig_ths[5] = (config->z_threshold & 0xff);

    /* write the thresholds to registers IG_THS_* */
    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_IG_THS_XH, ig_ths, 6), res);

    /* write duration configuration to IG_DURATION  */
    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_IG_DURATION, &ig_dur, 1), res);

    /* write INT1 configuration  to IG_CFG */
    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_IG_CFG, &ig_cfg, 1), res);

    /* filter selection used for threshold comparison for INT1 generation */
    EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL5, L3GD20H_IG_SEL, config->filter), res);

    /* try to set HPen in case LPF2 and HPF is used */
    if (config->filter == L3GD20H_HPF_AND_LPF2) {
        EXEC_RET(_update_reg(dev, L3GD20H_REG_CTRL5, L3GD20H_HP_ENABLED, 1), res);
    }

    return res;
}

int l3gd20h_get_int_event_config(const l3gd20h_t *dev,
                                 l3gd20h_int_event_config_t *config)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(config != NULL);
    DEBUG_DEV("config=%p", dev, config);

    int res = L3GD20H_OK;

    uint8_t ig_cfg;
    uint8_t ig_dur;
    uint8_t ig_ths[6];
    uint8_t ctrl3;
    uint8_t ctrl5;

    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_IG_THS_XH, ig_ths, 6), res);
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_IG_CFG, &ig_cfg, 1), res);
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_IG_DURATION, &ig_dur, 1), res);
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_CTRL3, &ctrl3, 1), res);
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_CTRL5, &ctrl5, 1), res);

    config->x_low_enabled   = _get_reg_bit(ig_cfg, L3GD20H_INT1_X_LOW);
    config->x_high_enabled  = _get_reg_bit(ig_cfg, L3GD20H_INT1_X_HIGH);

    config->y_low_enabled   = _get_reg_bit(ig_cfg, L3GD20H_INT1_Y_LOW);
    config->y_high_enabled  = _get_reg_bit(ig_cfg, L3GD20H_INT1_Y_HIGH);

    config->z_low_enabled   = _get_reg_bit(ig_cfg, L3GD20H_INT1_Z_LOW);
    config->z_high_enabled  = _get_reg_bit(ig_cfg, L3GD20H_INT1_Z_HIGH);

    config->x_threshold     = msb_lsb_to_type(uint16_t, ig_ths, 0);
    config->y_threshold     = msb_lsb_to_type(uint16_t, ig_ths, 2);
    config->z_threshold     = msb_lsb_to_type(uint16_t, ig_ths, 4);

    config->filter          = _get_reg_bit(ctrl5, L3GD20H_IG_SEL);

    config->and_or          = _get_reg_bit(ig_cfg, L3GD20H_INT1_AND_OR);
    config->latch           = _get_reg_bit(ig_cfg, L3GD20H_INT1_LATCH);

    config->wait            = _get_reg_bit(ig_dur, L3GD20H_INT1_WAIT);
    config->duration        = _get_reg_bit(ig_dur, L3GD20H_INT1_DURATION);

    config->counter_mode    = 0;

    return res;
}

int l3gd20h_get_int_event_source(const l3gd20h_t *dev,
                                 l3gd20h_int_event_source_t *source)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(source != NULL);
    DEBUG_DEV("source=%p", dev, source);

    int res = L3GD20H_OK;

    l3gd20h_int_event_source_t ig_cfg;
    l3gd20h_int_event_source_t ig_src;

    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_IG_CFG, (uint8_t *)&ig_cfg, 1), res);
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_IG_SRC, (uint8_t *)&ig_src, 1), res);

    source->active = ig_src.active;
    source->x_low  = ig_src.x_low  & ig_cfg.x_low;
    source->x_high = ig_src.x_high & ig_cfg.x_high;
    source->y_low  = ig_src.y_low  & ig_cfg.y_low;
    source->y_high = ig_src.y_high & ig_cfg.y_high;
    source->z_low  = ig_src.z_low  & ig_cfg.z_low;
    source->z_high = ig_src.z_high & ig_cfg.z_high;

    return res;
}

int l3gd20h_get_int_data_source(const l3gd20h_t *dev,
                                l3gd20h_int_data_source_t *source)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(source != NULL);
    DEBUG_DEV("source=%p", dev, source);

    int res = L3GD20H_OK;

    uint8_t fifo_src;
    uint8_t status;

    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_STATUS, &status, 1), res);
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_FIFO_SRC, &fifo_src, 1), res);

    source->data_ready     = _get_reg_bit(status, L3GD20H_ANY_DATA_READY);
    source->fifo_threshold = _get_reg_bit(fifo_src, L3GD20H_FIFO_THS);
    source->fifo_overrun   = _get_reg_bit(fifo_src, L3GD20H_FIFO_OVR);
    source->fifo_empty     = _get_reg_bit(fifo_src, L3GD20H_FIFO_EMPTY);

    return res;
}

#endif /* MODULE_L3GD20H_FULL */

#if MODULE_L3GD20H_SPI

#define L3GD20H_SPI_BUF_SIZE 64      /* SPI register data buffer size */

#define L3GD20H_SPI_READ_FLAG      0x80
#define L3GD20H_SPI_WRITE_FLAG     0x00
#define L3GD20H_SPI_AUTO_INC_FLAG  0x40

int l3gd20h_reg_read(const l3gd20h_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);
    ASSERT_PARAM(len < L3GD20H_SPI_BUF_SIZE)

    DEBUG_DEV("read %d byte from sensor registers starting at addr 0x%02x",
              dev, len, reg);

    uint8_t addr = (reg & 0x3f) | L3GD20H_SPI_READ_FLAG | L3GD20H_SPI_AUTO_INC_FLAG;

    /* prepae input and output by stream */
    static uint8_t mosi[L3GD20H_SPI_BUF_SIZE];
    static uint8_t miso[L3GD20H_SPI_BUF_SIZE];

    memset(mosi, 0xff, L3GD20H_SPI_BUF_SIZE);
    memset(miso, 0xff, L3GD20H_SPI_BUF_SIZE);

    /* in output stream only the first by is filled with register address */
    mosi[0] = addr;

    if (spi_acquire(dev->params.dev, dev->params.cs, SPI_MODE_0, dev->params.clk)) {
        DEBUG_DEV("could not aquire SPI bus", dev);
        return -L3GD20H_ERROR_SPI;
    }

    spi_transfer_bytes(dev->params.dev, dev->params.cs, false, mosi, miso, len + 1);
    spi_release(dev->params.dev);

    /* shift data one by left, first byte received while sending register address is invalid */
    for (int i = 0; i < len; i++) {
        data[i] = miso[i + 1];
    }

    if (ENABLE_DEBUG) {
        printf("[l3gd20h] %s spi dev=%d: read following bytes: ", __func__, dev->params.dev);
        for (int i = 0; i < len; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }

    return L3GD20H_OK;
}


int l3gd20h_reg_write(const l3gd20h_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);
    ASSERT_PARAM(len < L3GD20H_SPI_BUF_SIZE)

    DEBUG_DEV("write %d bytes to sensor registers starting at addr 0x%02x",
              dev, len, reg);

    uint8_t addr = (reg & 0x3f) | L3GD20H_SPI_WRITE_FLAG | L3GD20H_SPI_AUTO_INC_FLAG;

    static uint8_t mosi[L3GD20H_SPI_BUF_SIZE];

    reg &= 0x7f;

    /* first byte in output is the register address */
    mosi[0] = addr;

    /* shift data one byte right, first byte in output is the register address */
    for (int i = 0; i < len; i++) {
        mosi[i + 1] = data[i];
    }

    if (ENABLE_DEBUG) {
        printf("[l3gd20h] %s spi dev=%d: write following bytes: ",
               __func__, dev->params.dev);
        for (int i = 1; i < len + 1; i++) {
            printf("%02x ", mosi[i]);
        }
        printf("\n");
    }

    if (spi_acquire(dev->params.dev, dev->params.cs, SPI_MODE_0, dev->params.clk)) {
        DEBUG_DEV("could not aquire SPI bus", dev);
        return -L3GD20H_ERROR_SPI;
    }

    spi_transfer_bytes(dev->params.dev, dev->params.cs, false, mosi, 0, len + 1);
    spi_release(dev->params.dev);

    return L3GD20H_OK;
}

#endif /* MODULE_L3GD20H_SPI */

#if MODULE_L3GD20H_I2C

#define I2C_AUTO_INCREMENT    (0x80)

int l3gd20h_reg_read(const l3gd20h_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("read %d byte from sensor registers starting at addr 0x%02x",
              dev, len, reg);

    if (len > 1) {
        reg |= I2C_AUTO_INCREMENT;
    }

    if (i2c_acquire(dev->params.dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -L3GD20H_ERROR_I2C;
    }

    int res = i2c_read_regs(dev->params.dev, dev->params.addr, reg, data, len, 0);
    i2c_release(dev->params.dev);

    if (res == L3GD20H_OK) {
        if (ENABLE_DEBUG) {
            printf("[l3gd20h] %s i2c dev=%d addr=%02x: read following bytes: ",
                   __func__, dev->params.dev, dev->params.addr);
            for (int i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
    }
    else {
        DEBUG_DEV("could not read %d bytes from sensor registers "
                  "starting at addr %02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return -L3GD20H_ERROR_I2C;
    }

    return res;
}

int l3gd20h_reg_write(const l3gd20h_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("write %d bytes to sensor registers starting at addr 0x%02x",
              dev, len, reg);

    if (len > 1) {
        reg |= I2C_AUTO_INCREMENT;
    }

    if (ENABLE_DEBUG) {
        printf("[l3gd20h] %s i2c dev=%d addr=%02x: write following bytes: ",
               __func__, dev->params.dev, dev->params.addr);
        for (int i = 0; i < len; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }

    if (i2c_acquire(dev->params.dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -L3GD20H_ERROR_I2C;
    }

    int res;

    if (!data || !len) {
        res = i2c_write_byte(dev->params.dev, dev->params.addr, reg, 0);
    }
    else {
        res = i2c_write_regs(dev->params.dev, dev->params.addr, reg, data, len, 0);
    }
    i2c_release(dev->params.dev);

    if (res != L3GD20H_OK) {
        DEBUG_DEV("could not write %d bytes to sensor registers "
                  "starting at addr 0x%02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return -L3GD20H_ERROR_I2C;
    }

    return res;
}

#endif /* MODULE_L3GD20H_I2C */
/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(l3gd20h_t *dev)
{
    DEBUG_DEV("", dev);

    int res = L3GD20H_OK;

    uint8_t chip_id;

    /* read the chip id from L3GD20H_REG_WHO_AM_I */
    EXEC_RET(l3gd20h_reg_read(dev, L3GD20H_REG_WHO_AM_I, &chip_id, 1), res);

    /* detrmine the sensor type */
    switch (chip_id) {
        case L3GD20H_CHIP_ID:  dev->sensor = L3GD20H;
                               break;
        case L3GD20_CHIP_ID:   dev->sensor = L3GD20;
                               break;
        case L3G4200D_CHIP_ID: dev->sensor = l3G4200D;
                               break;

        default: DEBUG_DEV("sensor is not available, wrong chip id %02x, "
                           "should be %02x", dev, chip_id, L3GD20H_CHIP_ID);
                 return -L3GD20H_ERROR_WRONG_CHIP_ID;
    }

    return res;
}


static int _reset(l3gd20h_t *dev)
{
    DEBUG_DEV("", dev);

    int res = L3GD20H_OK;

    /* send software reset */
    EXEC_RET(_update_reg(dev, L3GD20H_REG_LOW_ODR, L3GD20H_SW_RESET, 1), res);

    /* wait 100 ms after reset */
    xtimer_usleep(100000);

    uint8_t reg[6] = { 0 };

    /* initialize sensor completely including setting in power down mode */
    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_CTRL1, reg, 6), res);
    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_FIFO_CTRL, reg, 1), res);
    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_IG_CFG, reg, 1), res);
    EXEC_RET(l3gd20h_reg_write(dev, L3GD20H_REG_IG_THS_XH, reg, 6), res);

    return res;
}

static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit)
{
    ASSERT_PARAM(byte != NULL);

    uint8_t shift = 0;
    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    *byte = ((*byte & ~mask) | ((bit << shift) & mask));
}


static uint8_t _get_reg_bit(uint8_t byte, uint8_t mask)
{
    uint8_t shift = 0;

    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    return (byte & mask) >> shift;
}

static int _update_reg(const l3gd20h_t *dev, uint8_t reg, uint8_t mask, uint8_t val)
{
    DEBUG_DEV("reg=%02x mask=%02x val=%02x", dev, reg, mask, val);

    int res = L3GD20H_OK;

    uint8_t reg_val;
    uint8_t shift = 0;

    while (!((mask >> shift) & 0x01)) {
        shift++;
    }

    /* read current register value */
    EXEC_RET(l3gd20h_reg_read(dev, reg, &reg_val, 1), res);

    /* set masked bits to the given value  */
    reg_val = (reg_val & ~mask) | ((val << shift) & mask);

    /* write back new register value */
    EXEC_RET(l3gd20h_reg_write(dev, reg, &reg_val, 1), res);

    return res;
}
