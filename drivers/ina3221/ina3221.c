/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ina3221
 * @{
 *
 * @file
 * @brief       Device driver implementation for Texas Instruments INA3221
 *              three-channel, high-side current and bus voltage
 *              monitor
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include "byteorder.h"
#include "ina3221_internal.h"
#include "ina3221_regs.h"
#include "ina3221.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* register addresses differ by 2 */
#define INA3221_REG_CH_SHUNT_VOLTAGE(i) \
        (INA3221_REG_CH1_SHUNT_VOLTAGE + (i * 2))

#define INA3221_REG_CH_BUS_VOLTAGE(i) \
        (INA3221_REG_CH1_BUS_VOLTAGE + (i * 2))

#define INA3221_REG_CH_CRIT_ALERT_LIMIT(i) \
        (INA3221_REG_CH1_CRIT_ALERT_LIMIT + (i * 2))

#define INA3221_REG_CH_WARN_ALERT_LIMIT(i) \
        (INA3221_REG_CH1_WARN_ALERT_LIMIT + (i * 2))

/**
 * @brief Read register value
 *
 * @param[in]   dev Device handle
 * @param[in]   reg Register address
 * @param[out]  out Output register value
 *
 * @post        @p out is in host byte order
 *
 * @return      0, on success
 * @return      -EIO, if i2c bus acquirement failed
 * @return      @see i2c_read_regs
 */
static int _read_reg(const ina3221_t *dev, uint8_t reg, uint16_t *out)
{
    if (i2c_acquire(dev->params.i2c)) {
        return -EIO;
    }
    int status = i2c_read_regs(dev->params.i2c, dev->params.addr, reg, out,
                               INA3221_REG_LEN, 0);
    i2c_release(dev->params.i2c);
    if (status < 0) {
        return status;
    }
    *out = ntohs(*out);
    return 0;
}

/**
 * @brief Write register value
 *
 * @param[in]   dev Device handle
 * @param[in]   reg Register address
 * @param[out]  in Input register value
 *
 * @pre         @p in must be in host byte order
 *
 * @return      0, on success
 * @return      -EIO, if i2c bus acquirement failed
 * @return      @see i2c_write_regs
 */
static int _write_reg(const ina3221_t *dev, uint8_t reg, uint16_t in)
{
    in = htons(in);
    if (i2c_acquire(dev->params.i2c)) {
        return -EIO;
    }
    int status = i2c_write_regs(dev->params.i2c, dev->params.addr, reg, &in,
                                INA3221_REG_LEN, 0);
    i2c_release(dev->params.i2c);
    if (status < 0) {
        return status;
    }
    return 0;
}

int ina3221_reset(ina3221_t *dev)
{
    uint16_t config;
    int status = _write_reg(dev, INA3221_REG_CONFIGURATION, INA3221_RESET);
    if (status < 0) {
        return status;
    }
    /* Check if default config is present after reset */
    status = _read_reg(dev, INA3221_REG_CONFIGURATION, &config);
    if (status < 0) {
        return status;
    }
    if (config != INA3221_DEFCONFIG) {
        return -ENODEV;
    }
    dev->params.config = INA3221_DEFCONFIG;
    return 0;
}

int ina3221_init(ina3221_t *dev, const ina3221_params_t *params)
{
    int status;
    if (!dev || !params) {
        return -EFAULT;
    }
    dev->params = *params;
    uint16_t id;
    status = _read_reg(dev, INA3221_REG_MANUFACTURER_ID, &id);
    if (status < 0) {
        return status;
    }
    if (id != INA3221_MANUFACTURER_ID) {
        return -ENXIO;
    }
    status = _read_reg(dev, INA3221_REG_DIE_ID, &id);
    if (status < 0) {
        return status;
    }
    if (id != INA3221_DIE_ID) {
        return -ENXIO;
    }
    if (ina3221_reset(dev) != 0) {
        return -ENODEV;
    }
    uint16_t cfg;
    if ((ina3221_set_config(dev, params->config) != 0) ||
        (ina3221_get_config(dev, &cfg) != 0)) {
        return -EINVAL;
    }
#if IS_USED(MODULE_INA3221_ALERTS) || defined(DOXYGEN)
    memset(dev->alert_callbacks, 0, sizeof(dev->alert_callbacks));
    memset(dev->alert_callback_arguments, 0,
           sizeof(dev->alert_callback_arguments));
#endif
    return 0;
}

int ina3221_set_config(ina3221_t *dev, uint16_t cfg)
{
    cfg &= ~INA3221_RESET; /* prevent accidental reset */
    int status = _write_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    dev->params.config = cfg;
    return 0;
}

int ina3221_get_config(const ina3221_t *dev, uint16_t *cfg)
{
    int status = _read_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    *cfg &= ~INA3221_RESET; /* clear reset flag */
    return 0;
}

int ina3221_set_enable_channel(ina3221_t *dev, ina3221_ch_t ch)
{
    uint16_t cfg;
    int status;
    if ((status = ina3221_get_config(dev, &cfg)) < 0) {
        return status;
    }
    ina3221_config_set_enabled_channels(&cfg, ch);
    if ((status = ina3221_set_config(dev, cfg)) < 0) {
        return status;
    }
    return 0;
}

int ina3221_set_num_samples(ina3221_t *dev, ina3221_num_samples_t ns)
{
    uint16_t cfg;
    int status;
    if ((status = ina3221_get_config(dev, &cfg)) < 0) {
        return status;
    }
    ina3221_config_set_num_samples(&cfg, ns);
    if ((status = ina3221_set_config(dev, cfg)) < 0) {
        return status;
    }
    return 0;
}

int ina3221_set_conv_time_bus_adc(ina3221_t *dev,
                                  ina3221_conv_time_bus_adc_t ctb)
{
    uint16_t cfg;
    int status;
    if ((status = ina3221_get_config(dev, &cfg)) < 0) {
        return status;
    }
    ina3221_config_set_conv_time_bus(&cfg, ctb);
    if ((status = ina3221_set_config(dev, cfg)) < 0) {
        return status;
    }
    return 0;
}

int ina3221_set_conv_time_shunt_adc(ina3221_t *dev,
                                    ina3221_conv_time_shunt_adc_t cts)
{
    uint16_t cfg;
    int status;
    if ((status = ina3221_get_config(dev, &cfg)) < 0) {
        return status;
    }
    ina3221_config_set_conv_time_shunt(&cfg, cts);
    if ((status = ina3221_set_config(dev, cfg)) < 0) {
        return status;
    }
    return 0;
}

int ina3221_set_mode(ina3221_t *dev, ina3221_mode_t mode)
{
    uint16_t cfg;
    int status;
    if ((status = ina3221_get_config(dev, &cfg)) < 0) {
        return status;
    }
    ina3221_config_set_mode(&cfg, mode);
    if ((status = ina3221_set_config(dev, cfg)) < 0) {
        return status;
    }
    return 0;
}

int ina3221_set_enable_sum_channel(const ina3221_t *dev, ina3221_ch_t ch)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);
    if (status < 0) {
        return status;
    }
    mask_en &= ~INA3221_ENABLE_SUM_CH_MASK;
    mask_en |= ((ch & INA3221_CH1) ? INA3221_ENABLE_SUM_CH1 : 0) |
               ((ch & INA3221_CH2) ? INA3221_ENABLE_SUM_CH2 : 0) |
               ((ch & INA3221_CH3) ? INA3221_ENABLE_SUM_CH3 : 0);
    status = _write_reg(dev, INA3221_REG_MASK_ENABLE, mask_en);
    if (status < 0) {
        return status;
    }
    return 0;
}

