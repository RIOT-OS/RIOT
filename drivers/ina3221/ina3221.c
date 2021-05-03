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
#include "ina3221_params.h"
#include "ina3221_regs.h"
#include "ina3221.h"
#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct {
    uint8_t chi_reg_shunt;
    uint8_t chi_reg_bus;
    uint8_t chi_reg_crit_alert_limit;
    uint8_t chi_reg_warn_alert_limit;
} ina3221_channel_info_t;

static ina3221_channel_info_t _chi[INA3221_NUM_CH] = {
    {
        .chi_reg_shunt = INA3221_REG_CH1_SHUNT_VOLTAGE,
        .chi_reg_bus = INA3221_REG_CH1_BUS_VOLTAGE,
        .chi_reg_crit_alert_limit = INA3221_REG_CH1_CRIT_ALERT_LIMIT,
        .chi_reg_warn_alert_limit = INA3221_REG_CH1_WARN_ALERT_LIMIT
    },
    {
        .chi_reg_shunt = INA3221_REG_CH2_SHUNT_VOLTAGE,
        .chi_reg_bus = INA3221_REG_CH2_BUS_VOLTAGE,
        .chi_reg_crit_alert_limit = INA3221_REG_CH2_CRIT_ALERT_LIMIT,
        .chi_reg_warn_alert_limit = INA3221_REG_CH2_WARN_ALERT_LIMIT
    },
    {
        .chi_reg_shunt = INA3221_REG_CH3_SHUNT_VOLTAGE,
        .chi_reg_bus = INA3221_REG_CH3_BUS_VOLTAGE,
        .chi_reg_crit_alert_limit = INA3221_REG_CH3_CRIT_ALERT_LIMIT,
        .chi_reg_warn_alert_limit = INA3221_REG_CH3_WARN_ALERT_LIMIT
    }
};

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
 * @return      -INA3221_I2C_ERROR, if i2c bus acquirement failed
 * @return      @see i2c_read_regs
 */
