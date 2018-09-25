/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ccs811
 * @brief       Internal addresses, registers, constants for the CCS811 sensor
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the CCS811 sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CCS811_CONSTANTS_H
#define CCS811_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    CCS811 I2C address
 * @{
 */
#define CCS811_ADDR                      (0x5A) /* 0x5B when i2c addr is high */
/** @} */

/**
 * @name    CCS811 registers
 * @{
 */
#define CCS811_REG_STATUS                (0x00)
#define CCS811_REG_MEAS_MODE             (0x01)
#define CCS811_REG_ALG_RESULT_DATA       (0x02)
#define CCS811_REG_RAW_DATA              (0x03)
#define CCS811_REG_ENV_DATA              (0x05)
#define CCS811_REG_THRESHOLDS            (0x10)
#define CCS811_REG_BASELINE              (0x11)
#define CCS811_REG_HW_ID                 (0x20)
#define CCS811_REG_HW_VERSION            (0x21)
#define CCS811_REG_FW_BOOT_VERSION       (0x23)
#define CCS811_REG_FW_APP_VERSION        (0x24)
#define CCS811_REG_ERROR_ID              (0xE0)
#define CCS811_REG_APP_START             (0xF4)
#define CCS811_REG_SW_RESET              (0xFF)
/** @} */

/**
 * @name    CCS811 constants
 * @{
 */
#define CCS811_HW_ID                     (0x81)
#define CCS811_HW_VERSION                (1 << 4)
/** @} */

/**
 * @name    status register constants
 * @{
 */
#define CCS811_STATUS_REG_ERROR         (0)
#define CCS811_STATUS_REG_DATA_READY    (1 << 3)
#define CCS811_STATUS_REG_APP_VALID     (1 << 4)
#define CCS811_STATUS_REG_FW_MODE       (1 << 7)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CCS811_CONSTANTS_H */
/** @} */
