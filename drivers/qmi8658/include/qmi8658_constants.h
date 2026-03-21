/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_qmi8658
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    QMI8658 registers
 * @{
 */
#define QMI8658_REG_RESET           (0x60)      /**< Soft reset register */
#define QMI8658_REG_WHO_AM_I        (0x00)      /**< Device identifier register */
#define QMI8658_REG_CTRL1           (0x02)      /**< Serial interface, sensor enable settings */
#define QMI8658_REG_CTRL2           (0x03)      /**< Accelerometer settings */
#define QMI8658_REG_CTRL3           (0x04)      /**< Gyroscope settings */
#define QMI8658_REG_CTRL7           (0x08)      /**< Sensor enable register */
#define QMI8658_REG_STATUS0         (0x2E)      /**< Output data status register */
#define QMI8658_REG_TEMP_L          (0x33)      /**< Temperature data output (low byte) */
#define QMI8658_REG_TEMP_H          (0x34)      /**< Temperature data output (high byte) */
#define QMI8658_REG_AX_L            (0x35)      /**< X-axis acceleration data (low byte) */
#define QMI8658_REG_AX_H            (0x36)      /**< X-axis acceleration data (high byte) */
#define QMI8658_REG_AY_L            (0x37)      /**< Y-axis acceleration data (low byte) */
#define QMI8658_REG_AY_H            (0x38)      /**< Y-axis acceleration data (high byte) */
#define QMI8658_REG_AZ_L            (0x39)      /**< Z-axis acceleration data (low byte) */
#define QMI8658_REG_AZ_H            (0x3A)      /**< Z-axis acceleration data (high byte) */
#define QMI8658_REG_GX_L            (0x3B)      /**< X-axis angular rate data (low byte) */
#define QMI8658_REG_GX_H            (0x3C)      /**< X-axis angular rate data (high byte) */
#define QMI8658_REG_GY_L            (0x3D)      /**< Y-axis angular rate data (low byte) */
#define QMI8658_REG_GY_H            (0x3E)      /**< Y-axis angular rate data (high byte) */
#define QMI8658_REG_GZ_L            (0x3F)      /**< Z-axis angular rate data (low byte) */
#define QMI8658_REG_GZ_H            (0x40)      /**< Z-axis angular rate data (high byte) */
/** @} */

/**
 * @name    QMI8658 register values
 * @{
 */
#define QMI8658_RESET_VALUE         (0xB0)  /**< Value in register 0x4D after successful reset */
#define QMI8658_WHO_AM_I_VALUE      (0x05)  /**< Expected device identifier */
/** @} */

/**
 * @name    Helper masks and offsets
 * @{
 */
/** Shift of full scale part in CTRL2/3 */
#define QMI8658_CTRL_FS_SHIFT               (4)
/** Address auto increment bit in CTRL1 */
#define QMI8658_CTRL1_ADDR_AI_MASK          (1 << 6)
/** SensorDisable bit in CTRL1 */
#define QMI8658_CTRL1_SENSOR_DISABLE_MASK   (1 << 0)
/** Accelerometer data status bit in STATUS0 */
#define QMI8658_STATUS0_ACC_MASK            (1 << 0)
/** Gyroscope data status bit in STATUS0 */
#define QMI8658_STATUS0_GYRO_MASK           (1 << 1)
/** Scale factor of raw temperature data */
#define QMI8658_TEMP_SCALE_FACTOR           (256)
/** @} */

/**
 * @name   Mode transition times
 * @{
 */
/** Wait time after soft reset */
#define QMI8658_RESET_WAIT_MS           (15)
/** Worst case wait time after setting ODR in normal mode */
#define QMI8658_NORMAL_FILTER_WAIT_MS   (100)
/** Worst case wait time after setting ODR in low power mode */
#define QMI8658_LOWPWR_FILTER_WAIT_MS   (1000)
/** @} */

/**
 * @brief   Sensor IDs
 */
typedef enum {
    QMI8658_SENSOR_ACC = 0,
    QMI8658_SENSOR_GYRO
} qmi8658_sensor_id_t;

/**
 * @brief   Enable sensor flags
 */
typedef enum {
    QMI8658_DISABLE_ALL = 0,
    QMI8658_ENABLE_ACC  = 1,
    QMI8658_ENABLE_GYRO = 2
} qmi8658_enable_flag_t;

#ifdef __cplusplus
}
#endif

/** @} */
