/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp9808
 * @brief       Internal addresses, registers, constants for the MCP9808 sensor.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the MCP9808 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef MCP9808_CONSTANTS_H
#define MCP9808_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    MCP9808 I2C address
 * @{
 */
/**
 * @brief  Device address (7 bit address)
 *
 * Last 3 bits must match A2, A1 and A0 pins configuration (default is to have
 * all to 0).
*/
#define MCP9808_ADDR                    (0x18)
/** @} */

/**
 * @name    MCP9808 constants
 * @{
 */
#define MCP9808_DEVICE_ID               (0x04)
/** @} */

/**
 * @name    MCP9808 register addresses
 * @{
 */
#define MCP9808_REG_CONFIG              (0x01)
#define MCP9808_REG_T_UPPER             (0x02)
#define MCP9808_REG_T_LOWER             (0x03)
#define MCP9808_REG_T_CRIT              (0x04)
#define MCP9808_REG_T_AMBIENT           (0x05)
#define MCP9808_REG_MANUFACTURER_ID     (0x06)
#define MCP9808_REG_DEVICE_ID_REV       (0x07)
#define MCP9808_REG_RESOLUTION          (0x08)
/** @} */

/**
 * @name    MCP9808 config register bits
 * @{
 */
#define MCP9808_REG_CONFIG_MASK         (0x07FF)
#define MCP9808_REG_CONFIG_T_HYST       (0x0600)
#define MCP9808_REG_CONFIG_SHDN         (0x0100)
#define MCP9808_REG_CONFIG_CRIT_LOCK    (0x0080)
#define MCP9808_REG_CONFIG_WIN_LOCK     (0x0040)
#define MCP9808_REG_CONFIG_INT_CLEAR    (0x0020)
#define MCP9808_REG_CONFIG_ALERT_STAT   (0x0010)
#define MCP9808_REG_CONFIG_ALERT_CNT    (0x0008)
#define MCP9808_REG_CONFIG_ALERT_SEL    (0x0004)
#define MCP9808_REG_CONFIG_ALERT_POL    (0x0002)
#define MCP9808_REG_CONFIG_ALERT_MOD    (0x0001)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MCP9808_CONSTANTS_H */
/** @} */
