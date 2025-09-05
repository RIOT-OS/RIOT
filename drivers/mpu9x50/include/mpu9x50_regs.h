/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mpu9x50
 * @{
 *
 * @file
 * @brief       Register and bit definitions for the MPU-9X50 (MPU9150 and MPU9250) 9-Axis Motion Sensor
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    MPU-9X50 register definitions
 * @{
 */
#define MPU9X50_YG_OFFS_TC_REG          (0x01)
#define MPU9X50_RATE_DIV_REG            (0x19)
#define MPU9X50_LPF_REG                 (0x1A)
#define MPU9X50_GYRO_CFG_REG            (0x1B)
#define MPU9X50_ACCEL_CFG_REG           (0x1C)
#define MPU9X50_FIFO_EN_REG             (0x23)
#define MPU9X50_I2C_MST_REG             (0x24)
#define MPU9X50_SLAVE0_ADDR_REG         (0x25)
#define MPU9X50_SLAVE0_REG_REG          (0x26)
#define MPU9X50_SLAVE0_CTRL_REG         (0x27)
#define MPU9X50_SLAVE1_ADDR_REG         (0x28)
#define MPU9X50_SLAVE1_REG_REG          (0x29)
#define MPU9X50_SLAVE1_CTRL_REG         (0x2A)
#define MPU9X50_SLAVE4_CTRL_REG         (0x34)
#define MPU9X50_INT_PIN_CFG_REG         (0x37)
#define MPU9X50_INT_ENABLE_REG          (0x38)
#define MPU9X50_DMP_INT_STATUS          (0x39)
#define MPU9X50_INT_STATUS              (0x3A)
#define MPU9X50_ACCEL_START_REG         (0x3B)
#define MPU9X50_TEMP_START_REG          (0x41)
#define MPU9X50_GYRO_START_REG          (0x43)
#define MPU9X50_EXT_SENS_DATA_START_REG (0x49)
#define MPU9X50_COMPASS_DATA_START_REG  (0x4A)
#define MPU9X50_SLAVE0_DATA_OUT_REG     (0x63)
#define MPU9X50_SLAVE1_DATA_OUT_REG     (0x64)
#define MPU9X50_SLAVE2_DATA_OUT_REG     (0x65)
#define MPU9X50_SLAVE3_DATA_OUT_REG     (0x66)
#define MPU9X50_I2C_DELAY_CTRL_REG      (0x67)
#define MPU9X50_USER_CTRL_REG           (0x6A)
#define MPU9X50_PWR_MGMT_1_REG          (0x6B)
#define MPU9X50_PWR_MGMT_2_REG          (0x6C)
#define MPU9X50_FIFO_COUNT_START_REG    (0x72)
#define MPU9X50_FIFO_RW_REG             (0x74)
#define MPU9X50_WHO_AM_I_REG            (0x75)
/** @} */

 /**
  * @name    Compass register definitions
  * @{
  */
#define COMPASS_WHOAMI_REG              (0x00)
#define COMPASS_ST1_REG                 (0x02)
#define COMPASS_DATA_START_REG          (0x03)
#define COMPASS_ST2_REG                 (0x09)
#define COMPASS_CNTL_REG                (0x0A)
#define COMPASS_ASTC_REG                (0x0C)
#define COMPASS_ASAX_REG                (0x10)
#define COMPASS_ASAY_REG                (0x11)
#define COMPASS_ASAZ_REG                (0x12)
/** @} */

/**
 * @name    MPU9X50 bitfield definitions
 * @{
 */
#define BIT_SLV0_DELAY_EN               (0x01)
#define BIT_SLV1_DELAY_EN               (0x02)
#define BIT_I2C_BYPASS_EN               (0x02)
#define BIT_I2C_MST_EN                  (0x20)
#define BIT_PWR_MGMT1_SLEEP             (0x40)
#define BIT_WAIT_FOR_ES                 (0x40)
#define BIT_I2C_MST_VDDIO               (0x80)
#define BIT_SLAVE_RW                    (0x80)
#define BIT_SLAVE_EN                    (0x80)
#define BIT_DMP_EN                      (0x80)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