int ina3221_get_enable_sum_channel(const ina3221_t *dev, ina3221_ch_t *ch)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);
    if (status < 0) {
        return status;
    }
    *ch = ((mask_en & INA3221_ENABLE_SUM_CH1) ? INA3221_CH1 : 0) |
          ((mask_en & INA3221_ENABLE_SUM_CH2) ? INA3221_CH2 : 0) |
          ((mask_en & INA3221_ENABLE_SUM_CH3) ? INA3221_CH3 : 0);
    return 0;
}

int ina3221_set_latch(const ina3221_t *dev, bool warn, bool crit)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);
    if (status < 0) {
        return status;
    }
    mask_en &= ~INA3221_ENABLE_LATCH_MASK;
    mask_en |= (warn ? INA3221_ENABLE_WARN_LATCH : 0);
    mask_en |= (crit ? INA3221_ENABLE_CRIT_LATCH : 0);
    status = _write_reg(dev, INA3221_REG_MASK_ENABLE, mask_en);
    if (status < 0) {
        return status;
    }
    return 0;
}

int ina3221_get_latch(const ina3221_t *dev, bool *warn, bool *crit)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);
    if (status < 0) {
        return status;
    }
    *warn = !!(mask_en & INA3221_ENABLE_WARN_LATCH);
    *crit = !!(mask_en & INA3221_ENABLE_CRIT_LATCH);
    return 0;
}

