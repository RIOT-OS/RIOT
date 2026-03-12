/*
 * Copyright (C) 2026 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#define QMI8658_REG_RESET           (0x60)
#define QMI8658_REG_WHO_AM_I        (0x00)
#define QMI8658_REG_REVISION_ID     (0x01)
#define QMI8658_REG_CTRL1           (0x02)
#define QMI8658_REG_CTRL2           (0x03)
#define QMI8658_REG_CTRL3           (0x04)
#define QMI8658_REG_CTRL5           (0x06)
#define QMI8658_REG_CTRL7           (0x08)
#define QMI8658_REG_CTRL8           (0x09)
#define QMI8658_REG_CTRL9           (0x0A)
#define QMI8658_REG_STATUSINT       (0x2D)
#define QMI8658_REG_STATUS0         (0x2E)
#define QMI8658_REG_STATUS1         (0x2F)
/* Temperature output registers */
#define QMI8658_REG_TEMP_L          (0x33)
#define QMI8658_REG_TEMP_H          (0x34)
/* Accelerometer output registers */
#define QMI8658_REG_AX_L            (0x35)
#define QMI8658_REG_AX_H            (0x36)
#define QMI8658_REG_AY_L            (0x37)
#define QMI8658_REG_AY_H            (0x38)
#define QMI8658_REG_AZ_L            (0x39)
#define QMI8658_REG_AZ_H            (0x3A)
/* Gyroscope output registers */
#define QMI8658_REG_GX_L            (0x3B)
#define QMI8658_REG_GX_H            (0x3C)
#define QMI8658_REG_GY_L            (0x3D)
#define QMI8658_REG_GY_H            (0x3E)
#define QMI8658_REG_GZ_L            (0x3F)
#define QMI8658_REG_GZ_H            (0x40)

/**
 * @name    QMI8658 register values
 * @{
 */
#define QMI8658_RESET_VALUE         (0xB0)
#define QMI8658_WHO_AM_I_VALUE      (0x05)

/**
* @name    Helper masks and offsets
* @{
*/
#define QMI8658_CTRL_FS_SHIFT           (4)
#define QMI8658_CTRL1_ADDR_AI_MASK      (1 << 6)
#define QMI8658_STATUS0_ACC_MASK        (1 << 0)
#define QMI8658_STATUS0_GYRO_MASK       (1 << 1)
#define QMI8658_TEMP_SCALE_FACTOR       (256)

/**
 * @brief   Reset wait interval in ms
 */
#define QMI8658_RESET_WAIT_MS       (15)

/**
 * @brief   Sensor IDs
*/
enum {
    QMI8658_SENSOR_ACC = 0,
    QMI8658_SENSOR_GYRO
};

#ifdef __cplusplus
}
#endif

/** @} */
