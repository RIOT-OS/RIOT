/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ina3221
 * @{
 *
 * @file
 * @brief       Internal definitions for Texas Instruments INA3221
 *              three-channel, high-side current and bus voltage
 *              monitor
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#define INA3221_REG_ADDR_LEN               (1)                  /**< Register address length */
#define INA3221_REG_LEN                    (2)                  /**< Register length */

#define INA3221_SHUNT_VOLTAGE_PRECISION_UV (40)                 /**< Shunt voltage measurement precision */
#define INA3221_BUS_VOLTAGE_PRECISION_MV   (8)                  /**< Bus voltage measurement precision */

#define INA3221_MANUFACTURER_ID            (0x5449)             /**< INA3221 manufacturer ID */
#define INA3221_DIE_ID                     (0x3220)             /**< INA3221 DIE ID */

/**
 * @name INA3221 Limits
 * @{
 */
#define INA3221_MAX_SHUNT_REG_VAL          ((int16_t)(0x7FF8))  /**< 0111 1111 1111 1000 */
#define INA3221_MIN_SHUNT_REG_VAL          ((int16_t)(0x8000))  /**< 1000 0000 0000 0000 */
#define INA3221_MAX_BUS_REG_VAL            ((int16_t)(0x7FF8))  /**< 0111 1111 1111 1000 */
#define INA3221_MIN_BUS_REG_VAL            ((int16_t)(0x8000))  /**< 1000 0000 0000 0000 */
#define INA3221_MAX_SHUNT_SUM_REG_VAL      ((int16_t)(0x7FFE))  /**< 0111 1111 1111 1110 */
#define INA3221_MIN_SHUNT_SUM_REG_VAL      ((int16_t)(0x8000))  /**< 1000 0000 0000 0000 */
#define INA3221_MAX_SHUNT_UV               (163800)             /**< Max. measurable shunt voltage value */
#define INA3221_MIN_SHUNT_UV               (-163840)            /**< Min. measurable shunt voltage value */
#define INA3221_MAX_BUS_MV                 (26000)              /**< Max. measurable bus voltage value */
#define INA3221_MIN_BUS_MV                 (0)                  /**< Min. measurable bus voltage value */
#define INA3221_MAX_SHUNT_SUM_UV           (655320)             /**< Max. measurable shunt sum voltage value */
#define INA3221_MIN_SHUNT_SUM_UV           (-655360)            /**< Min. measurable shunt sum voltage value */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
