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

static int read_bus_voltage(const void *dev, phydat_t *res)
{
    ina3221_ch_t ch = 0;
    int32_t values[INA3221_NUM_CH] = { 0 };
    ina3221_get_enable_channel(dev, &ch);
    if (ch) {
        int16_t voltage[INA3221_NUM_CH];
        ch &= ina3221_read_bus_mv(dev, voltage, NULL);
        res->scale = -3; /* mV to V */
        res->unit = UNIT_V;
        for (int i = 0; i < INA3221_NUM_CH; i++) {
            values[i] = voltage[i];
        }
    }
    phydat_fit(res, values, 3);
    return INA3221_NUM_CH;
}

static int read_current(const void *dev, phydat_t *res)
{
    ina3221_ch_t ch = 0;
    int32_t current[INA3221_NUM_CH] = { 0 };
    ina3221_get_enable_channel(dev, &ch);
    if (ch) {
        int32_t shunt_uv[INA3221_NUM_CH];
        ch &= ina3221_read_shunt_uv(dev, shunt_uv, NULL);
        ina3221_calculate_current_ua(ch, ((const ina3221_t *)dev)->params.rshunt_mohm,
                                     shunt_uv, current);
        res->scale = -6; /* uA to A */
        res->unit = UNIT_A;
    }
    phydat_fit(res, current, 3);
    return INA3221_NUM_CH;
}

static int read_power(const void *dev, phydat_t *res)
{
    ina3221_ch_t ch = 0;
    int32_t power[INA3221_NUM_CH] = { 0 };
    ina3221_get_enable_channel(dev, &ch);
    if (ch) {
        int32_t shunt_uv[INA3221_NUM_CH];
        int32_t current_ua[INA3221_NUM_CH];
        int16_t bus_mv[INA3221_NUM_CH];
        ch &= ina3221_read_shunt_uv(dev, shunt_uv, NULL);
        ch &= ina3221_read_bus_mv(dev, bus_mv, NULL);
        ina3221_calculate_current_ua(ch, ((const ina3221_t *)dev)->params.rshunt_mohm,
                                     shunt_uv, current_ua);
        ina3221_calculate_power_uw(ch, bus_mv, current_ua, power);
        res->scale = -6; /* uW to W */
        res->unit = UNIT_W;
    }
    phydat_fit(res, power, 3);
    return INA3221_NUM_CH;
}

static int read_shunt_voltage_sum(const void *dev, phydat_t *res)
{
    ina3221_ch_t ch = 0;
    int32_t shunt_voltage_sum = 0;
    ina3221_get_enable_channel(dev, &ch);
    if (ch) {
        ina3221_read_shunt_sum_uv(dev, &shunt_voltage_sum, NULL);
        res->scale = -6; /* uV to V */
        res->unit = UNIT_V;
    }
    phydat_fit(res, &shunt_voltage_sum, 1);
    return 1;
}

static int configure_channel(const void *dev, phydat_t *data)
{
    ina3221_ch_t ch = (data->val[0] ? INA3221_CH1 : 0) |
                      (data->val[1] ? INA3221_CH2 : 0) |
                      (data->val[2] ? INA3221_CH3 : 0);
    if (ina3221_set_enable_channel((ina3221_t *)dev, ch) != 0) {
        return -ECANCELED;
    }
    return INA3221_NUM_CH;
}

static int configure_channel_sum(const void *dev, phydat_t *data)
{
    ina3221_ch_t ch = (data->val[0] ? INA3221_CH1 : 0) |
                      (data->val[1] ? INA3221_CH2 : 0) |
                      (data->val[2] ? INA3221_CH3 : 0);
    if (ina3221_set_enable_sum_channel(dev, ch) != 0) {
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