ina3221_ch_t ina3221_set_crit_alert_limit(const ina3221_t *dev,
                                          ina3221_ch_t ch, int32_t in_uv)
{
    if (in_uv < INA3221_MIN_SHUNT_UV || in_uv > INA3221_MAX_SHUNT_UV) {
        return 0;
    }
    int16_t reg_val = shunt_voltage_uv_to_reg_val(in_uv);
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1U << i)) {
            if (_write_reg(dev, INA3221_REG_CH_CRIT_ALERT_LIMIT(i), reg_val)) {
                ch &= ~(1U << i);
            }
        }
    }
    return ch;
}

ina3221_ch_t ina3221_get_crit_alert_limit(const ina3221_t *dev, ina3221_ch_t ch,
                                          int32_t out_uv[INA3221_NUM_CH])
{
    uint16_t reg_val;
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        out_uv[i] = 0;
        if (ch & (1U << i)) {
            if (_read_reg(dev, INA3221_REG_CH_CRIT_ALERT_LIMIT(i), &reg_val)) {
                ch &= ~(1U << i);
            }
            else {
                out_uv[i] = reg_val_to_shunt_voltage_uv(reg_val);
            }
        }
    }
    return ch;
}

ina3221_ch_t ina3221_set_warn_alert_limit(const ina3221_t *dev,
                                          ina3221_ch_t ch, int32_t in_uv)
{
    if (in_uv < INA3221_MIN_SHUNT_UV || in_uv > INA3221_MAX_SHUNT_UV) {
        return 0;
    }
    int16_t reg_val = shunt_voltage_uv_to_reg_val(in_uv);
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1U << i)) {
            if (_write_reg(dev, INA3221_REG_CH_WARN_ALERT_LIMIT(i), reg_val)) {
                ch &= ~(1U << i);
            }
        }
    }
    return ch;
}

ina3221_ch_t ina3221_get_warn_alert_limit(const ina3221_t *dev, ina3221_ch_t ch,
                                          int32_t out_uv[INA3221_NUM_CH])
{
    uint16_t reg_val;
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        out_uv[i] = 0;
        if (ch & (1U << i)) {
            if (_read_reg(dev, INA3221_REG_CH_WARN_ALERT_LIMIT(i), &reg_val)) {
                ch &= ~(1U << i);
            }
            else {
                out_uv[i] = reg_val_to_shunt_voltage_uv(reg_val);
            }
        }
    }
    return ch;
}

int ina3221_set_shunt_voltage_sum_alert_limit(const ina3221_t *dev,
                                              int32_t in_uv)
{
    if (in_uv < INA3221_MIN_SHUNT_SUM_UV || in_uv > INA3221_MAX_SHUNT_SUM_UV) {
        return -ERANGE;
    }
    int16_t reg_val = sum_shunt_voltage_uv_to_reg_val(in_uv);
    int status = _write_reg(dev, INA3221_REG_SHUNT_VOLTAGE_SUM_LIMIT, reg_val);
    if (status < 0) {
        return status;
    }
    return 0;
}

int ina3221_get_shunt_voltage_sum_alert_limit(const ina3221_t *dev,
                                              int32_t *out_uv)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_SHUNT_VOLTAGE_SUM_LIMIT, &reg_val);
    if (status < 0) {
        return status;
    }
    *out_uv = sum_reg_val_to_shunt_voltage_uv(reg_val);
    return 0;
}

int ina3221_set_power_valid_upper_limit(const ina3221_t *dev, int32_t in_mv)
{
    if (in_mv < INA3221_MIN_BUS_MV || in_mv > INA3221_MAX_BUS_MV) {
        return -ERANGE;
    }
    int16_t reg_val = bus_voltage_mv_to_reg_val(in_mv);
    int status = _write_reg(dev, INA3221_REG_PV_UPPER_LIMIT, reg_val);
    if (status < 0) {
        return status;
    }
    return 0;
}

int ina3221_get_power_valid_upper_limit(const ina3221_t *dev, int32_t *out_mv)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_PV_UPPER_LIMIT, &reg_val);
    if (status < 0) {
        return status;
    }
    *out_mv = reg_val_to_bus_voltage_mv(reg_val);
    return 0;
}

