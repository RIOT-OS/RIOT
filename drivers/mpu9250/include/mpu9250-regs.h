/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright 2018 Ville Hiltunen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9250
 * @{
 *
 * @file
 * @brief       Register and bit definitions for the MPU-9250 9-Axis Motion Sensor
 *
 * @note        Originally for MPU-9150, repurposed for MPU-9250
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>,
 *              modified by Ville Hiltunen <hiltunenvillej@gmail.com>
 */

#ifndef MPU9250_REGS_H
#define MPU9250_REGS_H


#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    MPU-9250 register definitions
 * @{
 */
//#define MPU9250_YG_OFFS_TC_REG          (0x01) What is this.
#define MPU9250_RATE_DIV_REG            (0x19)
#define MPU9250_CONFIG                  (0x1A)
#define MPU9250_GYRO_CFG_REG            (0x1B)
#define MPU9250_ACCEL_CFG_REG           (0x1C)
#define MPU9250_ACCEL_CFG_REG2          (0x1D)
#define MPU9250_FIFO_EN_REG             (0x23)
#define MPU9250_I2C_MST_REG             (0x24)
#define MPU9250_SLAVE0_ADDR_REG         (0x25)
#define MPU9250_SLAVE0_REG_REG          (0x26)
#define MPU9250_SLAVE0_CTRL_REG         (0x27)
#define MPU9250_SLAVE1_ADDR_REG         (0x28)
#define MPU9250_SLAVE1_REG_REG          (0x29)
#define MPU9250_SLAVE1_CTRL_REG         (0x2A)
#define MPU9250_SLAVE4_CTRL_REG         (0x34)
#define MPU9250_INT_PIN_CFG_REG         (0x37)
#define MPU9250_INT_ENABLE_REG          (0x38)
//#define MPU9250_DMP_INT_STATUS          (0x39) This doesn't seem to exist?
#define MPU9250_INT_STATUS              (0x3A)
#define MPU9250_ACCEL_START_REG         (0x3B)
#define MPU9250_TEMP_START_REG          (0x41)
#define MPU9250_GYRO_START_REG          (0x43)
#define MPU9250_EXT_SENS_DATA_START_REG (0x49)
#define MPU9250_COMPASS_DATA_START_REG  (0x4A)
#define MPU9250_SLAVE0_DATA_OUT_REG     (0x63)
#define MPU9250_SLAVE1_DATA_OUT_REG     (0x64)
#define MPU9250_SLAVE2_DATA_OUT_REG     (0x65)
#define MPU9250_SLAVE3_DATA_OUT_REG     (0x66)
#define MPU9250_I2C_DELAY_CTRL_REG      (0x67)
#define MPU9250_USER_CTRL_REG           (0x6A)
#define MPU9250_PWR_MGMT_1_REG          (0x6B)
#define MPU9250_PWR_MGMT_2_REG          (0x6C)
#define MPU9250_FIFO_COUNT_START_REG    (0x72)
#define MPU9250_FIFO_RW_REG             (0x74)
#define MPU9250_WHO_AM_I_REG            (0x75)
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
 * @name    MPU9250 bitfield definitions
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

#endif /* MPU9250_REGS_H */
/** @} */
