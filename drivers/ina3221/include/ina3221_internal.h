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
 * @brief       Functions to convert register values of INA3221
 *              to actual voltage values and vice versa
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef INA3221_INTERNAL_H
#define INA3221_INTERNAL_H

#include <assert.h>
#include "ina3221_defines.h"
#include "ina3221.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert register value to shunt voltage in uV
 *
 * @param[in]   reg_val Register value
 *
 * @pre         reg_val must be in host byte order
 *
 * @return      Shunt voltage in uV
 */
static inline int32_t reg_val_to_shunt_voltage_uv(int16_t reg_val)
{
    assert(reg_val <= INA3221_MAX_SHUNT_REG_VAL);

    return reg_val / 8 * INA3221_SHUNT_VOLTAGE_PRECISION_UV;
}

/**
 * @brief Convert shunt voltage in uV to register value
 *
 * @param[in]   s_uv Shunt voltage in uV
 *
 * @post        Output register value is in host byte order
 *
 * @return      Register value
 */
static inline int16_t shunt_voltage_uv_to_reg_val(int32_t s_uv)
{
    assert(s_uv <= INA3221_MAX_SHUNT_UV);

    return s_uv / INA3221_SHUNT_VOLTAGE_PRECISION_UV * 8;
}

/**
 * @brief Convert register value to bus voltage in mV
 *
 * @param[in]   reg_val Register value
 *
 * @pre         reg_val must be in host byte order
 *
 * @return      Shunt voltage in mV
 */
static inline int16_t reg_val_to_bus_voltage_mv(int16_t reg_val)
{
    assert(reg_val <= INA3221_MAX_BUS_REG_VAL);

    return reg_val / 8 * INA3221_BUS_VOLTAGE_PRECISION_MV;
}

/**
 * @brief Convert bus voltage in mV to register value
 *
 * @param[in]   b_mv Bus voltage in mV
 *
 * @post        Output register value is in host byte order
 *
 * @return      Register value
 */
static inline int16_t bus_voltage_mv_to_reg_val(int16_t b_mv)
{
    assert(b_mv >= INA3221_MIN_BUS_MV);
    assert(b_mv <= INA3221_MAX_BUS_MV);

    return b_mv / INA3221_BUS_VOLTAGE_PRECISION_MV * 8;
}

/**
 * @brief Convert register value to shunt voltage sum value in mV
 *
 * @param[in]   sum_reg_val Register value
 *
 * @pre         sum_reg_val must be in host byte order
 *
 * @return      Shunt voltage sum value in mV
 */
static inline int32_t sum_reg_val_to_shunt_voltage_uv(int16_t sum_reg_val)
{
    assert(sum_reg_val <= INA3221_MAX_SHUNT_SUM_REG_VAL);

    return sum_reg_val / 2 * INA3221_SHUNT_VOLTAGE_PRECISION_UV;
}

/**
 * @brief Convert shunt voltage sum value in uV to register value
 *
 * @param[in]   sum_suv shunt voltage sum value in uV
 *
 * @post        Output register value is in host byte order
 *
 * @return      Register value
 */
static inline int16_t sum_shunt_voltage_uv_to_reg_val(int32_t sum_suv)
{
    assert(sum_suv <= INA3221_MAX_SHUNT_SUM_UV);

    return sum_suv / INA3221_SHUNT_VOLTAGE_PRECISION_UV * 2;
}

#ifdef __cplusplus
}
#endif

#endif /* INA3221_INTERNAL_H */
/** @} */
