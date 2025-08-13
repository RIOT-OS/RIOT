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
 * @brief       Register definitions for Texas Instruments INA3221
 *              three-channel, high-side current and bus voltage
 *              monitor
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief INA3221 internal register addresses
 */
typedef enum ina3221_reg {
    INA3221_REG_CONFIGURATION           = 0x00,     /**< Configuration register                         (R/W) */
    INA3221_REG_CH1_SHUNT_VOLTAGE       = 0x01,     /**< Channel 1 shunt voltage register               (R)   */
    INA3221_REG_CH1_BUS_VOLTAGE         = 0x02,     /**< Channel 1 bus voltage register                 (R)   */
    INA3221_REG_CH2_SHUNT_VOLTAGE       = 0x03,     /**< Channel 2 shunt voltage register               (R)   */
    INA3221_REG_CH2_BUS_VOLTAGE         = 0x04,     /**< Channel 2 bus voltage register                 (R)   */
    INA3221_REG_CH3_SHUNT_VOLTAGE       = 0x05,     /**< Channel 3 shunt voltage register               (R)   */
    INA3221_REG_CH3_BUS_VOLTAGE         = 0x06,     /**< Channel 3 bus voltage register                 (R)   */
    INA3221_REG_CH1_CRIT_ALERT_LIMIT    = 0x07,     /**< Channel 1 voltage thresholt for critical alert (R/W) */
    INA3221_REG_CH1_WARN_ALERT_LIMIT    = 0x08,     /**< Channel 1 voltage thresholt for warning alert  (R/W) */
    INA3221_REG_CH2_CRIT_ALERT_LIMIT    = 0x09,     /**< Channel 2 voltage thresholt for critical alert (R/W) */
    INA3221_REG_CH2_WARN_ALERT_LIMIT    = 0x0A,     /**< Channel 2 voltage thresholt for warning alert  (R/W) */
    INA3221_REG_CH3_CRIT_ALERT_LIMIT    = 0x0B,     /**< Channel 3 voltage thresholt for critical alert (R/W) */
    INA3221_REG_CH3_WARN_ALERT_LIMIT    = 0x0C,     /**< Channel 3 voltage thresholt for warning alert  (R/W) */
    INA3221_REG_SHUNT_VOLTAGE_SUM       = 0x0D,     /**< Shunt voltage sum register over all channels   (R)   */
    INA3221_REG_SHUNT_VOLTAGE_SUM_LIMIT = 0x0E,     /**< Shunt voltage sum threshold for critical alert (R/W) */
    INA3221_REG_MASK_ENABLE             = 0x0F,     /**< Channel enable and status register             (R/W) */
    INA3221_REG_PV_UPPER_LIMIT          = 0x10,     /**< Pover valid upper limit register               (R/W) */
    INA3221_REG_PV_LOWER_LIMIT          = 0x11,     /**< Power valid lower limit register               (R/W) */
    INA3221_REG_MANUFACTURER_ID         = 0xFE,     /**< Manufacturer ID (=0x5449)                      (R)   */
    INA3221_REG_DIE_ID                  = 0xFF      /**< Die ID (=0x3220)                               (R)   */
} ina3221_reg_t;

#ifdef __cplusplus
}
#endif

/** @} */