static int _read_reg(const ina3221_t *dev, uint8_t reg, uint16_t *out)
{
    if (i2c_acquire(dev->params.i2c)) {
        return -INA3221_I2C_ERROR;
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
 * @return      -INA3221_I2C_ERROR, if i2c bus acquirement failed
 * @return      @see i2c_write_regs
 */
static int _write_reg(const ina3221_t *dev, uint8_t reg, uint16_t in)
{
    in = htons(in);
    if (i2c_acquire(dev->params.i2c)) {
        return -INA3221_I2C_ERROR;
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
        return -INA3221_RESET_FAILED;
    }
    dev->params.config = INA3221_DEFCONFIG;
    return INA3221_OK;
}

int ina3221_init(ina3221_t *dev, const ina3221_params_t *params)
{
    int status;

    if (!dev || !params) {
        return -EINVAL;
    }
    dev->params = *params;
    uint16_t id;
    status = _read_reg(dev, INA3221_REG_MANUFACTURER_ID, &id);
    if (status < 0) {
        return status;
    }
    if (id != INA3221_MANUFACTURER_ID) {
        return -INA3221_BAD_MANUF_ID;
    }
    status = _read_reg(dev, INA3221_REG_DIE_ID, &id);
    if (status < 0) {
        return status;
    }
    if (id != INA3221_DIE_ID) {
        return -INA3221_BAD_DIE_ID;
    }
    if (ina3221_reset(dev) != INA3221_OK) {
        return -INA3221_RESET_FAILED;
    }
    if (_ina3221_set_config(dev, params->config) != INA3221_OK) {
        return -INA3221_CONFIG_FAILED;
    }
    uint16_t cfg;
    if (_ina3221_get_config(dev, &cfg) != INA3221_OK || cfg != params->config) {
        return -INA3221_CONFIG_FAILED;
    }
#if defined(MODULE_INA3221_ALERTS) || defined(DOXYGEN)
    memset(dev->alert_callbacks, 0, sizeof(dev->alert_callbacks));
    memset(dev->alert_callback_arguments, 0,
           sizeof(dev->alert_callback_arguments));
#endif
    return INA3221_OK;
}

int _ina3221_set_config(ina3221_t *dev, uint16_t cfg)
{
    cfg &= ~INA3221_RESET; /* prevent accidental reset */
    int status = _write_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    dev->params.config = cfg;
    return INA3221_OK;
}

int _ina3221_get_config(const ina3221_t *dev, uint16_t *cfg)
{
    *cfg = dev->params.config;
    *cfg &= ~INA3221_RESET; /* clear reset flag */
    return INA3221_OK;
}

int _ina3221_set_enable_channel(ina3221_t *dev, uint16_t ech)
{
    uint16_t cfg;
    int status = _read_reg(dev, INA3221_REG_CONFIGURATION, &cfg);

    if (status < 0) {
        return status;
    }
    cfg &= ~INA3221_ENABLE_CH_MASK;
    cfg |= (ech & INA3221_ENABLE_CH_MASK);
    status = _write_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    dev->params.config = cfg;
    return INA3221_OK;
}

int _ina3221_get_enable_channel(const ina3221_t *dev, uint16_t *ech)
{
    *ech = dev->params.config & INA3221_ENABLE_CH_MASK;
    return ((*ech & INA3221_ENABLE_CH1) ? 1 : 0) +
           ((*ech & INA3221_ENABLE_CH2) ? 1 : 0) +
           ((*ech & INA3221_ENABLE_CH3) ? 1 : 0);
}

int ina3221_set_num_samples(ina3221_t *dev, ina3221_num_samples_t ns)
{
    uint16_t cfg;
    int status = _read_reg(dev, INA3221_REG_CONFIGURATION, &cfg);

    if (status < 0) {
        return status;
    }
    cfg &= ~INA3221_NUM_SAMPLES_MASK;
    cfg |= (ns & INA3221_NUM_SAMPLES_MASK);
    status = _write_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    dev->params.config = cfg;
    return INA3221_OK;
}

int ina3221_get_num_samples(const ina3221_t *dev, ina3221_num_samples_t *ns)
{
    *ns = dev->params.config & INA3221_NUM_SAMPLES_MASK;
    return INA3221_OK;
}

int ina3221_set_conv_time_bus_adc(ina3221_t *dev,
                                  ina3221_conv_time_bus_adc_t ctb)
{
    uint16_t cfg;
    int status = _read_reg(dev, INA3221_REG_CONFIGURATION, &cfg);

    if (status < 0) {
        return status;
    }
    cfg &= ~INA3221_CONV_TIME_BADC_MASK;
    cfg |= (ctb & INA3221_CONV_TIME_BADC_MASK);
    status = _write_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    dev->params.config = cfg;
    return INA3221_OK;
}

int ina3221_get_conv_time_bus_adc(const ina3221_t *dev,
                                  ina3221_conv_time_bus_adc_t *ctb)
{
    *ctb = dev->params.config & INA3221_CONV_TIME_BADC_MASK;
    return INA3221_OK;
}

int ina3221_set_conv_time_shunt_adc(ina3221_t *dev,
                                    ina3221_conv_time_shunt_adc_t ctb)
{
    uint16_t cfg;
    int status = _read_reg(dev, INA3221_REG_CONFIGURATION, &cfg);

    if (status < 0) {
        return status;
    }
    cfg &= ~INA3221_CONV_TIME_SADC_MASK;
    cfg |= (ctb & INA3221_CONV_TIME_SADC_MASK);
    status = _write_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    dev->params.config = cfg;
    return INA3221_OK;
}

int ina3221_get_conv_time_shunt_adc(const ina3221_t *dev,
                                    ina3221_conv_time_shunt_adc_t *ctb)
{
    *ctb = dev->params.config & INA3221_CONV_TIME_SADC_MASK;
    return INA3221_OK;
}

int ina3221_set_mode(ina3221_t *dev, ina3221_mode_t mode)
{
    uint16_t cfg;
    int status = _read_reg(dev, INA3221_REG_CONFIGURATION, &cfg);

    if (status < 0) {
        return status;
    }
    cfg &= ~INA3221_MODE_MASK;
    cfg |= (mode & INA3221_MODE_MASK);
    status = _write_reg(dev, INA3221_REG_CONFIGURATION, cfg);
    if (status < 0) {
        return status;
    }
    dev->params.config = cfg;
    return INA3221_OK;
}

int ina3221_get_mode(const ina3221_t *dev, ina3221_mode_t *mode)
{
    *mode = dev->params.config & INA3221_MODE_MASK;
    return INA3221_OK;
}

int _ina3221_set_enable_sum_channel(const ina3221_t *dev,
                                    uint16_t esch)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);

    if (status < 0) {
        return status;
    }
    mask_en &= ~INA3221_ENABLE_SUM_CH_MASK;
    mask_en |= (esch & INA3221_ENABLE_SUM_CH_MASK);
    status = _write_reg(dev, INA3221_REG_MASK_ENABLE, mask_en);
    if (status < 0) {
        return status;
    }
    return INA3221_OK;
}

