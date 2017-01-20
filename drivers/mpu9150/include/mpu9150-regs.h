/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9150
 * @{
 *
 * @file
 * @brief       Register and bit definitions for the MPU-9150 9-Axis Motion Sensor
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef MPU9150_REGS_H
#define MPU9150_REGS_H


#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name MPU-9150 register definitions
 * @{
 */
#define MPU9150_YG_OFFS_TC_REG          (0x01)
#define MPU9150_RATE_DIV_REG            (0x19)
#define MPU9150_LPF_REG                 (0x1A)
#define MPU9150_GYRO_CFG_REG            (0x1B)
#define MPU9150_ACCEL_CFG_REG           (0x1C)
#define MPU9150_FIFO_EN_REG             (0x23)
#define MPU9150_I2C_MST_REG             (0x24)
#define MPU9150_SLAVE0_ADDR_REG         (0x25)
#define MPU9150_SLAVE0_REG_REG          (0x26)
#define MPU9150_SLAVE0_CTRL_REG         (0x27)
#define MPU9150_SLAVE1_ADDR_REG         (0x28)
#define MPU9150_SLAVE1_REG_REG          (0x29)
#define MPU9150_SLAVE1_CTRL_REG         (0x2A)
#define MPU9150_SLAVE4_CTRL_REG         (0x34)
#define MPU9150_INT_PIN_CFG_REG         (0x37)
#define MPU9150_INT_ENABLE_REG          (0x38)
#define MPU9150_DMP_INT_STATUS          (0x39)
#define MPU9150_INT_STATUS              (0x3A)
#define MPU9150_ACCEL_START_REG         (0x3B)
#define MPU9150_TEMP_START_REG          (0x41)
#define MPU9150_GYRO_START_REG          (0x43)
#define MPU9150_EXT_SENS_DATA_START_REG (0x49)
#define MPU9150_COMPASS_DATA_START_REG  (0x4A)
#define MPU9150_SLAVE0_DATA_OUT_REG     (0x63)
#define MPU9150_SLAVE1_DATA_OUT_REG     (0x64)
#define MPU9150_SLAVE2_DATA_OUT_REG     (0x65)
#define MPU9150_SLAVE3_DATA_OUT_REG     (0x66)
#define MPU9150_I2C_DELAY_CTRL_REG      (0x67)
#define MPU9150_USER_CTRL_REG           (0x6A)
#define MPU9150_PWR_MGMT_1_REG          (0x6B)
#define MPU9150_PWR_MGMT_2_REG          (0x6C)
#define MPU9150_FIFO_COUNT_START_REG    (0x72)
#define MPU9150_FIFO_RW_REG             (0x74)
#define MPU9150_WHO_AM_I_REG            (0x75)
/** @} */

 /**
  * @name Compass register definitions
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
 * @name MPU9150 bitfield definitions
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

#endif /* MPU9150_REGS_H */
/** @} */
