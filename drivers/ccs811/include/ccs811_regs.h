/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ccs811
 * @brief       Register definitions for the AMS CCS811 digital gas sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CCS811 register addresses
 * @{
 */
#define CCS811_REG_STATUS          (0x00)
#define CCS811_REG_MEAS_MODE       (0x01)
#define CCS811_REG_ALG_RESULT_DATA (0x02)
#define CCS811_REG_RAW_DATA        (0x03)
#define CCS811_REG_ENV_DATA        (0x05)
#define CCS811_REG_NTC             (0x06)
#define CCS811_REG_THRESHOLDS      (0x10)
#define CCS811_REG_BASELINE        (0x11)

#define CCS811_REG_HW_ID           (0x20)
#define CCS811_REG_HW_VER          (0x21)
#define CCS811_REG_FW_BOOT_VER     (0x23)
#define CCS811_REG_FW_APP_VER      (0x24)

#define CCS811_REG_ERROR_ID        (0xe0)

#define CCS811_REG_APP_ERASE       (0xf1)
#define CCS811_REG_APP_DATA        (0xf2)
#define CCS811_REG_APP_VERIFY      (0xf3)
#define CCS811_REG_APP_START       (0xf4)
#define CCS811_REG_SW_RESET        (0xff)
/** @} */

/**
 * @name Status register bits (#CCS811_REG_STATUS)
 * @{
 */
#define CCS811_STATUS_ERROR        (0x01)  /**< error occurred, details in CCS811_REG_ERROR */
#define CCS811_STATUS_DATA_RDY     (0x08)  /**< new data sample available in ALG_RESULT_DATA */
#define CCS811_STATUS_APP_VALID    (0x10)  /**< valid application firmware loaded */
#define CCS811_STATUS_FW_MODE      (0x80)  /**< firmware is in application mode */
/** @} */

/**
 * @name Error register bits (#CCS811_REG_ERROR_ID)
 * @{
 */
#define CCS811_ERR_WRITE_REG_INV   (0x01)  /**< invalid register address on write */
#define CCS811_ERR_READ_REG_INV    (0x02)  /**< invalid register address on read */
#define CCS811_ERR_MEASMODE_INV    (0x04)  /**< invalid requested measurement mode */
#define CCS811_ERR_MAX_RESISTANCE  (0x08)  /**< maximum sensor resistance exceeded  */
#define CCS811_ERR_HEATER_FAULT    (0x10)  /**< heater current not in range */
#define CCS811_ERR_HEATER_SUPPLY   (0x20)  /**< heater voltage not applied correctly */
/** @} */

/** CCS811 hardware ID */
#define CCS811_HW_ID               (0x81)

#ifdef __cplusplus
}
#endif

/** @} */