int _ina3221_get_enable_sum_channel(const ina3221_t *dev,
                                    uint16_t *esch)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);

    if (status < 0) {
        return status;
    }
    *esch = mask_en & (INA3221_ENABLE_SUM_CH_MASK);
    return ((*esch & INA3221_ENABLE_SUM_CH1) ? 1 : 0) +
           ((*esch & INA3221_ENABLE_SUM_CH2) ? 1 : 0) +
           ((*esch & INA3221_ENABLE_SUM_CH3) ? 1 : 0);
}

int ina3221_set_latch(const ina3221_t *dev, ina3221_enable_latch_t latch)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);

    if (status < 0) {
        return status;
    }
    mask_en &= ~INA3221_ENABLE_LATCH_MASK;
    mask_en |= (latch & INA3221_ENABLE_LATCH_MASK);
    status = _write_reg(dev, INA3221_REG_MASK_ENABLE, mask_en);
    if (status < 0) {
        return status;
    }
    return INA3221_OK;
}

int ina3221_get_latch(const ina3221_t *dev, ina3221_enable_latch_t *latch)
{
    uint16_t mask_en;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &mask_en);

    if (status < 0) {
        return status;
    }
    *latch = mask_en & INA3221_ENABLE_LATCH_MASK;
    return INA3221_OK;
}

int ina3221_set_crit_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t in_uv)
{
    if (in_uv < INA3221_MIN_SHUNT_UV || in_uv > INA3221_MAX_SHUNT_UV) {
        return -ERANGE;
    }
    int16_t reg_val = shunt_voltage_uv_to_reg_val(in_uv);
    int status = INA3221_OK;
    int i, j;
    for (i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            status = _write_reg(dev, _chi[i].chi_reg_crit_alert_limit, reg_val);
            if (status < 0) {
                break;
            }
            j++;
        }
    }
    return j ? j : status;
}

int ina3221_get_crit_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t *out_uv)
{
    uint16_t reg_val;
    int status = INA3221_OK;
    int i, j;

    for (i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            status = _read_reg(dev, _chi[i].chi_reg_crit_alert_limit, &reg_val);
            if (status < 0) {
                break;
            }
            out_uv[j++] = reg_val_to_shunt_voltage_uv(reg_val);
        }
    }
    return j ? j : status;
}

int ina3221_set_warn_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t in_uv)
{
    if (in_uv < INA3221_MIN_SHUNT_UV || in_uv > INA3221_MAX_SHUNT_UV) {
        return -ERANGE;
    }
    int16_t reg_val = shunt_voltage_uv_to_reg_val(in_uv);
    int status = INA3221_OK;
    int i, j;
    for (i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            status = _write_reg(dev, _chi[i].chi_reg_warn_alert_limit, reg_val);
            if (status < 0) {
                break;
            }
            j++;
        }
    }
    return j ? j : status;
}

int ina3221_get_warn_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t *out_uv)
{
    uint16_t reg_val;
    int status = INA3221_OK;
    int i, j;

    for (i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            status = _read_reg(dev, _chi[i].chi_reg_warn_alert_limit, &reg_val);
            if (status < 0) {
                break;
            }
            out_uv[j++] = reg_val_to_shunt_voltage_uv(reg_val);
        }
    }
    return j ? j : status;
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
    return INA3221_OK;
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
    return INA3221_OK;
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
    return INA3221_OK;
}

int ina3221_get_power_valid_upper_limit(const ina3221_t *dev, int32_t *out_mv)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_PV_UPPER_LIMIT, &reg_val);

    if (status < 0) {
        return status;
    }
    *out_mv = reg_val_to_bus_voltage_mv(reg_val);
    return INA3221_OK;
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
    return INA3221_OK;
}

