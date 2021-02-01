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
 * @brief       SAUL adaption for INA3221 device
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include "phydat.h"
#include "saul.h"
#include "ina3221.h"

#define ENABLE_DEBUG            0
#include "debug.h"

#define SAUL_INA3221_NO_VALUE   (0)

static int read_bus_voltage(const void *dev, phydat_t *res)
{
    ina3221_t *_dev = (ina3221_t *)dev;
    uint16_t ench = 0;
    int16_t voltage[INA3221_NUM_CH] = { 0 };
    int num_ch = _ina3221_get_enable_channel(_dev, &ench);

    if (!num_ch) {
        return -ECANCELED;
    }
    ina3221_channel_t ch = (((ench & INA3221_ENABLE_CH1) ? INA3221_CH1 : 0) |
                            ((ench & INA3221_ENABLE_CH2) ? INA3221_CH2 : 0) |
                            ((ench & INA3221_ENABLE_CH3) ? INA3221_CH3 : 0));
    ina3221_read_bus_mv(_dev, ch, voltage, NULL);
    res->scale = -3; /* mV to V*/
    res->unit = UNIT_V;
    for (int i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        res->val[i] = (ch & (1 << i))
                      ? voltage[j++] : SAUL_INA3221_NO_VALUE;
    }
    return INA3221_NUM_CH;
}

static int read_current(const void *dev, phydat_t *res)
{
    ina3221_t *_dev = (ina3221_t *)dev;
    uint16_t ench = 0;
    int32_t shunt_uv[INA3221_NUM_CH] = { 0 };
    int32_t current[INA3221_NUM_CH] = { 0 };
    int num_ch = _ina3221_get_enable_channel(_dev, &ench);

    if (!num_ch) {
        return -ECANCELED;
    }
    ina3221_channel_t ch = (((ench & INA3221_ENABLE_CH1) ? INA3221_CH1 : 0) |
                            ((ench & INA3221_ENABLE_CH2) ? INA3221_CH2 : 0) |
                            ((ench & INA3221_ENABLE_CH3) ? INA3221_CH3 : 0));
    ina3221_read_shunt_uv(_dev, ch, shunt_uv, NULL);
    ina3221_calculate_current_ua(_dev, ch, shunt_uv, current);
    res->scale = -6; /* uA to A*/
    res->unit = UNIT_A;
    for (int i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        res->val[i] = (ch & (1 << i))
                      ? (int16_t)current[j++] : SAUL_INA3221_NO_VALUE;
    }
    ina3221_ch_align(ch, current, current, sizeof(int32_t));
    phydat_fit(res, current, 3);
    return INA3221_NUM_CH;
}

static int read_power(const void *dev, phydat_t *res)
{
    ina3221_t *_dev = (ina3221_t *)dev;
    uint16_t ench = 0;
    int32_t shunt_uv[INA3221_NUM_CH] = { 0 };
    int32_t current_ua[INA3221_NUM_CH] = { 0 };
    int16_t bus_mv[INA3221_NUM_CH] = { 0 };
    int32_t power[INA3221_NUM_CH] = { 0 };
    int num_ch = _ina3221_get_enable_channel(_dev, &ench);

    if (!num_ch) {
        return -ECANCELED;
    }
    ina3221_channel_t ch = (((ench & INA3221_ENABLE_CH1) ? INA3221_CH1 : 0) |
                            ((ench & INA3221_ENABLE_CH2) ? INA3221_CH2 : 0) |
                            ((ench & INA3221_ENABLE_CH3) ? INA3221_CH3 : 0));
    int num_ch_shunt_uv = ina3221_read_shunt_uv(_dev, ch, shunt_uv, NULL);
    ina3221_calculate_current_ua(_dev, ch, shunt_uv, current_ua);
    ina3221_read_bus_mv(_dev, ch, bus_mv, NULL);
    ina3221_calculate_power_uw(bus_mv, current_ua, num_ch_shunt_uv, power);
    res->scale = -6; /* uW to W*/
    res->unit = UNIT_W;
    for (int i = 0, j = 0; i < INA3221_NUM_CH; i++) {
        res->val[i] = (ch & (1 << i))
                      ? (int16_t)power[j++] : SAUL_INA3221_NO_VALUE;
    }
    ina3221_ch_align(ch, power, power, sizeof(int32_t));
    phydat_fit(res, power, 3);
    return INA3221_NUM_CH;
}

static int read_shunt_voltage_sum(const void *dev, phydat_t *res)
{
    ina3221_t *_dev = (ina3221_t *)dev;
    uint16_t ench = 0;
    int32_t shunt_voltage_sum = SAUL_INA3221_NO_VALUE;
    int num_ch = _ina3221_get_enable_channel(_dev, &ench);

    if (!num_ch) {
        return -ECANCELED;
    }
    ina3221_read_shunt_sum_uv(_dev, &shunt_voltage_sum, NULL);
    res->scale = -6; /* uV to V*/
    res->unit = UNIT_V;
    res->val[0] = shunt_voltage_sum;
    phydat_fit(res, &shunt_voltage_sum, 1);
    return 1;
}

static int configure_channel(const void *dev, phydat_t *data)
{
    ina3221_t *_dev = (ina3221_t *)dev;
    uint16_t ench =
        ((data->val[0] & INA3221_CH1) ? INA3221_ENABLE_CH1 : 0) |
        ((data->val[0] & INA3221_CH2) ? INA3221_ENABLE_CH2 : 0) |
        ((data->val[0] & INA3221_CH3) ? INA3221_ENABLE_CH3 : 0);

    if (_ina3221_set_enable_channel(_dev, ench) != INA3221_OK) {
        return -ECANCELED;
    }
    return INA3221_NUM_CH;
}

static int configure_channel_sum(const void *dev, phydat_t *data)
{
    ina3221_t *_dev = (ina3221_t *)dev;
    uint16_t esch =
        ((data->val[0] & INA3221_CH1) ? INA3221_ENABLE_SUM_CH1 : 0) |
        ((data->val[0] & INA3221_CH2) ? INA3221_ENABLE_SUM_CH2 : 0) |
        ((data->val[0] & INA3221_CH3) ? INA3221_ENABLE_SUM_CH3 : 0);

    if (_ina3221_set_enable_sum_channel(_dev, esch) != INA3221_OK) {
        return -ECANCELED;
    }
    return INA3221_NUM_CH;
}

const saul_driver_t ina3221_saul_drivers[] = {
    {
        .read = read_bus_voltage,
        .write = configure_channel,
        .type = SAUL_SENSE_VOLTAGE
    },
    {
        .read = read_current,
        .write = configure_channel,
        .type = SAUL_SENSE_CURRENT
    },
    {
        .read = read_power,
        .write = configure_channel,
        .type = SAUL_SENSE_POWER
    },
    {
        .read = read_shunt_voltage_sum,
        .write = configure_channel_sum,
        .type = SAUL_SENSE_VOLTAGE
    }
};