int ina3221_set_power_valid_lower_limit(const ina3221_t *dev, int32_t in_mv)
{
    if (in_mv < INA3221_MIN_BUS_MV || in_mv > INA3221_MAX_BUS_MV) {
        return -ERANGE;
    }
    int16_t reg_val = bus_voltage_mv_to_reg_val(in_mv);
    int status = _write_reg(dev, INA3221_REG_PV_LOWER_LIMIT, reg_val);
    if (status < 0) {
        return status;
    }
    return 0;
}

int ina3221_get_power_valid_lower_limit(const ina3221_t *dev, int32_t *out_mv)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_PV_LOWER_LIMIT, &reg_val);
    if (status < 0) {
        return status;
    }
    *out_mv = reg_val_to_bus_voltage_mv(reg_val);
    return 0;
}

int ina3221_read_flags(const ina3221_t *dev, uint16_t *flags)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &reg_val);
    if (status < 0) {
        return status;
    }
    *flags = reg_val & INA3221_FLAGS_MASK;
    return 0;
}

int ina3221_read_shunt_sum_uv(const ina3221_t *dev, int32_t *out_uv,
                              uint16_t *flags)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_SHUNT_VOLTAGE_SUM, &reg_val);
    if (status < 0) {
        return status;
    }
    *out_uv = sum_reg_val_to_shunt_voltage_uv(reg_val);
    if (flags) {
        *flags = 0;
        _read_reg(dev, INA3221_REG_MASK_ENABLE, flags);
    }
    return 0;
}

ina3221_ch_t ina3221_read_shunt_uv(const ina3221_t *dev,
                                   int32_t out_uv[INA3221_NUM_CH],
                                   uint16_t *flags)
{
    uint16_t reg_val;
    ina3221_ch_t ch = ina3221_config_get_enabled_channels(dev->params.config);
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        out_uv[i] = 0;
        if (ch & (1U << i)) {
            if (_read_reg(dev, INA3221_REG_CH_SHUNT_VOLTAGE(i), &reg_val)) {
                ch &= ~(1U << i);
            }
            else {
                out_uv[i] = reg_val_to_shunt_voltage_uv(reg_val);
            }
        }
    }
    if (flags) {
        *flags = 0;
        _read_reg(dev, INA3221_REG_MASK_ENABLE, flags);
    }
    return ch;
}

ina3221_ch_t ina3221_read_bus_mv(const ina3221_t *dev,
                                 int16_t out_mv[INA3221_NUM_CH],
                                 uint16_t *flags)
{
    uint16_t reg_val;
    ina3221_ch_t ch = ina3221_config_get_enabled_channels(dev->params.config);
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        out_mv[i] = 0;
        if (ch & (1U << i)) {
            if (_read_reg(dev, INA3221_REG_CH_BUS_VOLTAGE(i), &reg_val)) {
                ch &= ~(1U << i);
            }
            else {
                out_mv[i] = reg_val_to_bus_voltage_mv(reg_val);
            }
        }
    }
    if (flags) {
        *flags = 0;
        _read_reg(dev, INA3221_REG_MASK_ENABLE, flags);
    }
    return ch;
}

void ina3221_calculate_current_ua(ina3221_ch_t ch,
                                  const uint16_t in_mohm[INA3221_NUM_CH],
                                  const int32_t in_uv[INA3221_NUM_CH],
                                  int32_t out_ua[INA3221_NUM_CH])
{
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1U << i)) {
            out_ua[i] = in_uv[i] * 1000 / in_mohm[i];
        }
        else {
            out_ua[i] = 0;
        }
    }
}

void ina3221_calculate_power_uw(ina3221_ch_t ch,
                                const int16_t in_mv[INA3221_NUM_CH],
                                const int32_t in_ua[INA3221_NUM_CH],
                                int32_t out_uw[INA3221_NUM_CH])
{
    for (int i = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1U << i)) {
            /* max 26V bus voltage */
            /* (2^31)-1 resolution; 2.147483647 Watt in Nanowatt resolution */
            /* 2.147483647 / 26000 = 82595.525 */
            if (in_ua[i] < (82596 - 500)) {
                out_uw[i] = (in_ua[i] * in_mv[i] + 500) / 1000;
            }
            else {
                out_uw[i] = (in_ua[i] + 500) / 1000 * in_mv[i];
            }
        }
        else {
            out_uw[i] = 0;
        }
    }
}