int ina3221_get_power_valid_lower_limit(const ina3221_t *dev, int32_t *out_mv)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_PV_LOWER_LIMIT, &reg_val);

    if (status < 0) {
        return status;
    }
    *out_mv = reg_val_to_bus_voltage_mv(reg_val);
    return INA3221_OK;
}

int ina3221_read_flags(const ina3221_t *dev, uint16_t *flags)
{
    uint16_t reg_val;
    int status = _read_reg(dev, INA3221_REG_MASK_ENABLE, &reg_val);

    if (status < 0) {
        return status;
    }
    *flags = reg_val & INA3221_FLAGS_MASK;
    return INA3221_OK;
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
        status = _read_reg(dev, INA3221_REG_MASK_ENABLE, flags);
        if (status < 0) {
            *flags = 0;
            DEBUG("ina3221_read_shunt_sum_uv: Reading flags failed\n");
        }
        else {
            *flags &= INA3221_FLAGS_MASK;
        }
    }
    return INA3221_OK;
}

int ina3221_read_shunt_uv(const ina3221_t *dev, ina3221_channel_t ch,
                          int32_t *out_uv, uint16_t *flags)
{
    uint16_t reg_val;
    int status = INA3221_OK;
    int i, j;

    for (i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            status = _read_reg(dev, _chi[i].chi_reg_shunt, &reg_val);
            if (status < 0) {
                break;
            }
            out_uv[j++] = reg_val_to_shunt_voltage_uv(reg_val);
        }
    }
    if (j && flags) {
        status = _read_reg(dev, INA3221_REG_MASK_ENABLE, flags);
        if (status < 0) {
            *flags = 0;
            DEBUG("ina3221_read_shunt_uv: Reading flags failed\n");
        }
        else {
            *flags &= INA3221_FLAGS_MASK;
        }
    }
    return j ? j : status;
}

int ina3221_read_bus_mv(const ina3221_t *dev, ina3221_channel_t ch,
                        int16_t *out_mv, uint16_t *flags)
{
    uint16_t reg_val;
    int status = INA3221_OK;
    int i, j = 0;

    for (i = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            status = _read_reg(dev, _chi[i].chi_reg_bus, &reg_val);
            if (status < 0) {
                break;
            }
            out_mv[j++] = reg_val_to_bus_voltage_mv(reg_val);
        }
    }
    if (j && flags) {
        status = _read_reg(dev, INA3221_REG_MASK_ENABLE, flags);
        if (status < 0) {
            *flags = 0;
            DEBUG("ina3221_read_bus_mv: Reading flags failed\n");
        }
        else {
            *flags &= INA3221_FLAGS_MASK;
        }
    }
    return j ? j : status;
}

int ina3221_calculate_current_ua(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t *in_uv, int32_t *out_ua)
{
    int i, j = 0;

    for (i = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            out_ua[j] = in_uv[j] * 1000 / dev->params.rshunt_mohm[i];
            j++;
        }
    }
    return j;
}

int ina3221_calculate_power_uw(int16_t *in_mv, int32_t *in_ua, uint8_t num,
                               int32_t *out_uw)
{
    if (num > INA3221_NUM_CH) {
        return -ERANGE;
    }
    int i;
    for (i = 0; i < num; i++) {
        /* max 26V bus voltage */
        /* (2^31)-1 resolution; 2.147483647 Watt in Nanowatt resolutiona */
        /* 2.147483647 / 26000 = 82595.525 */
        if (in_ua[i] < (82596 - 500)) {
            out_uw[i] = (in_ua[i] * in_mv[i] + 500) / 1000;
        }
        else {
            out_uw[i] = (in_ua[i] + 500) / 1000 * in_mv[i];
        }
    }
    return i;
}

void ina3221_ch_align(ina3221_channel_t ch, const void *in_res, void *out_res,
                      size_t res_val_size)
{
    uint8_t *in = (uint8_t *)in_res;
    uint8_t tmp_out[INA3221_NUM_CH][res_val_size];
    int j = 0;

    for (int i = 0; i < INA3221_NUM_CH; i++) {
        if (ch & (1 << i)) {
            memcpy(&tmp_out[i][0], in + j * res_val_size, res_val_size);
            j++;
        }
        else {
            memset(&tmp_out[i][0], 0, res_val_size);
        }
    }
    memcpy(out_res, tmp_out, sizeof(tmp_out));
}
